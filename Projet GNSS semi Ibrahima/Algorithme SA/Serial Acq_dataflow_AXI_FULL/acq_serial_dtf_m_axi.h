// ===========================================================================
// acq_serial_dtf_m_axi.h
// Header principal du noyau HLS d'acquisition GNSS par corrélation temporelle.
//
// Ce fichier centralise :
//   - Les paramètres fixes de la trame (N, FS, fréquence porteuse, seuils)
//   - Les paramètres adaptatifs selon le mode de compilation (csim vs synthèse)
//   - La configuration de l'oscillateur numérique (DDS)
//   - Les types de données HLS utilisés dans tout le pipeline
//   - Les structures de paquets échangés entre fonctions (streams HLS)
//   - La déclaration de la fonction top-level synthétisable
// ===========================================================================

#ifndef ACQ_SERIAL_DTFLOW_H
#define ACQ_SERIAL_DTFLOW_H

#include <hls_stream.h>   // hls::stream<T> : FIFO matérielles entre fonctions DATAFLOW
#include <ap_int.h>       // ap_int<N>, ap_uint<N> : entiers à largeur de bits arbitraire
#include <ap_fixed.h>     // ap_fixed<W,I> : virgule fixe (W bits totaux, I bits entiers)
#include <hls_math.h>     // Fonctions mathématiques synthétisables (sin, cos, sqrt, etc.)

// ---------------------------------------------------------------------------
// Paramètres fixes — identiques en csim et en synthèse
// ---------------------------------------------------------------------------

// Nombre d'échantillons par trame de corrélation.
// Correspond à 1 ms de signal GPS à FS = 11.999 MHz (légèrement sous-échantillonné
// pour couvrir exactement 1 période de code C/A à 1.023 MHz).
#define N 11999

// Fréquence d'échantillonnage en virgule flottante (Hz) — utilisée pour les calculs
// analytiques hors synthèse (Python, MATLAB, vérification csim).
#define FS 11999000.0

// Fréquence centrale IF du signal reçu après descente en fréquence analogique (Hz).
// Le DDS annule cette fréquence + l'hypothèse Doppler pour ramener le signal en bande de base.
#define FREQUENCE_CENTRALE 3563000.0

// Seuil de détection par ratio pic/moyenne (non utilisé dans la version courante,
// remplacé par SEUIL_K ci-dessous).
#define SEUIL_RATIO 2.5

// Coefficient CFAR simplifié : détection si max_power > mean_power * (1 + SEUIL_K).
// Valeur élevée (3.0) pour limiter les fausses alarmes.
#define SEUIL_K 3.0

// ---------------------------------------------------------------------------
// Paramètres adaptatifs — plage Doppler différente selon le contexte
//
// En synthèse (__SYNTHESIS__) : plage complète ±10 kHz pour la production FPGA.
// En csim (simulation C)      : plage réduite [-2500, -1000] Hz pour accélérer
//                               la simulation (pas de RTL, mais N×NB_PHASES×nb_fd
//                               opérations restent coûteuses en C).
// ---------------------------------------------------------------------------
#ifdef __SYNTHESIS__
    // Plage de recherche Doppler complète pour l'acquisition GPS (Hz)
    #define FD_START -10000
    #define FD_END   10000
    #define NB_PHASES 1023  // Nombre de chips du code C/A GPS (1 période = 1023 chips)
#else
    // Plage réduite pour la csim : couvre la zone attendue du vrai Doppler dans le jeu de test
    #define FD_START -2500
    #define FD_END   -1000
    #define NB_PHASES 1023
#endif

// ---------------------------------------------------------------------------
// Paramètres de l'oscillateur numérique (DDS — Direct Digital Synthesizer)
// ---------------------------------------------------------------------------

// Résolution de la LUT trigonométrique : 2^10 = 1024 entrées.
// Compromis entre précision de phase et surface BRAM/ROM consommée.
#define DDS_LUT_BITS           10
#define DDS_LUT_SIZE           (1 << DDS_LUT_BITS)  // = 1024 entrées cos/sin

// Résolution de l'accumulateur de phase : 2^32 pas par tour.
// L'incrément de phase est calculé par hz_to_phase_inc() selon : inc = freq/FS * 2^32.
#define DDS_PHASE_BITS         32

// Versions entières des constantes analogiques — utilisées dans hz_to_phase_inc()
// pour éviter la virgule flottante en synthèse (division entière long long).
#define FREQUENCE_CENTRALE_HZ  3563000   // Fréquence IF en Hz (entier)
#define FS_INT                 11999000  // Fréquence d'échantillonnage en Hz (entier)

// Nombre effectif de décalages tau évalués (alias de NB_PHASES, prévu pour
// permettre une restriction partielle de la recherche sans modifier NB_PHASES).
#define NB_PHASES_TO_CHECK NB_PHASES

// Pas angulaire en radians par échantillon à FS (2π/FS).
// Pré-calculé en virgule flottante pour usage éventuel en csim ou post-traitement.
#define TWO_PI_DIV_FS 0.000000523641347317

// Garde de compilation pour activer/désactiver les signaux de debug TLAST
// dans les streams (utile pour vérifier la délimitation de trames en co-simulation RTL).
#ifndef ACQ_ENABLE_TLAST_DEBUG
#define ACQ_ENABLE_TLAST_DEBUG 0
#endif

