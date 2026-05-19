# ==============================================================
# Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2023.2 (64-bit)
# Tool Version Limit: 2023.10
# Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
# Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
# 
# ==============================================================
cmake_minimum_required(VERSION 3.15)
project(acquisition_stsa_top)
find_package(common)
collector_create (PROJECT_LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}")
collect (PROJECT_LIB_SOURCES xacquisition_stsa_top.c)
collect (PROJECT_LIB_SOURCES xacquisition_stsa_top_sinit.c)
collect (PROJECT_LIB_SOURCES xacquisition_stsa_top_linux.c)
collect (PROJECT_LIB_SOURCES xacquisition_stsa_top_g.c)
collector_list (_sources PROJECT_LIB_SOURCES)
collector_create (PROJECT_LIB_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}")
collect (PROJECT_LIB_HEADERS xacquisition_stsa_top.h)
collect (PROJECT_LIB_HEADERS xacquisition_stsa_top_hw.h)
collector_list (_headers PROJECT_LIB_HEADERS)
file(COPY ${_headers} DESTINATION ${CMAKE_BINARY_DIR}/include)
include_directories(${CMAKE_BINARY_DIR}/include)
add_library(acquisition_stsa_top STATIC ${_sources})
set_target_properties(acquisition_stsa_top PROPERTIES LINKER_LANGUAGE C)

