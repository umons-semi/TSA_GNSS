// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2023.2 (64-bit)
// Tool Version Limit: 2023.10
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
/***************************** Include Files *********************************/
#include "xacquisition_serial_m_axi.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XAcquisition_serial_m_axi_CfgInitialize(XAcquisition_serial_m_axi *InstancePtr, XAcquisition_serial_m_axi_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Ctrl_BaseAddress = ConfigPtr->Ctrl_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XAcquisition_serial_m_axi_Start(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_AP_CTRL) & 0x80;
    XAcquisition_serial_m_axi_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XAcquisition_serial_m_axi_IsDone(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XAcquisition_serial_m_axi_IsIdle(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XAcquisition_serial_m_axi_IsReady(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XAcquisition_serial_m_axi_EnableAutoRestart(XAcquisition_serial_m_axi *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAcquisition_serial_m_axi_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_AP_CTRL, 0x80);
}

void XAcquisition_serial_m_axi_DisableAutoRestart(XAcquisition_serial_m_axi *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAcquisition_serial_m_axi_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_AP_CTRL, 0);
}

void XAcquisition_serial_m_axi_Set_rx_real(XAcquisition_serial_m_axi *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAcquisition_serial_m_axi_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_RX_REAL_DATA, (u32)(Data));
    XAcquisition_serial_m_axi_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_RX_REAL_DATA + 4, (u32)(Data >> 32));
}

u64 XAcquisition_serial_m_axi_Get_rx_real(XAcquisition_serial_m_axi *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_RX_REAL_DATA);
    Data += (u64)XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_RX_REAL_DATA + 4) << 32;
    return Data;
}

void XAcquisition_serial_m_axi_Set_prn_in(XAcquisition_serial_m_axi *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAcquisition_serial_m_axi_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_PRN_IN_DATA, (u32)(Data));
    XAcquisition_serial_m_axi_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_PRN_IN_DATA + 4, (u32)(Data >> 32));
}

u64 XAcquisition_serial_m_axi_Get_prn_in(XAcquisition_serial_m_axi *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_PRN_IN_DATA);
    Data += (u64)XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_PRN_IN_DATA + 4) << 32;
    return Data;
}

void XAcquisition_serial_m_axi_Set_corr_out(XAcquisition_serial_m_axi *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAcquisition_serial_m_axi_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_CORR_OUT_DATA, (u32)(Data));
    XAcquisition_serial_m_axi_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_CORR_OUT_DATA + 4, (u32)(Data >> 32));
}

u64 XAcquisition_serial_m_axi_Get_corr_out(XAcquisition_serial_m_axi *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_CORR_OUT_DATA);
    Data += (u64)XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_CORR_OUT_DATA + 4) << 32;
    return Data;
}

u32 XAcquisition_serial_m_axi_Get_corr_count(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_CORR_COUNT_DATA);
    return Data;
}

u32 XAcquisition_serial_m_axi_Get_corr_count_vld(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_CORR_COUNT_CTRL);
    return Data & 0x1;
}

u32 XAcquisition_serial_m_axi_Get_doppler_out(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_DOPPLER_OUT_DATA);
    return Data;
}

u32 XAcquisition_serial_m_axi_Get_doppler_out_vld(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_DOPPLER_OUT_CTRL);
    return Data & 0x1;
}

u32 XAcquisition_serial_m_axi_Get_codephase_out(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_CODEPHASE_OUT_DATA);
    return Data;
}

u32 XAcquisition_serial_m_axi_Get_codephase_out_vld(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_CODEPHASE_OUT_CTRL);
    return Data & 0x1;
}

u32 XAcquisition_serial_m_axi_Get_sat_detected(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_SAT_DETECTED_DATA);
    return Data;
}

u32 XAcquisition_serial_m_axi_Get_sat_detected_vld(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_SAT_DETECTED_CTRL);
    return Data & 0x1;
}

