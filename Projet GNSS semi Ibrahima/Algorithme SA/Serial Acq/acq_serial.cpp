// Acquisition GPS série - Vitis HLS 2023.2 - Cible : xc7z020-clg400-1

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// VERSION NCO RECURSIF optimisation max atteinte sur Zynq (timing et ressources)
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "acq_serial.h"
#include "dds_lut_rom.h"   // DDS_SIN_LUT[1024], DDS_COS_LUT[1024]

#ifndef __SYNTHESIS__
#include <iostream>
#endif

 
// static inline void sincos_osc(angle_t a, osc_t &s, osc_t &c) {
//     #pragma HLS INLINE
//         trig_t st = 0, ct = 0;
//         hls::sincos(a, &st, &ct);
//         s = (osc_t)st;
//         c = (osc_t)ct;
// }

typedef ap_uint<32> phase_u_t;
typedef ap_int<32>  phase_t;

static inline phase_t hz_to_phase_inc(int freq_hz) {
    #pragma HLS INLINE
    const long long SCALE = (1LL << DDS_PHASE_BITS);
    long long num = (long long)freq_hz * SCALE;
    if (num >= 0) num += (FS_INT / 2);
    else          num -= (FS_INT / 2);
    return (phase_t)(num / (long long)FS_INT);
}

