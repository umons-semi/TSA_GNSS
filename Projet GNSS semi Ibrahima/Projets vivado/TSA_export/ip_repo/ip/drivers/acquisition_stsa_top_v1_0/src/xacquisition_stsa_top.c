// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2023.2 (64-bit)
// Tool Version Limit: 2023.10
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
/***************************** Include Files *********************************/
#include "xacquisition_stsa_top.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XAcquisition_stsa_top_CfgInitialize(XAcquisition_stsa_top *InstancePtr, XAcquisition_stsa_top_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Ctrl_BaseAddress = ConfigPtr->Ctrl_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XAcquisition_stsa_top_Start(XAcquisition_stsa_top *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_AP_CTRL) & 0x80;
    XAcquisition_stsa_top_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XAcquisition_stsa_top_IsDone(XAcquisition_stsa_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XAcquisition_stsa_top_IsIdle(XAcquisition_stsa_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XAcquisition_stsa_top_IsReady(XAcquisition_stsa_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XAcquisition_stsa_top_EnableAutoRestart(XAcquisition_stsa_top *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAcquisition_stsa_top_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_AP_CTRL, 0x80);
}

void XAcquisition_stsa_top_DisableAutoRestart(XAcquisition_stsa_top *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAcquisition_stsa_top_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_AP_CTRL, 0);
}

u32 XAcquisition_stsa_top_Get_doppler_out(XAcquisition_stsa_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_DOPPLER_OUT_DATA);
    return Data;
}

u32 XAcquisition_stsa_top_Get_doppler_out_vld(XAcquisition_stsa_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_DOPPLER_OUT_CTRL);
    return Data & 0x1;
}

u32 XAcquisition_stsa_top_Get_phase_out(XAcquisition_stsa_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_PHASE_OUT_DATA);
    return Data;
}

u32 XAcquisition_stsa_top_Get_phase_out_vld(XAcquisition_stsa_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_PHASE_OUT_CTRL);
    return Data & 0x1;
}

u64 XAcquisition_stsa_top_Get_peak_out(XAcquisition_stsa_top *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_PEAK_OUT_DATA);
    Data += (u64)XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_PEAK_OUT_DATA + 4) << 32;
    return Data;
}

u32 XAcquisition_stsa_top_Get_peak_out_vld(XAcquisition_stsa_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_PEAK_OUT_CTRL);
    return Data & 0x1;
}

u32 XAcquisition_stsa_top_Get_detected_out(XAcquisition_stsa_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_DETECTED_OUT_DATA);
    return Data;
}

u32 XAcquisition_stsa_top_Get_detected_out_vld(XAcquisition_stsa_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_DETECTED_OUT_CTRL);
    return Data & 0x1;
}

u32 XAcquisition_stsa_top_Get_max_power_out(XAcquisition_stsa_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_MAX_POWER_OUT_DATA);
    return Data;
}

u32 XAcquisition_stsa_top_Get_max_power_out_vld(XAcquisition_stsa_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_MAX_POWER_OUT_CTRL);
    return Data & 0x1;
}

u32 XAcquisition_stsa_top_Get_mean_power_out(XAcquisition_stsa_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_MEAN_POWER_OUT_DATA);
    return Data;
}

u32 XAcquisition_stsa_top_Get_mean_power_out_vld(XAcquisition_stsa_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_MEAN_POWER_OUT_CTRL);
    return Data & 0x1;
}

void XAcquisition_stsa_top_InterruptGlobalEnable(XAcquisition_stsa_top *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAcquisition_stsa_top_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_GIE, 1);
}

void XAcquisition_stsa_top_InterruptGlobalDisable(XAcquisition_stsa_top *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAcquisition_stsa_top_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_GIE, 0);
}

void XAcquisition_stsa_top_InterruptEnable(XAcquisition_stsa_top *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_IER);
    XAcquisition_stsa_top_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_IER, Register | Mask);
}

void XAcquisition_stsa_top_InterruptDisable(XAcquisition_stsa_top *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_IER);
    XAcquisition_stsa_top_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_IER, Register & (~Mask));
}

void XAcquisition_stsa_top_InterruptClear(XAcquisition_stsa_top *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAcquisition_stsa_top_WriteReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_ISR, Mask);
}

u32 XAcquisition_stsa_top_InterruptGetEnabled(XAcquisition_stsa_top *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_IER);
}

u32 XAcquisition_stsa_top_InterruptGetStatus(XAcquisition_stsa_top *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XAcquisition_stsa_top_ReadReg(InstancePtr->Ctrl_BaseAddress, XACQUISITION_STSA_TOP_CTRL_ADDR_ISR);
}

