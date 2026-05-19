// ===========================================================================
// acq_serial_dtf_m_axi.cpp
// Noyau HLS d'acquisition GNSS par corrélation croisée dans le domaine temporel.
//
// Architecture générale :
//   1. Lecture des entrées depuis la mémoire DDR via AXI Full (burst)
//   2. Construction du tableau de décalages tau et des banques PRN
//   3. Pour chaque hypothèse Doppler (fd) :
//        a. Mixage I/Q par DDS (suppression de la fréquence porteuse)
//        b. Corrélation avec la PRN pour tous les décalages tau (tuilage)
//   4. Réduction : recherche du pic de puissance et calcul du seuil de détection
//
// Paramètres clés définis dans le header :
//   N           : nombre d'échantillons par trame
//   NB_PHASES   : nombre de décalages tau testés
//   FD_START/END: plage de recherche Doppler en Hz
//   FS_INT      : fréquence d'échantillonnage (entier, pour calcul d'incrément de phase)
//   DDS_PHASE_BITS / DDS_LUT_BITS : résolution de l'oscillateur numérique
//   SEUIL_K     : coefficient de détection (max > mean * (1 + K))
// ===========================================================================

#include "acq_serial_dtf_m_axi.h"
#include "dds_lut_rom.h"

// Largeur du tuile de décalages tau traités en parallèle dans process_tau_tile_v9.
// Augmenter TAU_TILE améliore le débit mais consomme plus de ressources (DSP, LUT).
static const int TAU_TILE = 8;

// ========================================================
// INPUT DATAFLOW (AXI FULL) : lecture memoire -> streams -> buffers
// ========================================================

// Lit N échantillons depuis les pointeurs AXI Full (DDR) et les envoie dans deux
// streams HLS. L'interface m_axi génère automatiquement des transferts en rafale
// (burst AXI) grâce au pattern d'accès séquentiel avec II=1.
static void read_inputs_to_stream_m_axi(
    const mem_word_t *rx_real,
    const mem_word_t *prn_in,
    hls::stream<data_t> &sig_s,
    hls::stream<ap_uint<1> > &prn_s
) {
#pragma HLS INLINE off

    READ_INPUTS: for (int i = 0; i < N; i++) {
#pragma HLS PIPELINE II=1  // Initiation interval = 1 : un échantillon traité par cycle
        mem_word_t rxw  = rx_real[i];
        mem_word_t prnw = prn_in[i];

        sig_s.write((data_t)rxw);
        // Binarisation du chip PRN : toute valeur > 0 → 1, sinon → 0
        prn_s.write((prnw > 0) ? (ap_uint<1>)1 : (ap_uint<1>)0);
    }
}

// Transfère les données des streams internes vers les buffers BRAM locaux.
// La séparation read/store est imposée par le modèle DATAFLOW qui nécessite
// des fonctions producteur/consommateur distinctes pour éviter les dépendances.
static void store_stream_to_mem(
    hls::stream<data_t> &sig_s,
    hls::stream<ap_uint<1> > &prn_s,
    data_t signal_buf[N],
    ap_uint<1> prn_sign[2 * N]
) {
#pragma HLS INLINE off

    STORE_INPUTS: for (int i = 0; i < N; i++) {
#pragma HLS PIPELINE II=1
        signal_buf[i] = sig_s.read();
        // Seul le premier tiers du tableau est rempli ici (index 0..N-1) ;
        // la partie [N..2N-1] sera dupliquée plus tard dans load_inputs_once.
        prn_sign[i]   = prn_s.read();
    }
}

