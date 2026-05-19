// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2023.2 (64-bit)
// Tool Version Limit: 2023.10
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
#ifndef XACQUISITION_SERIAL_H
#define XACQUISITION_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xacquisition_serial_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#else
typedef struct {
#ifdef SDT
    char *Name;
#else
    u16 DeviceId;
#endif
    u64 Ctrl_BaseAddress;
} XAcquisition_serial_Config;
#endif

typedef struct {
    u64 Ctrl_BaseAddress;
    u32 IsReady;
} XAcquisition_serial;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XAcquisition_serial_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XAcquisition_serial_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XAcquisition_serial_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XAcquisition_serial_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
#ifdef SDT
int XAcquisition_serial_Initialize(XAcquisition_serial *InstancePtr, UINTPTR BaseAddress);
XAcquisition_serial_Config* XAcquisition_serial_LookupConfig(UINTPTR BaseAddress);
#else
int XAcquisition_serial_Initialize(XAcquisition_serial *InstancePtr, u16 DeviceId);
XAcquisition_serial_Config* XAcquisition_serial_LookupConfig(u16 DeviceId);
#endif
int XAcquisition_serial_CfgInitialize(XAcquisition_serial *InstancePtr, XAcquisition_serial_Config *ConfigPtr);
#else
int XAcquisition_serial_Initialize(XAcquisition_serial *InstancePtr, const char* InstanceName);
int XAcquisition_serial_Release(XAcquisition_serial *InstancePtr);
#endif

void XAcquisition_serial_Start(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_IsDone(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_IsIdle(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_IsReady(XAcquisition_serial *InstancePtr);
void XAcquisition_serial_EnableAutoRestart(XAcquisition_serial *InstancePtr);
void XAcquisition_serial_DisableAutoRestart(XAcquisition_serial *InstancePtr);

u32 XAcquisition_serial_Get_doppler_out(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_doppler_out_vld(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_codephase_out(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_codephase_out_vld(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_sat_detected(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_sat_detected_vld(XAcquisition_serial *InstancePtr);
void XAcquisition_serial_Set_fd_step(XAcquisition_serial *InstancePtr, u32 Data);
u32 XAcquisition_serial_Get_fd_step(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_max_power_out(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_max_power_out_vld(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_mean_power_out(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_mean_power_out_vld(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_rx_count(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_rx_count_vld(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_prn_count(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_prn_count_vld(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_rx_last_seen(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_rx_last_seen_vld(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_prn_last_seen(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_prn_last_seen_vld(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_rx_last_pos(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_rx_last_pos_vld(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_prn_last_pos(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_Get_prn_last_pos_vld(XAcquisition_serial *InstancePtr);

void XAcquisition_serial_InterruptGlobalEnable(XAcquisition_serial *InstancePtr);
void XAcquisition_serial_InterruptGlobalDisable(XAcquisition_serial *InstancePtr);
void XAcquisition_serial_InterruptEnable(XAcquisition_serial *InstancePtr, u32 Mask);
void XAcquisition_serial_InterruptDisable(XAcquisition_serial *InstancePtr, u32 Mask);
void XAcquisition_serial_InterruptClear(XAcquisition_serial *InstancePtr, u32 Mask);
u32 XAcquisition_serial_InterruptGetEnabled(XAcquisition_serial *InstancePtr);
u32 XAcquisition_serial_InterruptGetStatus(XAcquisition_serial *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
