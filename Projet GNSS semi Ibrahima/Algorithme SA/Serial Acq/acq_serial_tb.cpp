#include "acq_serial.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cmath>
#include <cstdint>

using namespace std;

// Lecture simplifiée pour la robustesse
vector<int> read_csv(string file) {
    vector<int> data;
    ifstream f(file);
    if (!f.is_open()) {
        cerr << "Erreur: Impossible d'ouvrir " << file << endl;
        exit(1);
    }

    string line;
    while (getline(f, line)) {
        stringstream ss(line);
        string val;
        // Découpe par virgule OU lit simplement si une seule valeur par ligne
        while (getline(ss, val, ',')) {
            if (!val.empty()) {
                try {
                    data.push_back(stoi(val));
                } catch (...) {
                    // Ignore les entêtes ou caractères non numériques
                }
            }
        }
    }
    f.close();
    return data;
}

/* // Convertit un double [-1, 1] en entier Q23
static int to_q23(double x) {
    if (x >  0.99999988) x =  0.99999988;
    if (x < -1.0)        x = -1.0;
    return (int)llround(x * (double)(1 << 23));
}
 */
int main() {
    cout << "Demarrage CSIM (N=" << N << ", Phases=" << NB_PHASES << ")" << endl;

    vector<int> signal = read_csv("../signal.csv");
    vector<int> prn = read_csv("../PRN-25.csv");

    if(signal.size() < N || prn.size() < N) {
        cout << "Erreur : Taille CSV (" << signal.size() << ") < N" << endl;
        return 1;
    }

    hls::stream<axis_t> rx_stream, prn_stream, corr_stream;

    // Injection
    for(int i=0; i<N; i++) {
        axis_t a, b;
        a.data = signal[i]; a.last = (i==N-1); a.keep = -1; a.strb = -1;
        b.data = prn[i];    b.last = (i==N-1); b.keep = -1; b.strb = -1;
        rx_stream.write(a);
        prn_stream.write(b);
    }

    int doppler, codephase, sat;
    auto start = chrono::high_resolution_clock::now();

    acquisition_serial(rx_stream, prn_stream, corr_stream, doppler, codephase, sat, 250);

    float max_power = 0.0;
    int count = 0;

    // ========== AJOUT DEBUG : Prints pour diagnostiquer ==========

    cout << "\n=== DEBUG CORRELATION ===" << endl;
    cout << "\n=== DEBUG CORRELATION ===" << endl;
    cout << "fd_idx, tau, accI_total, accQ_total, power" << endl;

    while(!corr_stream.empty()) {
        axis_t out = corr_stream.read();
        
        // CORRECTION : Lire directement comme int, pas via union float
        int current_power = (int)out.data;  // ← CORRIGÉ

        if (current_power > (int)max_power) {
            max_power = (float)current_power;
        }
        count++;

        cout << "Point " << count << ": power = " << current_power << endl;
    }
    
    auto stop = chrono::high_resolution_clock::now();
    double t = chrono::duration<double, milli>(stop-start).count();

    cout << "\n======= RESULTATS CORRIGES =======" << endl;
    cout << "Doppler estime    : " << doppler << " Hz (Attendu: -1750)" << endl;
    cout << "Phase de code     : " << codephase << " (Attendu: ~150)" << endl;
    cout << "Satellite detecte : " << (sat ? "OUI" : "NON") << endl;
    cout << "Pic de correlation: " << max_power << endl;
    cout << "Nombre de points  : " << count << " (nb_fd * NB_PHASES)" << endl;
    cout << "Temps CSIM        : " << t << " ms" << endl;

    return 0;
}