// Encapsule le pipeline DATAFLOW d'ingestion : la région DATAFLOW permet à
// read_inputs_to_stream_m_axi et store_stream_to_mem de s'exécuter en pipeline
// (overlap), réduisant la latence totale de lecture DDR.
static void capture_inputs_to_mem_m_axi(
    const mem_word_t *rx_real,
    const mem_word_t *prn_in,
    data_t signal_buf[N],
    ap_uint<1> prn_sign[2 * N]
) {
#pragma HLS INLINE off

    hls::stream<data_t> sig_s("sig_s");
    hls::stream<ap_uint<1> > prn_s("prn_s");
    // Depth=64 : FIFO interne suffisante pour absorber la latence de lecture DDR
    // sans bloquer le producteur.
#pragma HLS STREAM variable=sig_s depth=64
#pragma HLS STREAM variable=prn_s depth=64

#pragma HLS DATAFLOW
    read_inputs_to_stream_m_axi(rx_real, prn_in, sig_s, prn_s);
    store_stream_to_mem(sig_s, prn_s, signal_buf, prn_sign);
}

// Convertit une fréquence Doppler (Hz, entier) en incrément de phase pour le DDS.
// Formule : phase_inc = round(freq_hz / FS_INT * 2^DDS_PHASE_BITS)
// L'arrondi est géré manuellement (±FS_INT/2) pour éviter la troncature.
// La fréquence passée en argument est déjà négée par l'appelant pour réaliser
// la translation vers la bande de base.
static inline ap_int<32> hz_to_phase_inc(int freq_hz) {
#pragma HLS INLINE
    const long long SCALE = (1LL << DDS_PHASE_BITS);
    long long num = (long long)freq_hz * SCALE;
    if (num >= 0) {
        num += (FS_INT / 2);
    } else {
        num -= (FS_INT / 2);
    }
    return (ap_int<32>)(num / (long long)FS_INT);
}

// Orchestre le chargement complet des entrées :
//   - Transfert DDR → BRAM (via capture_inputs_to_mem_m_axi)
//   - Duplication de la séquence PRN dans la deuxième moitié du tableau (index N..2N-1)
//     afin de permettre des accès circulaires sans modulo dans process_tau_tile_v9.
// Les variables de diagnostic (rx_count, prn_count, etc.) sont initialisées ici
// pour être exposées via AXI-Lite au logiciel de contrôle.
static void load_inputs_once(
    const mem_word_t *rx_real,
    const mem_word_t *prn_in,
    data_t signal_buf[N],
    ap_uint<1> prn_sign[2 * N],
    int &rx_count,
    int &prn_count,
    int &rx_last_seen,
    int &prn_last_seen,
    int &rx_last_pos,
    int &prn_last_pos
) {
#pragma HLS INLINE off

    rx_count = 0;
    prn_count = 0;

    capture_inputs_to_mem_m_axi(
        rx_real,
        prn_in,
        signal_buf,
        prn_sign
    );

    rx_count      = N;
    prn_count     = N;
    rx_last_seen  = 0;  
    prn_last_seen = 0;  
    rx_last_pos   = -1;
    prn_last_pos  = -1;

    // Duplication de la PRN : prn_sign[N..2N-1] = prn_sign[0..N-1].
    // Permet à process_tau_tile_v9 d'accéder à (tau_start + n) sans débordement
    // de tableau pour les grandes valeurs de tau_start.
    DUP_PRN: for (int i = 0; i < N; i++) {
#pragma HLS PIPELINE II=1
        prn_sign[i + N] = prn_sign[i];
    }
}

// Précalcule, pour chaque hypothèse de décalage tau (0..NB_PHASES-1),
// l'index de départ dans la séquence PRN :
//   tau_start = floor(tau * N / NB_PHASES)
// Ce tableau évite de recalculer cette division dans la boucle interne critique.
static void build_tau_start_table(int tau_start_tbl[NB_PHASES]) {
#pragma HLS INLINE off

    BUILD_TAU_START: for (int tau = 0; tau < NB_PHASES; tau++) {
#pragma HLS PIPELINE II=1
        tau_start_tbl[tau] = (int)(((long long)tau * (long long)N) / NB_PHASES);
    }
}

