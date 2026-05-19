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
#include "xacquisition_serial_m_axi.h"

extern XAcquisition_serial_m_axi_Config XAcquisition_serial_m_axi_ConfigTable[];

#ifdef SDT
XAcquisition_serial_m_axi_Config *XAcquisition_serial_m_axi_LookupConfig(UINTPTR BaseAddress) {
	XAcquisition_serial_m_axi_Config *ConfigPtr = NULL;

	int Index;

	for (Index = (u32)0x0; XAcquisition_serial_m_axi_ConfigTable[Index].Name != NULL; Index++) {
		if (!BaseAddress || XAcquisition_serial_m_axi_ConfigTable[Index].Ctrl_BaseAddress == BaseAddress) {
			ConfigPtr = &XAcquisition_serial_m_axi_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XAcquisition_serial_m_axi_Initialize(XAcquisition_serial_m_axi *InstancePtr, UINTPTR BaseAddress) {
	XAcquisition_serial_m_axi_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XAcquisition_serial_m_axi_LookupConfig(BaseAddress);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XAcquisition_serial_m_axi_CfgInitialize(InstancePtr, ConfigPtr);
}
#else
XAcquisition_serial_m_axi_Config *XAcquisition_serial_m_axi_LookupConfig(u16 DeviceId) {
	XAcquisition_serial_m_axi_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XACQUISITION_SERIAL_M_AXI_NUM_INSTANCES; Index++) {
		if (XAcquisition_serial_m_axi_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XAcquisition_serial_m_axi_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XAcquisition_serial_m_axi_Initialize(XAcquisition_serial_m_axi *InstancePtr, u16 DeviceId) {
	XAcquisition_serial_m_axi_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XAcquisition_serial_m_axi_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XAcquisition_serial_m_axi_CfgInitialize(InstancePtr, ConfigPtr);
}
#endif

#endif