// ---------------------------------------------------------------------------
// Types de données HLS
//
// Le choix de la virgule fixe (ap_fixed) plutôt que float/double est essentiel
// pour la synthèse : HLS mappe directement ces types sur des DSP48 FPGA avec
// une latence et une surface prévisibles.
// ---------------------------------------------------------------------------

// Type de mot mémoire pour les interfaces AXI Full (DDR).
// ap_int<32> correspond à un mot 32 bits aligné, compatible avec les bus AXI4.
typedef ap_int<32> mem_word_t;

// Échantillon du signal reçu après normalisation ±1 (BPSK).
// 2 bits signés suffisent pour représenter {-1, 0, +1}.
typedef ap_int<2> data_t;

// Type des échantillons I/Q après mixage DDS.
// ap_fixed<24,8> : 8 bits partie entière + 16 bits fraction → précision suffisante
// pour les accumulations de corrélation sans débordement sur N=11999 échantillons.
typedef ap_fixed<24, 8> osc_t;

// Type intermédiaire pour les angles de phase (non utilisé dans la version DDS-LUT courante).
typedef ap_fixed<32, 6> angle_t;

// Type pour les valeurs trigonométriques LUT (cos/sin).
// ap_fixed<28,2> : plage [-2, 2) avec 26 bits de fraction, cohérent avec une LUT normalisée.
typedef ap_fixed<28, 2> trig_t;

// Accumulateur de corrélation I ou Q.
// ap_fixed<32,24> : plage ±2^23, suffisant pour accumuler N=11999 valeurs de osc_t (max ~8M).
typedef ap_fixed<32, 24> acc_t;

// Type de puissance de corrélation : P = I² + Q².
// ap_fixed<64,32> : large dynamique pour éviter tout débordement sur le carré des accumulateurs.
typedef ap_fixed<64, 32> power_t;

// Types pour l'accumulateur de phase DDS.
// phase_u_t (non signé) : overflow naturel modulo 2^32 = rotation de phase circulaire.
// phase_t   (signé)     : utilisé pour l'incrément de phase (peut être négatif → fréquence négative).
typedef ap_uint<32> phase_u_t;
typedef ap_int<32>  phase_t;

// ---------------------------------------------------------------------------
// Structures de paquets pour les streams HLS inter-fonctions
// ---------------------------------------------------------------------------

// Paquet signal reçu : un échantillon + flag de fin de trame (last).
// last=1 signale le dernier échantillon de la trame (protocole AXI-Stream TLAST).
struct rx_pkt_t {
    data_t     x;       // Valeur de l'échantillon normalisé
    ap_uint<1> last;    // Indicateur de fin de trame (TLAST AXI-Stream)
};

// Paquet signal mixé : composantes I/Q après translation Doppler + flag TLAST.
struct mix_pkt_t {
    osc_t      i;       // Composante en phase (I)
    osc_t      q;       // Composante en quadrature (Q)
    ap_uint<1> last;    // Indicateur de fin de trame
};

// Paquet de puissance de corrélation : résultat P=I²+Q² avec ses coordonnées
// dans la grille (tau, fd_idx). Échangé entre produce_all_fd_powers et reduce_all_powers.
struct pow_pkt_t {
    power_t power;   // Puissance de corrélation pour cette hypothèse (tau, fd_idx)
    int     tau;     // Indice de décalage de code (0..NB_PHASES-1)
    int     fd_idx;  // Indice d'hypothèse Doppler (0..nb_fd-1)
};

// ---------------------------------------------------------------------------
// Déclaration de la fonction top-level synthétisable
//
// Les arguments sont mappés sur des interfaces AXI Full (pointeurs mémoire)
// et AXI-Lite (scalaires) via les directives #pragma HLS INTERFACE dans le .cpp.
// ---------------------------------------------------------------------------
void acquisition_serial_m_axi(
    const mem_word_t *rx_real,      // [AXI Full GMEM0] Signal reçu I (N échantillons DDR)
    const mem_word_t *prn_in,       // [AXI Full GMEM1] Séquence PRN locale (N chips DDR)
    mem_word_t *corr_out,           // [AXI Full GMEM2] Matrice de corrélation en sortie (nb_fd × NB_PHASES)
    int &corr_count,                // [AXI-Lite] Nombre de valeurs écrites dans corr_out
    int &doppler_out,               // [AXI-Lite] Fréquence Doppler estimée au pic (Hz)
    int &codephase_out,             // [AXI-Lite] Décalage de code estimé au pic (indice tau)
    int &sat_detected,              // [AXI-Lite] 1 si détection confirmée (max > seuil CFAR)
    int fd_step,                    // [AXI-Lite] Pas de recherche Doppler en Hz
    int &max_power_out,             // [AXI-Lite] Puissance du pic de corrélation
    int &mean_power_out,            // [AXI-Lite] Puissance moyenne sur toute la grille
    int &rx_count,                  // [AXI-Lite] Nombre d'échantillons signal effectivement chargés
    int &prn_count,                 // [AXI-Lite] Nombre de chips PRN effectivement chargés
    int &rx_last_seen,              // [AXI-Lite] Diagnostic : dernier rx_last_seen (debug)
    int &prn_last_seen,             // [AXI-Lite] Diagnostic : dernier prn_last_seen (debug)
    int &rx_last_pos,               // [AXI-Lite] Diagnostic : position du dernier TLAST signal
    int &prn_last_pos               // [AXI-Lite] Diagnostic : position du dernier TLAST PRN
);

#endif // ACQ_SERIAL_DTFLOW_H