// Réplique TAU_TILE fois la séquence PRN dans un tableau 2D [TAU_TILE][2*N].
// Chaque "banque" k contient une copie identique de prn_sign, mais leur
// partitionnement en mémoires séparées (ARRAY_PARTITION complete dim=1)
// permet des accès simultanés aux TAU_TILE banques dans la boucle UNROLL.
static void build_prn_banks(
    const ap_uint<1> prn_sign[2 * N],
    ap_uint<1> prn_banks[TAU_TILE][2 * N]
) {
#pragma HLS INLINE off

    BUILD_PRN_BANKS: for (int i = 0; i < 2 * N; i++) {
#pragma HLS PIPELINE II=1
        COPY_BANKS: for (int k = 0; k < TAU_TILE; k++) {
#pragma HLS UNROLL  // TAU_TILE copies écrites en parallèle au même cycle
            prn_banks[k][i] = prn_sign[i];
        }
    }
}

// Réalise la translation en fréquence (mixage I/Q) pour une hypothèse Doppler fd_hz.
// La fréquence totale annulée est -(fc + fd_hz), ce qui ramène le signal en bande de base.
// Le DDS est implémenté par accumulation de phase et lecture dans les tables cosinus/sinus
// (DDS_COS_LUT, DDS_SIN_LUT), dont la précision dépend de DDS_LUT_BITS.
// DEPENDENCE inter false : informe HLS qu'il n'y a pas de dépendance entre les
// itérations sur mixI_loc et mixQ_loc (tableaux locaux, accès séquentiels).
static void doppler_mixer_to_mem(
    const data_t signal_buf[N],
    osc_t mixI_loc[N],
    osc_t mixQ_loc[N],
    int fd_hz
) {
#pragma HLS INLINE off

    const ap_int<32> phase_inc = hz_to_phase_inc(-(FREQUENCE_CENTRALE_HZ + fd_hz));
    ap_uint<32> phase_acc = 0;

    MIX_ALL: for (int n = 0; n < N; n++) {
#pragma HLS PIPELINE II=1
#pragma HLS DEPENDENCE variable=mixI_loc inter false
#pragma HLS DEPENDENCE variable=mixQ_loc inter false
        // Extraction des DDS_LUT_BITS bits de poids fort pour l'index LUT
        ap_uint<DDS_LUT_BITS> lut_idx =
            phase_acc.range(DDS_PHASE_BITS - 1, DDS_PHASE_BITS - DDS_LUT_BITS);

        osc_t c = DDS_COS_LUT[(int)lut_idx];
        osc_t s = DDS_SIN_LUT[(int)lut_idx];
        osc_t x = (osc_t)signal_buf[n];

        // Démodulation en quadrature : I = x*cos, Q = -x*sin
        mixI_loc[n] = x * c;
        mixQ_loc[n] = -(x * s);
        // Accumulation de phase modulo 2^32 (overflow naturel des entiers non signés)
        phase_acc   = (ap_uint<32>)(phase_acc + (ap_uint<32>)phase_inc);
    }
}

