#ifndef ACQ_SERIAL_H
#define ACQ_SERIAL_H

#include <hls_stream.h>
#include <ap_int.h>
#include <ap_fixed.h>
#include <hls_math.h>
#include <ap_axi_sdata.h> // Indispensable pour axis_t

// --- Paramètres FIXES ---
#define N 11999 
#define FS 11999000.0
#define FREQUENCE_CENTRALE 3563000.0
#define SEUIL_RATIO 2.5

// --- Paramètres ADAPTATIFS (CSIM vs SYNTHESE) ---
#ifdef __SYNTHESIS__
    #define FD_START -10000
    #define FD_END   10000
    #define NB_PHASES 1023
#else
    #define FD_START -2500
    #define FD_END   -1000
    #define NB_PHASES 1023  // Le TB pourra enfin lire cette valeur
#endif

// --- Paramètres DDS LUT ---
#define DDS_LUT_BITS           10
#define DDS_LUT_SIZE           (1 << DDS_LUT_BITS)   // 1024
#define DDS_PHASE_BITS         32
#define FREQUENCE_CENTRALE_HZ  3563000
#define FS_INT                 11999000

// Pour éviter toute erreur dans le .cpp
#define NB_PHASES_TO_CHECK NB_PHASES
#define TWO_PI_DIV_FS  0.000000523641347317 

// Constante pré-calculée pour éviter le float : 2*PI / FS
// 2 * 3.1415926535 / 11999000.0 = 0.000000523641347...
// Valeur plus précise : 0.000000523641347317316...
//const ap_fixed<40, 2> TWO_PI_DIV_FS = 0.000000523641347317;


typedef ap_axis<32, 0, 0, 0> axis_t; 

// Types de données
typedef ap_int<2> data_t;

// osc_t : 16 bits (18 max) pour tenir dans un seul port de DSP
//ap_fixed<16, 2, AP_RND_CONV, AP_SAT> osc_t
// AP_RND_CONV aide à maintenir la précision dans la boucle récursive
typedef ap_fixed<24, 8> osc_t;  //ap_fixed<24, 4>: trop de bit(large) 

// Type pour les calculs d'angle/NCO
typedef ap_fixed<32, 6> angle_t;
typedef ap_fixed<28,2>  trig_t;   // type imposé par hls::sincos(angle_t,...)
// acc_t : On peut rester sur 32 bits. 40 bits ralentit le timing sur Zynq.
// Avec N=11999, la somme max ne dépassera pas 32 bits si data_t est petit.
typedef ap_fixed<32, 24> acc_t;
typedef ap_fixed<64, 32> power_t;

void acquisition_serial(
    hls::stream<axis_t> &rx_real,
    hls::stream<axis_t> &prn_in,
    hls::stream<axis_t> &corr_out,
    int &doppler_out,
    int &codephase_out,
    int &sat_detected,
    int fd_step
);

#endif






// #ifndef ACQ_SERIAL_H
// #define ACQ_SERIAL_H

// #include <hls_stream.h>
// #include <ap_int.h>
// #include <ap_fixed.h>
// #include <hls_math.h>
// #include <ap_axi_sdata.h>

// // --- Paramètres FIXES ---
// #define N 11999
// #define FS_INT 11999000
// #define FS 11999000.0
// #define FREQUENCE_CENTRALE_HZ 3563000
// #define FREQUENCE_CENTRALE 3563000.0
// #define SEUIL_RATIO 2.5

// // --- Paramètres ADAPTATIFS ---
// #ifdef __SYNTHESIS__
//     #define FD_START -10000
//     #define FD_END    10000
//     #define NB_PHASES 1023
// #else
//     #define FD_START -2500
//     #define FD_END   -1000
//     #define NB_PHASES 1023
// #endif

// #define NB_PHASES_TO_CHECK NB_PHASES
// #define TWO_PI_DIV_FS 0.000000523641347317

// // --- DDS ---
// #define DDS_PHASE_BITS 24
// #define DDS_LUT_BITS   10
// #define DDS_LUT_SIZE   (1 << DDS_LUT_BITS)

// typedef ap_axis<32, 0, 0, 0> axis_t;

// // Types
// typedef ap_int<16> data_t;       // signal RX
// typedef ap_fixed<24, 8> osc_t;   // mix/NCO
// typedef ap_fixed<32, 6> angle_t; // pour sincos d'init LUT
// typedef ap_fixed<28, 2> trig_t;  // sortie attendue de hls::sincos(angle_t,...)

// typedef ap_fixed<32, 24> acc_t;
// typedef ap_fixed<64, 32> power_t;

// void acquisition_serial(
//     hls::stream<axis_t> &rx_real,
//     hls::stream<axis_t> &prn_in,
//     hls::stream<axis_t> &corr_out,
//     int &doppler_out,
//     int &codephase_out,
//     int &sat_detected,
//     int fd_step
// );

// #endif