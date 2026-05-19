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
// 0x20 : Data signal of codephase_out
//        bit 31~0 - codephase_out[31:0] (Read)
// 0x24 : Control signal of codephase_out
//        bit 0  - codephase_out_ap_vld (Read/COR)
//        others - reserved
// 0x30 : Data signal of sat_detected
//        bit 31~0 - sat_detected[31:0] (Read)
// 0x34 : Control signal of sat_detected
//        bit 0  - sat_detected_ap_vld (Read/COR)
//        others - reserved
// 0x40 : Data signal of fd_step
//        bit 31~0 - fd_step[31:0] (Read/Write)
// 0x44 : reserved
// 0x48 : Data signal of max_power_out
//        bit 31~0 - max_power_out[31:0] (Read)
// 0x4c : Control signal of max_power_out
//        bit 0  - max_power_out_ap_vld (Read/COR)
//        others - reserved
// 0x58 : Data signal of mean_power_out
//        bit 31~0 - mean_power_out[31:0] (Read)
// 0x5c : Control signal of mean_power_out
//        bit 0  - mean_power_out_ap_vld (Read/COR)
//        others - reserved
// 0x68 : Data signal of rx_count
//        bit 31~0 - rx_count[31:0] (Read)
// 0x6c : Control signal of rx_count
//        bit 0  - rx_count_ap_vld (Read/COR)
//        others - reserved
// 0x78 : Data signal of prn_count
//        bit 31~0 - prn_count[31:0] (Read)
// 0x7c : Control signal of prn_count
//        bit 0  - prn_count_ap_vld (Read/COR)
//        others - reserved
// 0x88 : Data signal of rx_last_seen
//        bit 31~0 - rx_last_seen[31:0] (Read)
// 0x8c : Control signal of rx_last_seen
//        bit 0  - rx_last_seen_ap_vld (Read/COR)
//        others - reserved
// 0x98 : Data signal of prn_last_seen
//        bit 31~0 - prn_last_seen[31:0] (Read)
// 0x9c : Control signal of prn_last_seen
//        bit 0  - prn_last_seen_ap_vld (Read/COR)
//        others - reserved
// 0xa8 : Data signal of rx_last_pos
//        bit 31~0 - rx_last_pos[31:0] (Read)
// 0xac : Control signal of rx_last_pos
//        bit 0  - rx_last_pos_ap_vld (Read/COR)
//        others - reserved
// 0xb8 : Data signal of prn_last_pos
//        bit 31~0 - prn_last_pos[31:0] (Read)
// 0xbc : Control signal of prn_last_pos
//        bit 0  - prn_last_pos_ap_vld (Read/COR)
//        others - reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XACQUISITION_SERIAL_CTRL_ADDR_AP_CTRL             0x00
#define XACQUISITION_SERIAL_CTRL_ADDR_GIE                 0x04
#define XACQUISITION_SERIAL_CTRL_ADDR_IER                 0x08
#define XACQUISITION_SERIAL_CTRL_ADDR_ISR                 0x0c
#define XACQUISITION_SERIAL_CTRL_ADDR_DOPPLER_OUT_DATA    0x10
#define XACQUISITION_SERIAL_CTRL_BITS_DOPPLER_OUT_DATA    32
#define XACQUISITION_SERIAL_CTRL_ADDR_DOPPLER_OUT_CTRL    0x14
#define XACQUISITION_SERIAL_CTRL_ADDR_CODEPHASE_OUT_DATA  0x20
#define XACQUISITION_SERIAL_CTRL_BITS_CODEPHASE_OUT_DATA  32
#define XACQUISITION_SERIAL_CTRL_ADDR_CODEPHASE_OUT_CTRL  0x24
#define XACQUISITION_SERIAL_CTRL_ADDR_SAT_DETECTED_DATA   0x30
#define XACQUISITION_SERIAL_CTRL_BITS_SAT_DETECTED_DATA   32
#define XACQUISITION_SERIAL_CTRL_ADDR_SAT_DETECTED_CTRL   0x34
#define XACQUISITION_SERIAL_CTRL_ADDR_FD_STEP_DATA        0x40
#define XACQUISITION_SERIAL_CTRL_BITS_FD_STEP_DATA        32
#define XACQUISITION_SERIAL_CTRL_ADDR_MAX_POWER_OUT_DATA  0x48
#define XACQUISITION_SERIAL_CTRL_BITS_MAX_POWER_OUT_DATA  32
#define XACQUISITION_SERIAL_CTRL_ADDR_MAX_POWER_OUT_CTRL  0x4c
#define XACQUISITION_SERIAL_CTRL_ADDR_MEAN_POWER_OUT_DATA 0x58
#define XACQUISITION_SERIAL_CTRL_BITS_MEAN_POWER_OUT_DATA 32
#define XACQUISITION_SERIAL_CTRL_ADDR_MEAN_POWER_OUT_CTRL 0x5c
#define XACQUISITION_SERIAL_CTRL_ADDR_RX_COUNT_DATA       0x68
#define XACQUISITION_SERIAL_CTRL_BITS_RX_COUNT_DATA       32
#define XACQUISITION_SERIAL_CTRL_ADDR_RX_COUNT_CTRL       0x6c
#define XACQUISITION_SERIAL_CTRL_ADDR_PRN_COUNT_DATA      0x78
#define XACQUISITION_SERIAL_CTRL_BITS_PRN_COUNT_DATA      32
#define XACQUISITION_SERIAL_CTRL_ADDR_PRN_COUNT_CTRL      0x7c
#define XACQUISITION_SERIAL_CTRL_ADDR_RX_LAST_SEEN_DATA   0x88
#define XACQUISITION_SERIAL_CTRL_BITS_RX_LAST_SEEN_DATA   32
#define XACQUISITION_SERIAL_CTRL_ADDR_RX_LAST_SEEN_CTRL   0x8c
#define XACQUISITION_SERIAL_CTRL_ADDR_PRN_LAST_SEEN_DATA  0x98
#define XACQUISITION_SERIAL_CTRL_BITS_PRN_LAST_SEEN_DATA  32
#define XACQUISITION_SERIAL_CTRL_ADDR_PRN_LAST_SEEN_CTRL  0x9c
#define XACQUISITION_SERIAL_CTRL_ADDR_RX_LAST_POS_DATA    0xa8
#define XACQUISITION_SERIAL_CTRL_BITS_RX_LAST_POS_DATA    32
#define XACQUISITION_SERIAL_CTRL_ADDR_RX_LAST_POS_CTRL    0xac
#define XACQUISITION_SERIAL_CTRL_ADDR_PRN_LAST_POS_DATA   0xb8
#define XACQUISITION_SERIAL_CTRL_BITS_PRN_LAST_POS_DATA   32
#define XACQUISITION_SERIAL_CTRL_ADDR_PRN_LAST_POS_CTRL   0xbc