// Calcule la corrélation I/Q pour un tuile de TAU_TILE décalages tau consécutifs.
// Stratégie d'optimisation HLS :
//   - ARRAY_PARTITION complete sur dim=1 (tau) et dim=2 (pairs/impairs) :
//     tous les accumulateurs sont dans des registres distincts, accessibles en parallèle.
//   - UNROLL sur TAU_TILE_LOOP : les TAU_TILE corrélations avancent en parallèle
//     à chaque cycle de SAMPLE_LOOP (pipeliné II=1).
//   - Double accumulateur (index 0=pairs, 1=impairs) : technique de réduction de
//     dépendance de données (évite une chaîne d'addition critique de longueur N).
// La puissance est calculée en fin de tuile : P = I²+Q² (pas de sqrt, comparaison
// de puissance suffisante pour la détection de pic).
static void process_tau_tile_v9(
    const osc_t mixI_loc[N],
    const osc_t mixQ_loc[N],
    const ap_uint<1> prn_banks[TAU_TILE][2 * N],
    const int tau_start_tbl[NB_PHASES],
    hls::stream<pow_pkt_t> &pow_out,
    int fd_idx,
    int tau_base
) {
#pragma HLS INLINE off

    acc_t accI[TAU_TILE][2];
    acc_t accQ[TAU_TILE][2];
    // Partition complète : 2*TAU_TILE accumulateurs I et Q dans des registres séparés
#pragma HLS ARRAY_PARTITION variable=accI complete dim=1
#pragma HLS ARRAY_PARTITION variable=accI complete dim=2
#pragma HLS ARRAY_PARTITION variable=accQ complete dim=1
#pragma HLS ARRAY_PARTITION variable=accQ complete dim=2

    // Initialisation de tous les accumulateurs à zéro (déroulée entièrement)
    INIT_TILE: for (int k = 0; k < TAU_TILE; k++) {
#pragma HLS UNROLL
        accI[k][0] = 0;
        accI[k][1] = 0;
        accQ[k][0] = 0;
        accQ[k][1] = 0;
    }

    // Boucle principale de corrélation : pour chaque échantillon n,
    // accumulation simultanée sur TAU_TILE décalages tau grâce à UNROLL.
    // Le chip PRN est lu depuis la banque k (accès indépendants grâce à ARRAY_PARTITION).
    // La multiplication par le chip se réduit à un changement de signe (PRN binaire ±1).
    SAMPLE_LOOP: for (int n = 0; n < N; n++) {
#pragma HLS PIPELINE II=1
        osc_t base_i = mixI_loc[n];
        osc_t base_q = mixQ_loc[n];
        bool odd = (n & 1) != 0;  // Sélection de l'accumulateur pair ou impair

        TAU_TILE_LOOP: for (int k = 0; k < TAU_TILE; k++) {
#pragma HLS UNROLL
            int tau = tau_base + k;
            if (tau < NB_PHASES) {
                osc_t i = base_i;
                osc_t q = base_q;
                // Lecture du chip PRN à la position (tau_start + n) dans la séquence dupliquée
                ap_uint<1> s = prn_banks[k][tau_start_tbl[tau] + n];

                // Chip = 0 → symbole -1 → inversion de signe (multiplication implicite)
                if (!s) {
                    i = -i;
                    q = -q;
                }

                // Accumulation sur l'accumulateur pair ou impair pour casser la chaîne de dépendance
                if (odd) {
                    accI[k][1] += i;
                    accQ[k][1] += q;
                } else {
                    accI[k][0] += i;
                    accQ[k][0] += q;
                }
            }
        }
    }

    // Réduction finale : sommation des deux accumulateurs et calcul de la puissance P = I²+Q².
    // Chaque paquet pow_pkt_t est émis dans le stream avec ses coordonnées (tau, fd_idx)
    // pour reconstruction de la matrice de corrélation par reduce_all_powers.
    OUTPUT_TILE: for (int k = 0; k < TAU_TILE; k++) {
#pragma HLS PIPELINE II=1
        int tau = tau_base + k;
        if (tau < NB_PHASES) {
            acc_t accI_total = accI[k][0] + accI[k][1];
            acc_t accQ_total = accQ[k][0] + accQ[k][1];

            pow_pkt_t p;
            p.power  = (power_t)(accI_total * accI_total + accQ_total * accQ_total);
            p.tau    = tau;
            p.fd_idx = fd_idx;
            pow_out.write(p);
        }
    }
}

