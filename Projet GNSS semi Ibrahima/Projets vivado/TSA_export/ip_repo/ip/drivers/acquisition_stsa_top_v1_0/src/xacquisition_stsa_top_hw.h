// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2023.2 (64-bit)
// Tool Version Limit: 2023.10
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
// CTRL
// 0x00 : Control signals
//        bit 0  - ap_start (Read/Write/COH)
//        bit 1  - ap_done (Read/COR)
//        bit 2  - ap_idle (Read)
//        bit 3  - ap_ready (Read/COR)
//        bit 7  - auto_restart (Read/Write)
//        bit 9  - interrupt (Read)
//        others - reserved
// 0x04 : Global Interrupt Enable Register
//        bit 0  - Global Interrupt Enable (Read/Write)
//        others - reserved
// 0x08 : IP Interrupt Enable Register (Read/Write)
//        bit 0 - enable ap_done interrupt (Read/Write)
//        bit 1 - enable ap_ready interrupt (Read/Write)
//        others - reserved
// 0x0c : IP Interrupt Status Register (Read/TOW)
//        bit 0 - ap_done (Read/TOW)
//        bit 1 - ap_ready (Read/TOW)
//        others - reserved
// 0x10 : Data signal of doppler_out
//        bit 31~0 - doppler_out[31:0] (Read)
// 0x14 : Control signal of doppler_out
//        bit 0  - doppler_out_ap_vld (Read/COR)
//        others - reserved
// 0x20 : Data signal of phase_out
//        bit 31~0 - phase_out[31:0] (Read)
// 0x24 : Control signal of phase_out
//        bit 0  - phase_out_ap_vld (Read/COR)
//        others - reserved
// 0x30 : Data signal of peak_out
//        bit 31~0 - peak_out[31:0] (Read)
// 0x34 : Data signal of peak_out
//        bit 15~0 - peak_out[47:32] (Read)
//        others   - reserved
// 0x38 : Control signal of peak_out
//        bit 0  - peak_out_ap_vld (Read/COR)
//        others - reserved
// 0x48 : Data signal of detected_out
//        bit 0  - detected_out[0] (Read)
//        others - reserved
// 0x4c : Control signal of detected_out
//        bit 0  - detected_out_ap_vld (Read/COR)
//        others - reserved
// 0x58 : Data signal of max_power_out
//        bit 31~0 - max_power_out[31:0] (Read)
// 0x5c : Control signal of max_power_out
//        bit 0  - max_power_out_ap_vld (Read/COR)
//        others - reserved
// 0x68 : Data signal of mean_power_out
//        bit 31~0 - mean_power_out[31:0] (Read)
// 0x6c : Control signal of mean_power_out
//        bit 0  - mean_power_out_ap_vld (Read/COR)
//        others - reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XACQUISITION_STSA_TOP_CTRL_ADDR_AP_CTRL             0x00
#define XACQUISITION_STSA_TOP_CTRL_ADDR_GIE                 0x04
#define XACQUISITION_STSA_TOP_CTRL_ADDR_IER                 0x08
#define XACQUISITION_STSA_TOP_CTRL_ADDR_ISR                 0x0c
#define XACQUISITION_STSA_TOP_CTRL_ADDR_DOPPLER_OUT_DATA    0x10
#define XACQUISITION_STSA_TOP_CTRL_BITS_DOPPLER_OUT_DATA    32
#define XACQUISITION_STSA_TOP_CTRL_ADDR_DOPPLER_OUT_CTRL    0x14
#define XACQUISITION_STSA_TOP_CTRL_ADDR_PHASE_OUT_DATA      0x20
#define XACQUISITION_STSA_TOP_CTRL_BITS_PHASE_OUT_DATA      32
#define XACQUISITION_STSA_TOP_CTRL_ADDR_PHASE_OUT_CTRL      0x24
#define XACQUISITION_STSA_TOP_CTRL_ADDR_PEAK_OUT_DATA       0x30
#define XACQUISITION_STSA_TOP_CTRL_BITS_PEAK_OUT_DATA       48
#define XACQUISITION_STSA_TOP_CTRL_ADDR_PEAK_OUT_CTRL       0x38
#define XACQUISITION_STSA_TOP_CTRL_ADDR_DETECTED_OUT_DATA   0x48
#define XACQUISITION_STSA_TOP_CTRL_BITS_DETECTED_OUT_DATA   1
#define XACQUISITION_STSA_TOP_CTRL_ADDR_DETECTED_OUT_CTRL   0x4c
#define XACQUISITION_STSA_TOP_CTRL_ADDR_MAX_POWER_OUT_DATA  0x58
#define XACQUISITION_STSA_TOP_CTRL_BITS_MAX_POWER_OUT_DATA  32
#define XACQUISITION_STSA_TOP_CTRL_ADDR_MAX_POWER_OUT_CTRL  0x5c
#define XACQUISITION_STSA_TOP_CTRL_ADDR_MEAN_POWER_OUT_DATA 0x68
#define XACQUISITION_STSA_TOP_CTRL_BITS_MEAN_POWER_OUT_DATA 32
#define XACQUISITION_STSA_TOP_CTRL_ADDR_MEAN_POWER_OUT_CTRL 0x6c

