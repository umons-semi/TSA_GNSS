// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2023.2 (64-bit)
// Tool Version Limit: 2023.10
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#ifdef SDT
#include "xparameters.h"
#endif
#include "xacquisition_stsa_top.h"

extern XAcquisition_stsa_top_Config XAcquisition_stsa_top_ConfigTable[];

#ifdef SDT
XAcquisition_stsa_top_Config *XAcquisition_stsa_top_LookupConfig(UINTPTR BaseAddress) {
	XAcquisition_stsa_top_Config *ConfigPtr = NULL;

	int Index;

	for (Index = (u32)0x0; XAcquisition_stsa_top_ConfigTable[Index].Name != NULL; Index++) {
		if (!BaseAddress || XAcquisition_stsa_top_ConfigTable[Index].Ctrl_BaseAddress == BaseAddress) {
			ConfigPtr = &XAcquisition_stsa_top_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XAcquisition_stsa_top_Initialize(XAcquisition_stsa_top *InstancePtr, UINTPTR BaseAddress) {
	XAcquisition_stsa_top_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XAcquisition_stsa_top_LookupConfig(BaseAddress);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XAcquisition_stsa_top_CfgInitialize(InstancePtr, ConfigPtr);
}
#else
XAcquisition_stsa_top_Config *XAcquisition_stsa_top_LookupConfig(u16 DeviceId) {
	XAcquisition_stsa_top_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XACQUISITION_STSA_TOP_NUM_INSTANCES; Index++) {
		if (XAcquisition_stsa_top_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XAcquisition_stsa_top_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XAcquisition_stsa_top_Initialize(XAcquisition_stsa_top *InstancePtr, u16 DeviceId) {
	XAcquisition_stsa_top_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XAcquisition_stsa_top_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XAcquisition_stsa_top_CfgInitialize(InstancePtr, ConfigPtr);
}
#endif

#endif