// Parcourt tous les décalages tau par groupes de TAU_TILE et délègue à process_tau_tile_v9.
// Le LOOP_TRIPCOUNT informe HLS de la valeur nominale pour les rapports de timing
// (NB_PHASES / TAU_TILE = 128 itérations pour les paramètres par défaut).
static void process_tau_df_v9(
    const osc_t mixI_loc[N],
    const osc_t mixQ_loc[N],
    const ap_uint<1> prn_banks[TAU_TILE][2 * N],
    const int tau_start_tbl[NB_PHASES],
    hls::stream<pow_pkt_t> &pow_out,
    int fd_idx
) {
#pragma HLS INLINE off

    TAU_TILE_LOOP: for (int tau_base = 0; tau_base < NB_PHASES; tau_base += TAU_TILE) {
#pragma HLS LOOP_TRIPCOUNT min=128 max=128 avg=128
        process_tau_tile_v9(mixI_loc, mixQ_loc, prn_banks, tau_start_tbl, pow_out, fd_idx, tau_base);
    }
}

// Traite une hypothèse Doppler complète :
//   1. Mixage I/Q → buffers BRAM locaux (mixI_loc, mixQ_loc)
//   2. Corrélation sur tous les tau → émission dans pow_out
// La partition cyclique factor=2 sur mixI_loc/mixQ_loc permet des lectures
// simultanées sur les index pairs et impairs dans SAMPLE_LOOP (II=1).
static void process_one_fd(
    const data_t signal_buf[N],
    const ap_uint<1> prn_banks[TAU_TILE][2 * N],
    const int tau_start_tbl[NB_PHASES],
    hls::stream<pow_pkt_t> &pow_out,
    int fd_idx,
    int fd_step
) {
#pragma HLS INLINE off

    osc_t mixI_loc[N];
    osc_t mixQ_loc[N];
    // Partition cyclique factor=2 : deux ports de lecture distincts pour accès pair/impair
#pragma HLS ARRAY_PARTITION variable=mixI_loc cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=mixQ_loc cyclic factor=2 dim=1

    int fd_hz = FD_START + fd_idx * fd_step;

    doppler_mixer_to_mem(signal_buf, mixI_loc, mixQ_loc, fd_hz);
    process_tau_df_v9(mixI_loc, mixQ_loc, prn_banks, tau_start_tbl, pow_out, fd_idx);
}

// Producteur DATAFLOW : itère sur toutes les hypothèses Doppler (0..nb_fd-1)
// et alimente le stream pow_s avec les puissances de corrélation.
// LOOP_TRIPCOUNT : bornes min/max pour le rapport de synthèse HLS (valeur max = 21 Doppler bins).
static void produce_all_fd_powers(
    const data_t signal_buf[N],
    const ap_uint<1> prn_banks[TAU_TILE][2 * N],
    const int tau_start_tbl[NB_PHASES],
    int nb_fd,
    int fd_step,
    hls::stream<pow_pkt_t> &pow_s
) {
#pragma HLS INLINE off

    FD_LOOP: for (int fd_idx = 0; fd_idx < nb_fd; fd_idx++) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=21 avg=11
        process_one_fd(signal_buf, prn_banks, tau_start_tbl, pow_s, fd_idx, fd_step);
    }
}

// Consommateur DATAFLOW : lit tous les paquets de puissance depuis pow_s,
// les écrit en DDR (corr_out) et calcule simultanément :
//   - max_val  : puissance maximale (pic de corrélation)
//   - sum_corr : somme totale (pour calcul de la puissance moyenne)
//   - best_tau / best_fd_idx : coordonnées du pic (hypothèse retenue)
// L'opération se fait en un seul passage (II=1) sans second scan de la matrice.
static void reduce_all_powers(
    hls::stream<pow_pkt_t> &pow_s,
    mem_word_t *corr_out,
    int nb_fd,
    power_t &max_val,
    power_t &sum_corr,
    int &best_tau,
    int &best_fd_idx,
    int &corr_count
) {
#pragma HLS INLINE off

    const int total = nb_fd * NB_PHASES;  // Taille totale de la matrice tau × fd

    REDUCE_ALL: for (int i = 0; i < total; i++) {
#pragma HLS PIPELINE II=1
        pow_pkt_t p = pow_s.read();

        sum_corr += p.power;
        if (p.power > max_val) {
            max_val     = p.power;
            best_tau    = p.tau;
            best_fd_idx = p.fd_idx;
        }

        corr_out[i] = (mem_word_t)p.power;  // Écriture de la matrice complète en DDR
    }

    corr_count = total;
}

