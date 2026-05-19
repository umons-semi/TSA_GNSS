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
// 0x10 : Data signal of rx_real
//        bit 31~0 - rx_real[31:0] (Read/Write)
// 0x14 : Data signal of rx_real
//        bit 31~0 - rx_real[63:32] (Read/Write)
// 0x18 : reserved
// 0x1c : Data signal of prn_in
//        bit 31~0 - prn_in[31:0] (Read/Write)
// 0x20 : Data signal of prn_in
//        bit 31~0 - prn_in[63:32] (Read/Write)
// 0x24 : reserved
// 0x28 : Data signal of corr_out
//        bit 31~0 - corr_out[31:0] (Read/Write)
// 0x2c : Data signal of corr_out
//        bit 31~0 - corr_out[63:32] (Read/Write)
// 0x30 : reserved
// 0x34 : Data signal of corr_count
//        bit 31~0 - corr_count[31:0] (Read)
// 0x38 : Control signal of corr_count
//        bit 0  - corr_count_ap_vld (Read/COR)
//        others - reserved
// 0x44 : Data signal of doppler_out
//        bit 31~0 - doppler_out[31:0] (Read)
// 0x48 : Control signal of doppler_out
//        bit 0  - doppler_out_ap_vld (Read/COR)
//        others - reserved
// 0x54 : Data signal of codephase_out
//        bit 31~0 - codephase_out[31:0] (Read)
// 0x58 : Control signal of codephase_out
//        bit 0  - codephase_out_ap_vld (Read/COR)
//        others - reserved
// 0x64 : Data signal of sat_detected
//        bit 31~0 - sat_detected[31:0] (Read)
// 0x68 : Control signal of sat_detected
//        bit 0  - sat_detected_ap_vld (Read/COR)
//        others - reserved
// 0x74 : Data signal of fd_step
//        bit 31~0 - fd_step[31:0] (Read/Write)
// 0x78 : reserved
// 0x7c : Data signal of max_power_out
//        bit 31~0 - max_power_out[31:0] (Read)
// 0x80 : Control signal of max_power_out
//        bit 0  - max_power_out_ap_vld (Read/COR)
//        others - reserved
// 0x8c : Data signal of mean_power_out
//        bit 31~0 - mean_power_out[31:0] (Read)
// 0x90 : Control signal of mean_power_out
//        bit 0  - mean_power_out_ap_vld (Read/COR)
//        others - reserved
// 0x9c : Data signal of rx_count
//        bit 31~0 - rx_count[31:0] (Read)
// 0xa0 : Control signal of rx_count
//        bit 0  - rx_count_ap_vld (Read/COR)
//        others - reserved
// 0xac : Data signal of prn_count
//        bit 31~0 - prn_count[31:0] (Read)
// 0xb0 : Control signal of prn_count
//        bit 0  - prn_count_ap_vld (Read/COR)
//        others - reserved
// 0xbc : Data signal of rx_last_seen
//        bit 31~0 - rx_last_seen[31:0] (Read)
// 0xc0 : Control signal of rx_last_seen
//        bit 0  - rx_last_seen_ap_vld (Read/COR)
//        others - reserved
// 0xcc : Data signal of prn_last_seen
//        bit 31~0 - prn_last_seen[31:0] (Read)
// 0xd0 : Control signal of prn_last_seen
//        bit 0  - prn_last_seen_ap_vld (Read/COR)
//        others - reserved
// 0xdc : Data signal of rx_last_pos
//        bit 31~0 - rx_last_pos[31:0] (Read)
// 0xe0 : Control signal of rx_last_pos
//        bit 0  - rx_last_pos_ap_vld (Read/COR)
//        others - reserved
// 0xec : Data signal of prn_last_pos
//        bit 31~0 - prn_last_pos[31:0] (Read)
// 0xf0 : Control signal of prn_last_pos
//        bit 0  - prn_last_pos_ap_vld (Read/COR)
//        others - reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_AP_CTRL             0x00
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_GIE                 0x04
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_IER                 0x08
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_ISR                 0x0c
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_RX_REAL_DATA        0x10
#define XACQUISITION_SERIAL_M_AXI_CTRL_BITS_RX_REAL_DATA        64
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_PRN_IN_DATA         0x1c
#define XACQUISITION_SERIAL_M_AXI_CTRL_BITS_PRN_IN_DATA         64
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_CORR_OUT_DATA       0x28
#define XACQUISITION_SERIAL_M_AXI_CTRL_BITS_CORR_OUT_DATA       64
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_CORR_COUNT_DATA     0x34
#define XACQUISITION_SERIAL_M_AXI_CTRL_BITS_CORR_COUNT_DATA     32
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_CORR_COUNT_CTRL     0x38
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_DOPPLER_OUT_DATA    0x44
#define XACQUISITION_SERIAL_M_AXI_CTRL_BITS_DOPPLER_OUT_DATA    32
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_DOPPLER_OUT_CTRL    0x48
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_CODEPHASE_OUT_DATA  0x54
#define XACQUISITION_SERIAL_M_AXI_CTRL_BITS_CODEPHASE_OUT_DATA  32
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_CODEPHASE_OUT_CTRL  0x58
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_SAT_DETECTED_DATA   0x64
#define XACQUISITION_SERIAL_M_AXI_CTRL_BITS_SAT_DETECTED_DATA   32
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_SAT_DETECTED_CTRL   0x68
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_FD_STEP_DATA        0x74
#define XACQUISITION_SERIAL_M_AXI_CTRL_BITS_FD_STEP_DATA        32
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_MAX_POWER_OUT_DATA  0x7c
#define XACQUISITION_SERIAL_M_AXI_CTRL_BITS_MAX_POWER_OUT_DATA  32
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_MAX_POWER_OUT_CTRL  0x80
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_MEAN_POWER_OUT_DATA 0x8c
#define XACQUISITION_SERIAL_M_AXI_CTRL_BITS_MEAN_POWER_OUT_DATA 32
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_MEAN_POWER_OUT_CTRL 0x90
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_RX_COUNT_DATA       0x9c
#define XACQUISITION_SERIAL_M_AXI_CTRL_BITS_RX_COUNT_DATA       32
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_RX_COUNT_CTRL       0xa0
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_PRN_COUNT_DATA      0xac
#define XACQUISITION_SERIAL_M_AXI_CTRL_BITS_PRN_COUNT_DATA      32
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_PRN_COUNT_CTRL      0xb0
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_RX_LAST_SEEN_DATA   0xbc
#define XACQUISITION_SERIAL_M_AXI_CTRL_BITS_RX_LAST_SEEN_DATA   32
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_RX_LAST_SEEN_CTRL   0xc0
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_PRN_LAST_SEEN_DATA  0xcc
#define XACQUISITION_SERIAL_M_AXI_CTRL_BITS_PRN_LAST_SEEN_DATA  32
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_PRN_LAST_SEEN_CTRL  0xd0
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_RX_LAST_POS_DATA    0xdc
#define XACQUISITION_SERIAL_M_AXI_CTRL_BITS_RX_LAST_POS_DATA    32
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_RX_LAST_POS_CTRL    0xe0
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_PRN_LAST_POS_DATA   0xec
#define XACQUISITION_SERIAL_M_AXI_CTRL_BITS_PRN_LAST_POS_DATA   32
#define XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_PRN_LAST_POS_CTRL   0xf0

