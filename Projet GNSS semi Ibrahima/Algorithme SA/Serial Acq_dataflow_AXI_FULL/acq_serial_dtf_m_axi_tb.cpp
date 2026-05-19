// ===========================================================================
// Testbench C-sim pour acquisition_serial_m_axi
//
// Objectif : valider le noyau HLS en simulation fonctionnelle (csim) avant
// synthèse RTL. Le testbench charge des données réelles depuis des fichiers CSV
// (signal reçu et séquence PRN), appelle le DUT, puis vérifie la cohérence
// des sorties en recalculant le pic de corrélation côté logiciel.
//
// Structure :
//   1. Utilitaires de chargement CSV (parse_int_token, load_csv_int32_*)
//   2. Préparation des entrées (normalisation signal, copie PRN)
//   3. Appel du DUT (acquisition_serial_m_axi)
//   4. Vérification croisée DUT vs. recalcul SW (doppler, phase, puissance max)
// ===========================================================================

#include "acq_serial_dtf_m_axi.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cctype>
#include <cmath>
#include <direct.h>  // _getcwd (Windows MSVC/MinGW ; remplacer par unistd.h sur Linux)

// Affiche le répertoire de travail courant au lancement de la csim.
// Utile pour diagnostiquer les erreurs de chemin des fichiers CSV
// (le cwd varie selon l'IDE et la configuration Vitis HLS).
static void print_csim_cwd() {
    char cwd[2048];
    if (_getcwd(cwd, sizeof(cwd)) != nullptr) {
        std::cout << "[INFO] csim cwd = " << cwd << std::endl;
    } else {
        std::cout << "[WARN] impossible de lire le cwd." << std::endl;
    }
}

// Analyse un token CSV (chaîne) en entier 32 bits.
// Retourne false si le token est vide, non numérique, ou contient des
// caractères parasites après la valeur (ex. guillemets, séparateurs résiduels).
// L'utilisation de strtol (base 10) avec vérification du pointeur end garantit
// une détection robuste des tokens invalides, contrairement à atoi().
static bool parse_int_token(const std::string &token, int &value) {
    const char *s = token.c_str();
    // Ignorer les espaces/tabulations en tête de token
    while (*s && std::isspace((unsigned char)*s)) {
        ++s;
    }
    if (*s == '\0') {
        return false;  // Token vide ou tout-whitespace
    }

    char *end = nullptr;
    long v = std::strtol(s, &end, 10);
    if (end == s) {
        return false;  // Aucun chiffre trouvé
    }

    // Vérifier qu'il ne reste que des espaces après le nombre
    while (*end && std::isspace((unsigned char)*end)) {
        ++end;
    }
    if (*end != '\0') {
        return false;  // Caractères non numériques après la valeur
    }

    value = (int)v;
    return true;
}

// Charge un fichier CSV contenant des entiers (une ou plusieurs colonnes, plusieurs lignes)
// dans un vecteur. Retourne false si le fichier est inaccessible ou vide.
// Le parseur est tolérant : il ignore les tokens non numériques sans lever d'exception.
static bool load_csv_int32_one(const std::string &path, std::vector<int> &out) {
    std::ifstream fin(path.c_str());
    if (!fin) {
        return false;
    }

    out.clear();
    std::string line;
    while (std::getline(fin, line)) {
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ',')) {
            int v = 0;
            if (parse_int_token(token, v)) {
                out.push_back(v);
            }
        }
    }
    return !out.empty();
}

// Tentative de chargement d'un fichier CSV depuis une liste de chemins candidats.
// La stratégie multi-chemins compense les variations de cwd selon que la csim
// est lancée depuis Vitis HLS GUI, ligne de commande, ou un script Tcl.
// S'arrête et retourne true au premier chemin valide trouvé.
static bool load_csv_int32_candidates(
    const std::vector<std::string> &candidates,
    const char *label,
    std::vector<int> &out
) {
    for (size_t i = 0; i < candidates.size(); i++) {
        if (load_csv_int32_one(candidates[i], out)) {
            std::cout << "[OK] " << label << " charge depuis: " << candidates[i]
                      << " (n=" << out.size() << ")" << std::endl;
            return true;
        }
    }

    // Aucun chemin valide trouvé : affichage de tous les chemins tentés pour le debug
    std::cout << "[ERREUR] " << label << " introuvable. chemins testes:" << std::endl;
    for (size_t i = 0; i < candidates.size(); i++) {
        std::cout << "  - " << candidates[i] << std::endl;
    }
    return false;
}