// Région DATAFLOW principale pour le traitement Doppler×tau.
// produce_all_fd_powers et reduce_all_powers s'exécutent en pipeline :
// dès que le premier paquet est produit, le consommateur commence la réduction,
// sans attendre la fin de la production complète.
// Le stream pow_s (depth=64) joue le rôle de FIFO de découplage entre producteur et consommateur.
static void run_fd_dataflow_region(
    const data_t signal_buf[N],
    const ap_uint<1> prn_banks[TAU_TILE][2 * N],
    const int tau_start_tbl[NB_PHASES],
    mem_word_t *corr_out,
    int nb_fd,
    int fd_step,
    power_t &max_val,
    power_t &sum_corr,
    int &best_tau,
    int &best_fd_idx,
    int &corr_count
) {
#pragma HLS INLINE off

    hls::stream<pow_pkt_t> pow_s("pow_s_top");
#pragma HLS STREAM variable=pow_s depth=64

#pragma HLS DATAFLOW
    produce_all_fd_powers(signal_buf, prn_banks, tau_start_tbl, nb_fd, fd_step, pow_s);
    reduce_all_powers(pow_s, corr_out, nb_fd, max_val, sum_corr, best_tau, best_fd_idx, corr_count);
}

// ===========================================================================
// Fonction top-level synthétisable.
//
// Interfaces matérielles :
//   - m_axi (GMEM0/1/2) : accès burst DDR pour rx_real, prn_in, corr_out
//   - s_axilite (CTRL)  : registres de configuration et de statut accessibles
//                         depuis le PS (Zynq/MicroBlaze) via AXI-Lite
//
// Buffers statiques (persistants entre appels, mappés en BRAM) :
//   - signal_buf  : trame I/Q reçue (N échantillons)
//   - prn_sign    : séquence PRN binarisée, dupliquée (2N chips)
//   - prn_banks   : TAU_TILE copies de prn_sign pour accès parallèle
//   - tau_start_tbl : index de départ dans PRN pour chaque hypothèse tau
//
// Sortie de détection :
//   - sat_detected = 1 si max_power > mean_power * (1 + SEUIL_K)
//   - doppler_out / codephase_out : hypothèses retenues au pic de corrélation
// ===========================================================================
void acquisition_serial_m_axi(
    const mem_word_t *rx_real,
    const mem_word_t *prn_in,
    mem_word_t *corr_out,
    int &corr_count,
    int &doppler_out,
    int &codephase_out,
    int &sat_detected,
    int fd_step,
    int &max_power_out,
    int &mean_power_out,
    int &rx_count,
    int &prn_count,
    int &rx_last_seen,
    int &prn_last_seen,
    int &rx_last_pos,
    int &prn_last_pos
) {
    // Interfaces AXI Full pour les transferts en rafale vers/depuis la DDR
#pragma HLS INTERFACE m_axi port=rx_real  offset=slave bundle=GMEM0 depth=N
#pragma HLS INTERFACE m_axi port=prn_in   offset=slave bundle=GMEM1 depth=N
#pragma HLS INTERFACE m_axi port=corr_out offset=slave bundle=GMEM2

    // Tous les scalaires et pointeurs sont exposés comme registres AXI-Lite
    // dans le bundle CTRL, configurable depuis le logiciel de contrôle.
#pragma HLS INTERFACE s_axilite port=rx_real        bundle=CTRL
#pragma HLS INTERFACE s_axilite port=prn_in         bundle=CTRL
#pragma HLS INTERFACE s_axilite port=corr_out       bundle=CTRL
#pragma HLS INTERFACE s_axilite port=corr_count     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=doppler_out    bundle=CTRL
#pragma HLS INTERFACE s_axilite port=codephase_out  bundle=CTRL
#pragma HLS INTERFACE s_axilite port=sat_detected   bundle=CTRL
#pragma HLS INTERFACE s_axilite port=fd_step        bundle=CTRL
#pragma HLS INTERFACE s_axilite port=max_power_out  bundle=CTRL
#pragma HLS INTERFACE s_axilite port=mean_power_out bundle=CTRL
#pragma HLS INTERFACE s_axilite port=rx_count       bundle=CTRL
#pragma HLS INTERFACE s_axilite port=prn_count      bundle=CTRL
#pragma HLS INTERFACE s_axilite port=rx_last_seen   bundle=CTRL
#pragma HLS INTERFACE s_axilite port=prn_last_seen  bundle=CTRL
#pragma HLS INTERFACE s_axilite port=rx_last_pos    bundle=CTRL
#pragma HLS INTERFACE s_axilite port=prn_last_pos   bundle=CTRL
#pragma HLS INTERFACE s_axilite port=return         bundle=CTRL

    // Buffers statiques → synthétisés en BRAM, conservent leur valeur entre appels.
    // prn_sign alloué à 2*N pour la duplication circulaire (cf. load_inputs_once).
    static data_t signal_buf[N];
    static ap_uint<1> prn_sign[2 * N];
    static ap_uint<1> prn_banks[TAU_TILE][2 * N];
    static int tau_start_tbl[NB_PHASES];
    // Partition cyclique factor=2 : deux banques BRAM pour accès pair/impair simultanés
#pragma HLS ARRAY_PARTITION variable=prn_sign cyclic factor=2 dim=1
    // Partition complète sur dim=1 : TAU_TILE banques BRAM indépendantes (accès parallèle total)
#pragma HLS ARRAY_PARTITION variable=prn_banks complete dim=1

    // Sécurité : fd_step ≤ 0 invalide (évite division par zéro et boucles infinies)
    if (fd_step <= 0) {
        fd_step = 1;
    }

    // Nombre d'hypothèses Doppler à tester dans la plage [FD_START, FD_END]
    int nb_fd = (FD_END - FD_START) / fd_step + 1;

    power_t max_val  = 0;
    power_t sum_corr = 0;
    int best_tau     = 0;
    int best_fd_idx  = 0;

    corr_count = 0;

    // Étape 1 : chargement DDR → BRAM + duplication PRN
    load_inputs_once(
        rx_real,
        prn_in,
        signal_buf,
        prn_sign,
        rx_count,
        prn_count,
        rx_last_seen,
        prn_last_seen,
        rx_last_pos,
        prn_last_pos
    );

    // Étape 2 : précalcul des tables (tau_start et banques PRN)
    build_tau_start_table(tau_start_tbl);
    build_prn_banks(prn_sign, prn_banks);

    // Étape 3 : corrélation complète sur la grille (fd × tau) en mode DATAFLOW
    run_fd_dataflow_region(
        signal_buf,
        prn_banks,
        tau_start_tbl,
        corr_out,
        nb_fd,
        fd_step,
        max_val,
        sum_corr,
        best_tau,
        best_fd_idx,
        corr_count
    );

    // Étape 4 : calcul du seuil CFAR simplifié et décision de détection.
    // Le seuil est proportionnel à la puissance moyenne : threshold = mean * (1 + K).
    // Si le pic dépasse ce seuil, le satellite est considéré comme détecté.
    int total_pts = nb_fd * NB_PHASES;
    power_t mean  = sum_corr / total_pts;

    power_t threshold = mean * (power_t)(1.0 + SEUIL_K);
    sat_detected = (max_val > threshold) ? 1 : 0;

    // Export des résultats vers les registres AXI-Lite
    max_power_out  = (int)max_val;
    mean_power_out = (int)mean;
    doppler_out    = FD_START + best_fd_idx * fd_step;  // Conversion index → Hz
    codephase_out  = best_tau;
}