void acquisition_serial(
    hls::stream<axis_t> &rx_real,    // Signal GPS reçu (AXI-Stream)
    hls::stream<axis_t> &prn_in,     // Code PRN local  (AXI-Stream)
    hls::stream<axis_t> &corr_out,   // Puissances de corrélation (AXI-Stream → PS)
    int &doppler_out,                // Doppler estimé (Hz)
    int &codephase_out,              // Phase de code optimale (indice tau)
    int &sat_detected,               // 1 = détecté, 0 = non
    int fd_step                      // Pas de balayage Doppler (Hz)
) {
    #pragma HLS INTERFACE axis port=rx_real
    #pragma HLS INTERFACE axis port=prn_in
    #pragma HLS INTERFACE axis port=corr_out
    #pragma HLS INTERFACE s_axilite port=doppler_out   bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=codephase_out bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=sat_detected  bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=fd_step       bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=return        bundle=CTRL

    // --- Buffers BRAM ---
    static data_t     signal_buf[N];
    static ap_uint<1> prn_sign[2 * N]; // 1 => +1, 0 => -1
    static osc_t      mixI[N];
    static osc_t      mixQ[N];

    #pragma HLS ARRAY_PARTITION variable=prn_sign cyclic factor=2 dim=1
    #pragma HLS ARRAY_PARTITION variable=signal_buf cyclic factor=2 dim=1
    #pragma HLS ARRAY_PARTITION variable=mixI     cyclic factor=2 dim=1
    #pragma HLS ARRAY_PARTITION variable=mixQ     cyclic factor=2 dim=1

    if (fd_step <= 0) fd_step = 1;
    int nb_fd = (FD_END - FD_START) / fd_step + 1;
    // const int RENORM_PERIOD = 64;

    // ================================================================
    // ÉTAPE 1 : Chargement signal + PRN
    // ================================================================
    LOAD: for (int i = 0; i < N; i++) {
        #pragma HLS PIPELINE II=2

        axis_t tmp_r = rx_real.read();
        axis_t tmp_p = prn_in.read();

        signal_buf[i] = (data_t)tmp_r.data;
        prn_sign[i]   = (tmp_p.data > 0) ? (ap_uint<1>)1 : (ap_uint<1>)0;
    }

    // Duplication PRN
    PRN_DUP: for (int i = 0; i < N; i++) {
        #pragma HLS PIPELINE II=1
        prn_sign[i + N] = prn_sign[i];
    }

    // // Pré-calcul NCO Doppler : 2 sincos seulement, types homogènes
    // const angle_t angle0 = (angle_t)(-((FREQUENCE_CENTRALE + FD_START) * TWO_PI_DIV_FS));
    // const angle_t dangle = (angle_t)(-(fd_step * TWO_PI_DIV_FS));

    // osc_t sin_d_cur = 0, cos_d_cur = 0;
    // osc_t sin_step  = 0, cos_step  = 0;
    // sincos_osc(angle0, sin_d_cur, cos_d_cur);
    // sincos_osc(dangle, sin_step,  cos_step);

    // Version LUT pour éviter les calculs de sincos en float
    const phase_t phase_inc0     = hz_to_phase_inc(-(FREQUENCE_CENTRALE_HZ + FD_START));
    const phase_t phase_inc_step = hz_to_phase_inc(-fd_step);

    power_t max_val     = 0;
    power_t sum_corr    = 0;
    int     best_tau    = 0;
    int     best_fd_idx = 0;

    // ================================================================
    // ÉTAPE 2 : Balayage Doppler
    // ================================================================
    FD_LOOP: for(int fd_idx = 0; fd_idx < nb_fd; fd_idx++) {
        #pragma HLS LOOP_TRIPCOUNT min=1 max=21 avg=11
        // #pragma HLS PIPELINE

     /*
        // NCO Doppler courant
        osc_t cos_d = cos_d_cur;
        osc_t sin_d = sin_d_cur;

        osc_t prod_sc = sin_d * cos_d;
        osc_t cos2    = cos_d * cos_d - sin_d * sin_d;
        osc_t sin2    = prod_sc + prod_sc;

         // État initial du NCO
        osc_t cos_n_reg = 1.0;
        osc_t sin_n_reg = 0.0;

        int base = 0;

        ROT_FULL: for (; base + RENORM_PERIOD <= N; base += RENORM_PERIOD) {
            ROT64: for (int k = 0; k + 1 < RENORM_PERIOD; k += 2) {
                #pragma HLS PIPELINE
                #pragma HLS DEPENDENCE variable=mixI inter false
                #pragma HLS DEPENDENCE variable=mixI intra false
                #pragma HLS DEPENDENCE variable=mixQ inter false
                #pragma HLS DEPENDENCE variable=mixQ intra false

                const int n0 = base + k;
                const int n1 = n0 + 1;

                osc_t c0 = cos_n_reg;
                osc_t s0 = sin_n_reg;

                osc_t c1 = c0 * cos_d - s0 * sin_d;
                osc_t s1 = s0 * cos_d + c0 * sin_d;

                data_t x0 = signal_buf[n0];
                data_t x1 = signal_buf[n1];

                mixI[n0] = x0 * c0;
                mixQ[n0] = -x0 * s0;
                mixI[n1] = x1 * c1;
                mixQ[n1] = -x1 * s1;

                cos_n_reg = c0 * cos2 - s0 * sin2;
                sin_n_reg = s0 * cos2 + c0 * sin2;
            }

            osc_t c2 = cos_n_reg * cos_n_reg;
            osc_t s2 = sin_n_reg * sin_n_reg;
            #pragma HLS BIND_OP variable=c2 op=mul impl=dsp
            #pragma HLS BIND_OP variable=s2 op=mul impl=dsp

            osc_t mag2 = c2 + s2;
            osc_t norm = (osc_t)1.5 - (mag2 >> 1);

            cos_n_reg = cos_n_reg * norm;
            sin_n_reg = sin_n_reg * norm;
        }

        ROT_TAIL: for (; base + 1 < N; base += 2) {
            #pragma HLS PIPELINE   
            #pragma HLS DEPENDENCE variable=mixI inter false
            #pragma HLS DEPENDENCE variable=mixI intra false
            #pragma HLS DEPENDENCE variable=mixQ inter false
            #pragma HLS DEPENDENCE variable=mixQ intra false

            const int n0 = base;
            const int n1 = base + 1;

            osc_t c0 = cos_n_reg;
            osc_t s0 = sin_n_reg;

            osc_t c1 = c0 * cos_d - s0 * sin_d;
            osc_t s1 = s0 * cos_d + c0 * sin_d;

            data_t x0 = signal_buf[n0];
            data_t x1 = signal_buf[n1];

            mixI[n0] = x0 * c0;
            mixQ[n0] = -x0 * s0;
            mixI[n1] = x1 * c1;
            mixQ[n1] = -x1 * s1;

            cos_n_reg = c0 * cos2 - s0 * sin2;
            sin_n_reg = s0 * cos2 + c0 * sin2;
        }

        if (base < N) {
            data_t x = signal_buf[base];
            mixI[base] = x * cos_n_reg;
            mixQ[base] = -x * sin_n_reg;

            osc_t c_next = cos_n_reg * cos_d - sin_n_reg * sin_d;
            osc_t s_next = sin_n_reg * cos_d + cos_n_reg * sin_d;
            cos_n_reg = c_next;
            sin_n_reg = s_next;
        }
 */
        phase_t   phase_inc = phase_inc0 + (phase_t)((long long)fd_idx * (long long)phase_inc_step);
        phase_u_t phase_acc = 0;

        ROT: for (int n = 0; n < N; n++) {
            #pragma HLS PIPELINE II=1
            #pragma HLS DEPENDENCE variable=mixI inter false
            #pragma HLS DEPENDENCE variable=mixQ inter false
            
            ap_uint<DDS_LUT_BITS> lut_idx =
                phase_acc.range(DDS_PHASE_BITS - 1, DDS_PHASE_BITS - DDS_LUT_BITS);

            osc_t c = DDS_COS_LUT[(int)lut_idx];
            osc_t s = DDS_SIN_LUT[(int)lut_idx];
            osc_t x = (osc_t)signal_buf[n];
            
            mixI[n] = x * c;
            mixQ[n] = -(x * s);

            phase_acc = (phase_u_t)(phase_acc + (phase_u_t)phase_inc);
        }
 
        TAU_LOOP: for(int tau = 0; tau < NB_PHASES; tau++) {
            #pragma HLS LOOP_TRIPCOUNT min=1023 max=1023 avg=1023
            //#pragma HLS PIPELINE II=2
            // #pragma HLS loop_flatten
            #pragma HLS DEPENDENCE variable=prn_sign inter false
            // #pragma HLS pipeline II=1
            acc_t accI[2] = {0, 0};
            acc_t accQ[2] = {0, 0};
            #pragma HLS ARRAY_PARTITION variable=accI complete
            #pragma HLS ARRAY_PARTITION variable=accQ complete

            int idx0 = (int)(((long long)tau * (long long)N) / NB_PHASES);

           

            CORR: for (int n = 0; n + 1 < N; n += 2) {
                //#pragma HLS PIPELINE II=1
                #pragma HLS UNROLL factor=2
                #pragma HLS DEPENDENCE variable=accI inter false
                #pragma HLS DEPENDENCE variable=accQ inter false
                #pragma HLS DEPENDENCE variable=accI intra false
                #pragma HLS DEPENDENCE variable=accQ intra false
                ap_uint<1> s0 = prn_sign[idx0];
                ap_uint<1> s1 = prn_sign[idx0 + 1];
                
                osc_t i0 = mixI[n];
                osc_t i1 = mixI[n + 1];
                osc_t q0 = mixQ[n];
                osc_t q1 = mixQ[n + 1];

                if (!s0) { i0 = -i0; q0 = -q0; }
                if (!s1) { i1 = -i1; q1 = -q1; }

                accI[0] += i0;
                accI[1] += i1;
                accQ[0] += q0;
                accQ[1] += q1;

                idx0 += 2;
            }

            if (N & 1) {
                ap_uint<1> s_last = prn_sign[idx0];

                osc_t i_last = mixI[N - 1];
                osc_t q_last = mixQ[N - 1];
                if (!s_last) { i_last = -i_last; q_last = -q_last; }

                accI[0] += i_last;
                accQ[0] += q_last;
            }

            acc_t accI_total = accI[0] + accI[1];
            acc_t accQ_total = accQ[0] + accQ[1];
            power_t power = (power_t)(accI_total * accI_total + accQ_total * accQ_total);

            sum_corr += power;

            if(power > max_val) {
                max_val     = power;
                best_tau    = tau;
                best_fd_idx = fd_idx;
            }  

            axis_t out_axis;
            out_axis.data = (ap_int<32>)power;
            out_axis.keep = -1;
            out_axis.strb = -1;
            out_axis.last = (fd_idx == nb_fd - 1 && tau == NB_PHASES - 1);
            corr_out.write(out_axis);
        }

        /* if (fd_idx != nb_fd - 1) {
            osc_t cc = cos_d_cur * cos_step;
            osc_t ss = sin_d_cur * sin_step;
            osc_t cs = cos_d_cur * sin_step;
            osc_t sc = sin_d_cur * cos_step;
            #pragma HLS BIND_OP variable=cc op=mul impl=dsp
            #pragma HLS BIND_OP variable=ss op=mul impl=dsp
            #pragma HLS BIND_OP variable=cs op=mul impl=dsp
            #pragma HLS BIND_OP variable=sc op=mul impl=dsp

            osc_t c_next = cc - ss;
            osc_t s_next = sc + cs;

            if ((fd_idx & 31) == 31) {
                osc_t c2 = c_next * c_next;
                osc_t s2 = s_next * s_next;
                osc_t mag2 = c2 + s2;
                osc_t norm = (osc_t)1.5 - (mag2 >> 1);
                c_next = c_next * norm;
                s_next = s_next * norm;
            }

            cos_d_cur = c_next;
            sin_d_cur = s_next;
        } */
    } // fermeture de FD_LOOP

    // ÉTAPE 3 : Décision finale
    power_t mean = sum_corr / (nb_fd * NB_PHASES);
    sat_detected = (max_val > mean * (power_t)SEUIL_RATIO) ? 1 : 0;
    doppler_out  = FD_START + best_fd_idx * fd_step;
    codephase_out = best_tau;
}