void XAcquisition_serial_m_axi_Set_fd_step(XAcquisition_serial_m_axi *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAcquisition_serial_m_axi_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_FD_STEP_DATA, Data);
}

u32 XAcquisition_serial_m_axi_Get_fd_step(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_FD_STEP_DATA);
    return Data;
}

u32 XAcquisition_serial_m_axi_Get_max_power_out(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_MAX_POWER_OUT_DATA);
    return Data;
}

u32 XAcquisition_serial_m_axi_Get_max_power_out_vld(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_MAX_POWER_OUT_CTRL);
    return Data & 0x1;
}

u32 XAcquisition_serial_m_axi_Get_mean_power_out(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_MEAN_POWER_OUT_DATA);
    return Data;
}

u32 XAcquisition_serial_m_axi_Get_mean_power_out_vld(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_MEAN_POWER_OUT_CTRL);
    return Data & 0x1;
}

u32 XAcquisition_serial_m_axi_Get_rx_count(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_RX_COUNT_DATA);
    return Data;
}

u32 XAcquisition_serial_m_axi_Get_rx_count_vld(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_RX_COUNT_CTRL);
    return Data & 0x1;
}

u32 XAcquisition_serial_m_axi_Get_prn_count(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_PRN_COUNT_DATA);
    return Data;
}

u32 XAcquisition_serial_m_axi_Get_prn_count_vld(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_PRN_COUNT_CTRL);
    return Data & 0x1;
}

u32 XAcquisition_serial_m_axi_Get_rx_last_seen(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_RX_LAST_SEEN_DATA);
    return Data;
}

u32 XAcquisition_serial_m_axi_Get_rx_last_seen_vld(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_RX_LAST_SEEN_CTRL);
    return Data & 0x1;
}

u32 XAcquisition_serial_m_axi_Get_prn_last_seen(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_PRN_LAST_SEEN_DATA);
    return Data;
}

u32 XAcquisition_serial_m_axi_Get_prn_last_seen_vld(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_PRN_LAST_SEEN_CTRL);
    return Data & 0x1;
}

u32 XAcquisition_serial_m_axi_Get_rx_last_pos(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_RX_LAST_POS_DATA);
    return Data;
}

u32 XAcquisition_serial_m_axi_Get_rx_last_pos_vld(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_RX_LAST_POS_CTRL);
    return Data & 0x1;
}

u32 XAcquisition_serial_m_axi_Get_prn_last_pos(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_PRN_LAST_POS_DATA);
    return Data;
}

u32 XAcquisition_serial_m_axi_Get_prn_last_pos_vld(XAcquisition_serial_m_axi *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_PRN_LAST_POS_CTRL);
    return Data & 0x1;
}

void XAcquisition_serial_m_axi_InterruptGlobalEnable(XAcquisition_serial_m_axi *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAcquisition_serial_m_axi_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_GIE, 1);
}

void XAcquisition_serial_m_axi_InterruptGlobalDisable(XAcquisition_serial_m_axi *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAcquisition_serial_m_axi_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_GIE, 0);
}

void XAcquisition_serial_m_axi_InterruptEnable(XAcquisition_serial_m_axi *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_IER);
    XAcquisition_serial_m_axi_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_IER, Register | Mask);
}

void XAcquisition_serial_m_axi_InterruptDisable(XAcquisition_serial_m_axi *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_IER);
    XAcquisition_serial_m_axi_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_IER, Register & (~Mask));
}

void XAcquisition_serial_m_axi_InterruptClear(XAcquisition_serial_m_axi *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAcquisition_serial_m_axi_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_ISR, Mask);
}

u32 XAcquisition_serial_m_axi_InterruptGetEnabled(XAcquisition_serial_m_axi *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_IER);
}

u32 XAcquisition_serial_m_axi_InterruptGetStatus(XAcquisition_serial_m_axi *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XAcquisition_serial_m_axi_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_SERIAL_M_AXI_CTRL_ADDR_ISR);
}