int main() {
    std::cout << "==============================================" << std::endl;
    std::cout << "TB acquisition_serial_m_axi (PRN non-packe)" << std::endl;
    std::cout << "==============================================" << std::endl;
    // Rappel des constantes de configuration issues du header (dépend du mode de compilation)
    std::cout << "[INFO] N=" << N
              << " NB_PHASES=" << NB_PHASES
              << " FD_START=" << FD_START
              << " FD_END=" << FD_END << std::endl;

    // En csim (non synthèse), les paramètres proviennent du header ;
    // en mode CSIM_FAST, des constantes réduites peuvent être utilisées pour accélérer la sim.
#ifndef __SYNTHESIS__
    std::cout << "[WARN] C-sim utilise la config du header (normal ou CSIM_FAST)." << std::endl;
#endif

    print_csim_cwd();

    std::vector<int> sig_vec;
    std::vector<int> prn_vec;

    // Chemins candidats pour le signal reçu (relatifs au cwd potentiel de la csim)
    std::vector<std::string> sig_paths;
    sig_paths.push_back("signal.csv");
    sig_paths.push_back("../signal.csv");
    sig_paths.push_back("../../signal.csv");
    sig_paths.push_back("../../../signal.csv");

    // Chemins candidats pour la séquence PRN du satellite 25 (Gold code GPS)
    std::vector<std::string> prn_paths;
    prn_paths.push_back("PRN-25.csv");
    prn_paths.push_back("../PRN-25.csv");
    prn_paths.push_back("../../PRN-25.csv");
    prn_paths.push_back("../../../PRN-25.csv");

    bool ok_sig = load_csv_int32_candidates(sig_paths, "signal.csv", sig_vec);
    bool ok_prn = load_csv_int32_candidates(prn_paths, "PRN-25.csv", prn_vec);

    if (!ok_sig || !ok_prn) {
        std::cerr << "[ECHEC] Entrees indisponibles." << std::endl;
        return 1;
    }

    // Vérification que les fichiers contiennent au moins N échantillons (taille de trame)
    if ((int)sig_vec.size() < N || (int)prn_vec.size() < N) {
        std::cerr << "[ERREUR] taille insuffisante: signal=" << sig_vec.size()
                  << " prn=" << prn_vec.size() << " N=" << N << std::endl;
        return 1;
    }

    // Buffers d'entrée statiques → allocation sur le segment statique (évite le débordement de pile
    // pour les grandes valeurs de N). mem_word_t correspond à ap_int<32> en synthèse HLS.
    static mem_word_t rx_real[N];
    static mem_word_t prn_in[N];

    // Paramètre de recherche Doppler : pas de 250 Hz, couvrant [FD_START, FD_END]
    const int fd_step = 250;
    const int nb_fd = (FD_END - FD_START) / fd_step + 1;
    const int total_corr = nb_fd * NB_PHASES;  // Taille totale de la matrice de corrélation

    // Buffer de sortie pour la matrice de corrélation complète (nb_fd × NB_PHASES)
    std::vector<mem_word_t> corr_out_vec(total_corr, 0);
    mem_word_t *corr_out = corr_out_vec.data();

    // Normalisation des entrées :
    //   - Signal : binarisation ±1 (signal BPSK), avec gestion du code spécial 255 → -1
    //     (convention CSV où 255 représente -1 sur 8 bits non signés)
    //   - PRN : transmis tel quel au DUT (la binarisation est faite dans le noyau HLS)
    for (int i = 0; i < N; i++) {
        int s = sig_vec[i];
        if (s == 255) {
            s = -1;  // Recodage de la valeur 0xFF (uint8 débordé) en -1 signé
        }

        // PRN brute, meme convention que ton flux precedent
        int p = prn_vec[i];

        rx_real[i] = (s > 0) ? mem_word_t(1) : mem_word_t(-1);
        prn_in[i]  = (mem_word_t)p;
    }

    // Déclaration des sorties scalaires du DUT (exposées via AXI-Lite dans le noyau HLS)
    int corr_count     = 0;
    int doppler_out    = 0;
    int codephase_out  = 0;
    int sat_detected   = 0;
    int max_power_out  = 0;
    int mean_power_out = 0;

    // Variables de diagnostic internes au DUT (compteurs de chargement, positions debug)
    int rx_count       = 0;
    int prn_count      = 0;
    int rx_last_seen   = 0;
    int prn_last_seen  = 0;
    int rx_last_pos    = -1;
    int prn_last_pos   = -1;

    // -----------------------------------------------------------------------
    // Appel du DUT (Device Under Test)
    // En csim, cet appel est une simulation C pure (pas de RTL).
    // Les arguments correspondent exactement aux ports AXI-Lite et AXI Full
    // déclarés dans le noyau HLS.
    // -----------------------------------------------------------------------
    acquisition_serial_m_axi(
        rx_real,
        prn_in,
        corr_out,
        corr_count,
        doppler_out,
        codephase_out,
        sat_detected,
        fd_step,
        max_power_out,
        mean_power_out,
        rx_count,
        prn_count,
        rx_last_seen,
        prn_last_seen,
        rx_last_pos,
        prn_last_pos
    );

    bool ok = true;

    // Vérification des compteurs de chargement : le DUT doit avoir consommé exactement N échantillons
    if (rx_count != N) {
        std::cerr << "[ERREUR] rx_count=" << rx_count << " attendu=" << N << std::endl;
        ok = false;
    }
    if (prn_count != N) {
        std::cerr << "[ERREUR] prn_count=" << prn_count << " attendu=" << N << std::endl;
        ok = false;
    }
    // La matrice de corrélation doit contenir exactement nb_fd × NB_PHASES valeurs
    if (corr_count != total_corr) {
        std::cerr << "[ERREUR] corr_count=" << corr_count << " attendu=" << total_corr << std::endl;
        ok = false;
    }

    // -----------------------------------------------------------------------
    // Recalcul logiciel du pic de corrélation (référence SW indépendante du DUT).
    // Parcours linéaire de corr_out : l'index est converti en (fd_idx, tau) par
    //   fd_idx = best_idx / NB_PHASES
    //   tau    = best_idx % NB_PHASES
    // Ce recalcul permet de vérifier que le DUT rapporte bien les bonnes coordonnées
    // du pic, indépendamment de son algorithme de réduction interne.
    // -----------------------------------------------------------------------
    int best_idx = 0;
    mem_word_t best_val_sw = corr_out[0];
    for (int i = 1; i < total_corr; i++) {
        if (corr_out[i] > best_val_sw) {
            best_val_sw = corr_out[i];
            best_idx = i;
        }
    }

    // Reconstruction des coordonnées physiques depuis l'index linéaire
    int best_fd_idx_sw = best_idx / NB_PHASES;
    int best_tau_sw = best_idx % NB_PHASES;
    int doppler_sw = FD_START + best_fd_idx_sw * fd_step;  // Conversion index → Hz

    // Affichage des sorties DUT brutes
    std::cout << "\n[RESULTATS DUT]" << std::endl;
    std::cout << "doppler_out    = " << doppler_out << std::endl;
    std::cout << "codephase_out  = " << codephase_out << std::endl;
    std::cout << "max_power_out  = " << max_power_out << std::endl;
    std::cout << "mean_power_out = " << mean_power_out << std::endl;
    std::cout << "sat_detected   = " << sat_detected << std::endl;
    std::cout << "rx_count       = " << rx_count << std::endl;
    std::cout << "prn_count      = " << prn_count << std::endl;
    std::cout << "corr_count     = " << corr_count << std::endl;
    std::cout << "rx_last_seen   = " << rx_last_seen << std::endl;
    std::cout << "prn_last_seen  = " << prn_last_seen << std::endl;
    std::cout << "rx_last_pos    = " << rx_last_pos << std::endl;
    std::cout << "prn_last_pos   = " << prn_last_pos << std::endl;

    // Affichage du résultat SW de référence pour comparaison visuelle
    std::cout << "\n[CHECK corr_out]" << std::endl;
    std::cout << "best_idx_sw    = " << best_idx << std::endl;
    std::cout << "best_val_sw    = " << (int)best_val_sw << std::endl;
    std::cout << "doppler_sw     = " << doppler_sw << std::endl;
    std::cout << "phase_sw       = " << best_tau_sw << std::endl;

    // -----------------------------------------------------------------------
    // Vérifications croisées DUT vs. SW :
    //   - doppler_out  : fréquence Doppler estimée (Hz) doit correspondre au pic SW
    //   - codephase_out: décalage tau retenu doit correspondre au pic SW
    //   - max_power_out: puissance du pic doit être identique à la valeur SW
    // Un écart indique un bug dans la logique de réduction du DUT (reduce_all_powers).
    // -----------------------------------------------------------------------
    if (doppler_out != doppler_sw) {
        std::cerr << "[ERREUR] doppler_out != doppler_sw" << std::endl;
        ok = false;
    }
    if (codephase_out != best_tau_sw) {
        std::cerr << "[ERREUR] codephase_out != phase_sw" << std::endl;
        ok = false;
    }
    if (max_power_out != (int)best_val_sw) {
        std::cerr << "[ERREUR] max_power_out != best_val_sw" << std::endl;
        ok = false;
    }

    if (!ok) {
        std::cerr << "\n[ECHEC] Testbench KO." << std::endl;
        return 1;
    }

    std::cout << "\n[OK] Testbench passe." << std::endl;
    return 0;
}