// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2023.2 (64-bit)
// Tool Version Limit: 2023.10
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
`timescale 1ns/1ps
module acquisition_serial_CTRL_s_axi
#(parameter
    C_S_AXI_ADDR_WIDTH = 8,
    C_S_AXI_DATA_WIDTH = 32
)(
    input  wire                          ACLK,
    input  wire                          ARESET,
    input  wire                          ACLK_EN,
    input  wire [C_S_AXI_ADDR_WIDTH-1:0] AWADDR,
    input  wire                          AWVALID,
    output wire                          AWREADY,
    input  wire [C_S_AXI_DATA_WIDTH-1:0] WDATA,
    input  wire [C_S_AXI_DATA_WIDTH/8-1:0] WSTRB,
    input  wire                          WVALID,
    output wire                          WREADY,
    output wire [1:0]                    BRESP,
    output wire                          BVALID,
    input  wire                          BREADY,
    input  wire [C_S_AXI_ADDR_WIDTH-1:0] ARADDR,
    input  wire                          ARVALID,
    output wire                          ARREADY,
    output wire [C_S_AXI_DATA_WIDTH-1:0] RDATA,
    output wire [1:0]                    RRESP,
    output wire                          RVALID,
    input  wire                          RREADY,
    output wire                          interrupt,
    input  wire [31:0]                   doppler_out,
    input  wire                          doppler_out_ap_vld,
    input  wire [31:0]                   codephase_out,
    input  wire                          codephase_out_ap_vld,
    input  wire [31:0]                   sat_detected,
    input  wire                          sat_detected_ap_vld,
    output wire [31:0]                   fd_step,
    input  wire [31:0]                   max_power_out,
    input  wire                          max_power_out_ap_vld,
    input  wire [31:0]                   mean_power_out,
    input  wire                          mean_power_out_ap_vld,
    input  wire [31:0]                   rx_count,
    input  wire                          rx_count_ap_vld,
    input  wire [31:0]                   prn_count,
    input  wire                          prn_count_ap_vld,
    input  wire [31:0]                   rx_last_seen,
    input  wire                          rx_last_seen_ap_vld,
    input  wire [31:0]                   prn_last_seen,
    input  wire                          prn_last_seen_ap_vld,
    input  wire [31:0]                   rx_last_pos,
    input  wire                          rx_last_pos_ap_vld,
    input  wire [31:0]                   prn_last_pos,
    input  wire                          prn_last_pos_ap_vld,
    output wire                          ap_start,
    input  wire                          ap_done,
    input  wire                          ap_ready,
    input  wire                          ap_idle
);
//------------------------Address Info-------------------
// Protocol Used: ap_ctrl_hs
//
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

//------------------------Parameter----------------------
localparam
    ADDR_AP_CTRL               = 8'h00,
    ADDR_GIE                   = 8'h04,
    ADDR_IER                   = 8'h08,
    ADDR_ISR                   = 8'h0c,
    ADDR_DOPPLER_OUT_DATA_0    = 8'h10,
    ADDR_DOPPLER_OUT_CTRL      = 8'h14,
    ADDR_CODEPHASE_OUT_DATA_0  = 8'h20,
    ADDR_CODEPHASE_OUT_CTRL    = 8'h24,
    ADDR_SAT_DETECTED_DATA_0   = 8'h30,
    ADDR_SAT_DETECTED_CTRL     = 8'h34,
    ADDR_FD_STEP_DATA_0        = 8'h40,
    ADDR_FD_STEP_CTRL          = 8'h44,
    ADDR_MAX_POWER_OUT_DATA_0  = 8'h48,
    ADDR_MAX_POWER_OUT_CTRL    = 8'h4c,
    ADDR_MEAN_POWER_OUT_DATA_0 = 8'h58,
    ADDR_MEAN_POWER_OUT_CTRL   = 8'h5c,
    ADDR_RX_COUNT_DATA_0       = 8'h68,
    ADDR_RX_COUNT_CTRL         = 8'h6c,
    ADDR_PRN_COUNT_DATA_0      = 8'h78,
    ADDR_PRN_COUNT_CTRL        = 8'h7c,
    ADDR_RX_LAST_SEEN_DATA_0   = 8'h88,
    ADDR_RX_LAST_SEEN_CTRL     = 8'h8c,
    ADDR_PRN_LAST_SEEN_DATA_0  = 8'h98,
    ADDR_PRN_LAST_SEEN_CTRL    = 8'h9c,
    ADDR_RX_LAST_POS_DATA_0    = 8'ha8,
    ADDR_RX_LAST_POS_CTRL      = 8'hac,
    ADDR_PRN_LAST_POS_DATA_0   = 8'hb8,
    ADDR_PRN_LAST_POS_CTRL     = 8'hbc,
    WRIDLE                     = 2'd0,
    WRDATA                     = 2'd1,
    WRRESP                     = 2'd2,
    WRRESET                    = 2'd3,
    RDIDLE                     = 2'd0,
    RDDATA                     = 2'd1,
    RDRESET                    = 2'd2,
    ADDR_BITS                = 8;

//------------------------Local signal-------------------
    reg  [1:0]                    wstate = WRRESET;
    reg  [1:0]                    wnext;
    reg  [ADDR_BITS-1:0]          waddr;
    wire [C_S_AXI_DATA_WIDTH-1:0] wmask;
    wire                          aw_hs;
    wire                          w_hs;
    reg  [1:0]                    rstate = RDRESET;
    reg  [1:0]                    rnext;
    reg  [C_S_AXI_DATA_WIDTH-1:0] rdata;
    wire                          ar_hs;
    wire [ADDR_BITS-1:0]          raddr;
    // internal registers
    reg                           int_ap_idle;
    reg                           int_ap_ready = 1'b0;
    wire                          task_ap_ready;
    reg                           int_ap_done = 1'b0;
    wire                          task_ap_done;
    reg                           int_task_ap_done = 1'b0;
    reg                           int_ap_start = 1'b0;
    reg                           int_interrupt = 1'b0;
    reg                           int_auto_restart = 1'b0;
    reg                           auto_restart_status = 1'b0;
    wire                          auto_restart_done;
    reg                           int_gie = 1'b0;
    reg  [1:0]                    int_ier = 2'b0;
    reg  [1:0]                    int_isr = 2'b0;
    reg                           int_doppler_out_ap_vld;
    reg  [31:0]                   int_doppler_out = 'b0;
    reg                           int_codephase_out_ap_vld;
    reg  [31:0]                   int_codephase_out = 'b0;
    reg                           int_sat_detected_ap_vld;
    reg  [31:0]                   int_sat_detected = 'b0;
    reg  [31:0]                   int_fd_step = 'b0;
    reg                           int_max_power_out_ap_vld;
    reg  [31:0]                   int_max_power_out = 'b0;
    reg                           int_mean_power_out_ap_vld;
    reg  [31:0]                   int_mean_power_out = 'b0;
    reg                           int_rx_count_ap_vld;
    reg  [31:0]                   int_rx_count = 'b0;
    reg                           int_prn_count_ap_vld;
    reg  [31:0]                   int_prn_count = 'b0;
    reg                           int_rx_last_seen_ap_vld;
    reg  [31:0]                   int_rx_last_seen = 'b0;
    reg                           int_prn_last_seen_ap_vld;
    reg  [31:0]                   int_prn_last_seen = 'b0;
    reg                           int_rx_last_pos_ap_vld;
    reg  [31:0]                   int_rx_last_pos = 'b0;
    reg                           int_prn_last_pos_ap_vld;
    reg  [31:0]                   int_prn_last_pos = 'b0;

//------------------------Instantiation------------------


//------------------------AXI write fsm------------------
assign AWREADY = (wstate == WRIDLE);
assign WREADY  = (wstate == WRDATA);
assign BRESP   = 2'b00;  // OKAY
assign BVALID  = (wstate == WRRESP);
assign wmask   = { {8{WSTRB[3]}}, {8{WSTRB[2]}}, {8{WSTRB[1]}}, {8{WSTRB[0]}} };
assign aw_hs   = AWVALID & AWREADY;
assign w_hs    = WVALID & WREADY;

// wstate
always @(posedge ACLK) begin
    if (ARESET)
        wstate <= WRRESET;
    else if (ACLK_EN)
        wstate <= wnext;
end

// wnext
always @(*) begin
    case (wstate)
        WRIDLE:
            if (AWVALID)
                wnext = WRDATA;
            else
                wnext = WRIDLE;
        WRDATA:
            if (WVALID)
                wnext = WRRESP;
            else
                wnext = WRDATA;
        WRRESP:
            if (BREADY)
                wnext = WRIDLE;
            else
                wnext = WRRESP;
        default:
            wnext = WRIDLE;
    endcase
end

// waddr
always @(posedge ACLK) begin
    if (ACLK_EN) begin
        if (aw_hs)
            waddr <= AWADDR[ADDR_BITS-1:0];
    end
end

//------------------------AXI read fsm-------------------
assign ARREADY = (rstate == RDIDLE);
assign RDATA   = rdata;
assign RRESP   = 2'b00;  // OKAY
assign RVALID  = (rstate == RDDATA);
assign ar_hs   = ARVALID & ARREADY;
assign raddr   = ARADDR[ADDR_BITS-1:0];

// rstate
always @(posedge ACLK) begin
    if (ARESET)
        rstate <= RDRESET;
    else if (ACLK_EN)
        rstate <= rnext;
end

// rnext
always @(*) begin
    case (rstate)
        RDIDLE:
            if (ARVALID)
                rnext = RDDATA;
            else
                rnext = RDIDLE;
        RDDATA:
            if (RREADY & RVALID)
                rnext = RDIDLE;
            else
                rnext = RDDATA;
        default:
            rnext = RDIDLE;
    endcase
end

// rdata
always @(posedge ACLK) begin
    if (ACLK_EN) begin
        if (ar_hs) begin
            rdata <= 'b0;
            case (raddr)
                ADDR_AP_CTRL: begin
                    rdata[0] <= int_ap_start;
                    rdata[1] <= int_task_ap_done;
                    rdata[2] <= int_ap_idle;
                    rdata[3] <= int_ap_ready;
                    rdata[7] <= int_auto_restart;
                    rdata[9] <= int_interrupt;
                end
                ADDR_GIE: begin
                    rdata <= int_gie;
                end
                ADDR_IER: begin
                    rdata <= int_ier;
                end
                ADDR_ISR: begin
                    rdata <= int_isr;
                end
                ADDR_DOPPLER_OUT_DATA_0: begin
                    rdata <= int_doppler_out[31:0];
                end
                ADDR_DOPPLER_OUT_CTRL: begin
                    rdata[0] <= int_doppler_out_ap_vld;
                end
                ADDR_CODEPHASE_OUT_DATA_0: begin
                    rdata <= int_codephase_out[31:0];
                end
                ADDR_CODEPHASE_OUT_CTRL: begin
                    rdata[0] <= int_codephase_out_ap_vld;
                end
                ADDR_SAT_DETECTED_DATA_0: begin
                    rdata <= int_sat_detected[31:0];
                end
                ADDR_SAT_DETECTED_CTRL: begin
                    rdata[0] <= int_sat_detected_ap_vld;
                end
                ADDR_FD_STEP_DATA_0: begin
                    rdata <= int_fd_step[31:0];
                end
                ADDR_MAX_POWER_OUT_DATA_0: begin
                    rdata <= int_max_power_out[31:0];
                end
                ADDR_MAX_POWER_OUT_CTRL: begin
                    rdata[0] <= int_max_power_out_ap_vld;
                end
                ADDR_MEAN_POWER_OUT_DATA_0: begin
                    rdata <= int_mean_power_out[31:0];
                end
                ADDR_MEAN_POWER_OUT_CTRL: begin
                    rdata[0] <= int_mean_power_out_ap_vld;
                end
                ADDR_RX_COUNT_DATA_0: begin
                    rdata <= int_rx_count[31:0];
                end
                ADDR_RX_COUNT_CTRL: begin
                    rdata[0] <= int_rx_count_ap_vld;
                end
                ADDR_PRN_COUNT_DATA_0: begin
                    rdata <= int_prn_count[31:0];
                end
                ADDR_PRN_COUNT_CTRL: begin
                    rdata[0] <= int_prn_count_ap_vld;
                end
                ADDR_RX_LAST_SEEN_DATA_0: begin
                    rdata <= int_rx_last_seen[31:0];
                end
                ADDR_RX_LAST_SEEN_CTRL: begin
                    rdata[0] <= int_rx_last_seen_ap_vld;
                end
                ADDR_PRN_LAST_SEEN_DATA_0: begin
                    rdata <= int_prn_last_seen[31:0];
                end
                ADDR_PRN_LAST_SEEN_CTRL: begin
                    rdata[0] <= int_prn_last_seen_ap_vld;
                end
                ADDR_RX_LAST_POS_DATA_0: begin
                    rdata <= int_rx_last_pos[31:0];
                end
                ADDR_RX_LAST_POS_CTRL: begin
                    rdata[0] <= int_rx_last_pos_ap_vld;
                end
                ADDR_PRN_LAST_POS_DATA_0: begin
                    rdata <= int_prn_last_pos[31:0];
                end
                ADDR_PRN_LAST_POS_CTRL: begin
                    rdata[0] <= int_prn_last_pos_ap_vld;
                end
            endcase
        end
    end
end


//------------------------Register logic-----------------
assign interrupt         = int_interrupt;
assign ap_start          = int_ap_start;
assign task_ap_done      = (ap_done && !auto_restart_status) || auto_restart_done;
assign task_ap_ready     = ap_ready && !int_auto_restart;
assign auto_restart_done = auto_restart_status && (ap_idle && !int_ap_idle);
assign fd_step           = int_fd_step;
// int_interrupt
always @(posedge ACLK) begin
    if (ARESET)
        int_interrupt <= 1'b0;
    else if (ACLK_EN) begin
        if (int_gie && (|int_isr))
            int_interrupt <= 1'b1;
        else
            int_interrupt <= 1'b0;
    end
end

// int_ap_start
always @(posedge ACLK) begin
    if (ARESET)
        int_ap_start <= 1'b0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_AP_CTRL && WSTRB[0] && WDATA[0])
            int_ap_start <= 1'b1;
        else if (ap_ready)
            int_ap_start <= int_auto_restart; // clear on handshake/auto restart
    end
end

// int_ap_done
always @(posedge ACLK) begin
    if (ARESET)
        int_ap_done <= 1'b0;
    else if (ACLK_EN) begin
            int_ap_done <= ap_done;
    end
end

// int_task_ap_done
always @(posedge ACLK) begin
    if (ARESET)
        int_task_ap_done <= 1'b0;
    else if (ACLK_EN) begin
        if (task_ap_done)
            int_task_ap_done <= 1'b1;
        else if (ar_hs && raddr == ADDR_AP_CTRL)
            int_task_ap_done <= 1'b0; // clear on read
    end
end

// int_ap_idle
always @(posedge ACLK) begin
    if (ARESET)
        int_ap_idle <= 1'b0;
    else if (ACLK_EN) begin
            int_ap_idle <= ap_idle;
    end
end

// int_ap_ready
always @(posedge ACLK) begin
    if (ARESET)
        int_ap_ready <= 1'b0;
    else if (ACLK_EN) begin
        if (task_ap_ready)
            int_ap_ready <= 1'b1;
        else if (ar_hs && raddr == ADDR_AP_CTRL)
            int_ap_ready <= 1'b0;
    end
end

// int_auto_restart
always @(posedge ACLK) begin
    if (ARESET)
        int_auto_restart <= 1'b0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_AP_CTRL && WSTRB[0])
            int_auto_restart <=  WDATA[7];
    end
end

// auto_restart_status
always @(posedge ACLK) begin
    if (ARESET)
        auto_restart_status <= 1'b0;
    else if (ACLK_EN) begin
        if (int_auto_restart)
            auto_restart_status <= 1'b1;
        else if (ap_idle)
            auto_restart_status <= 1'b0;
    end
end

// int_gie
always @(posedge ACLK) begin
    if (ARESET)
        int_gie <= 1'b0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_GIE && WSTRB[0])
            int_gie <= WDATA[0];
    end
end

// int_ier
always @(posedge ACLK) begin
    if (ARESET)
        int_ier <= 1'b0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_IER && WSTRB[0])
            int_ier <= WDATA[1:0];
    end
end

// int_isr[0]
always @(posedge ACLK) begin
    if (ARESET)
        int_isr[0] <= 1'b0;
    else if (ACLK_EN) begin
        if (int_ier[0] & ap_done)
            int_isr[0] <= 1'b1;
        else if (w_hs && waddr == ADDR_ISR && WSTRB[0])
            int_isr[0] <= int_isr[0] ^ WDATA[0]; // toggle on write
    end
end

// int_isr[1]
always @(posedge ACLK) begin
    if (ARESET)
        int_isr[1] <= 1'b0;
    else if (ACLK_EN) begin
        if (int_ier[1] & ap_ready)
            int_isr[1] <= 1'b1;
        else if (w_hs && waddr == ADDR_ISR && WSTRB[0])
            int_isr[1] <= int_isr[1] ^ WDATA[1]; // toggle on write
    end
end

// int_doppler_out
always @(posedge ACLK) begin
    if (ARESET)
        int_doppler_out <= 0;
    else if (ACLK_EN) begin
        if (doppler_out_ap_vld)
            int_doppler_out <= doppler_out;
    end
end

// int_doppler_out_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_doppler_out_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (doppler_out_ap_vld)
            int_doppler_out_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_DOPPLER_OUT_CTRL)
            int_doppler_out_ap_vld <= 1'b0; // clear on read
    end
end

// int_codephase_out
always @(posedge ACLK) begin
    if (ARESET)
        int_codephase_out <= 0;
    else if (ACLK_EN) begin
        if (codephase_out_ap_vld)
            int_codephase_out <= codephase_out;
    end
end

// int_codephase_out_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_codephase_out_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (codephase_out_ap_vld)
            int_codephase_out_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_CODEPHASE_OUT_CTRL)
            int_codephase_out_ap_vld <= 1'b0; // clear on read
    end
end

// int_sat_detected
always @(posedge ACLK) begin
    if (ARESET)
        int_sat_detected <= 0;
    else if (ACLK_EN) begin
        if (sat_detected_ap_vld)
            int_sat_detected <= sat_detected;
    end
end

// int_sat_detected_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_sat_detected_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (sat_detected_ap_vld)
            int_sat_detected_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_SAT_DETECTED_CTRL)
            int_sat_detected_ap_vld <= 1'b0; // clear on read
    end
end

// int_fd_step[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_fd_step[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_FD_STEP_DATA_0)
            int_fd_step[31:0] <= (WDATA[31:0] & wmask) | (int_fd_step[31:0] & ~wmask);
    end
end

// int_max_power_out
always @(posedge ACLK) begin
    if (ARESET)
        int_max_power_out <= 0;
    else if (ACLK_EN) begin
        if (max_power_out_ap_vld)
            int_max_power_out <= max_power_out;
    end
end

// int_max_power_out_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_max_power_out_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (max_power_out_ap_vld)
            int_max_power_out_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_MAX_POWER_OUT_CTRL)
            int_max_power_out_ap_vld <= 1'b0; // clear on read
    end
end

// int_mean_power_out
always @(posedge ACLK) begin
    if (ARESET)
        int_mean_power_out <= 0;
    else if (ACLK_EN) begin
        if (mean_power_out_ap_vld)
            int_mean_power_out <= mean_power_out;
    end
end

// int_mean_power_out_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_mean_power_out_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (mean_power_out_ap_vld)
            int_mean_power_out_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_MEAN_POWER_OUT_CTRL)
            int_mean_power_out_ap_vld <= 1'b0; // clear on read
    end
end

// int_rx_count
always @(posedge ACLK) begin
    if (ARESET)
        int_rx_count <= 0;
    else if (ACLK_EN) begin
        if (rx_count_ap_vld)
            int_rx_count <= rx_count;
    end
end

// int_rx_count_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_rx_count_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (rx_count_ap_vld)
            int_rx_count_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_RX_COUNT_CTRL)
            int_rx_count_ap_vld <= 1'b0; // clear on read
    end
end

// int_prn_count
always @(posedge ACLK) begin
    if (ARESET)
        int_prn_count <= 0;
    else if (ACLK_EN) begin
        if (prn_count_ap_vld)
            int_prn_count <= prn_count;
    end
end

// int_prn_count_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_prn_count_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (prn_count_ap_vld)
            int_prn_count_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_PRN_COUNT_CTRL)
            int_prn_count_ap_vld <= 1'b0; // clear on read
    end
end

// int_rx_last_seen
always @(posedge ACLK) begin
    if (ARESET)
        int_rx_last_seen <= 0;
    else if (ACLK_EN) begin
        if (rx_last_seen_ap_vld)
            int_rx_last_seen <= rx_last_seen;
    end
end

// int_rx_last_seen_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_rx_last_seen_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (rx_last_seen_ap_vld)
            int_rx_last_seen_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_RX_LAST_SEEN_CTRL)
            int_rx_last_seen_ap_vld <= 1'b0; // clear on read
    end
end

// int_prn_last_seen
always @(posedge ACLK) begin
    if (ARESET)
        int_prn_last_seen <= 0;
    else if (ACLK_EN) begin
        if (prn_last_seen_ap_vld)
            int_prn_last_seen <= prn_last_seen;
    end
end

// int_prn_last_seen_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_prn_last_seen_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (prn_last_seen_ap_vld)
            int_prn_last_seen_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_PRN_LAST_SEEN_CTRL)
            int_prn_last_seen_ap_vld <= 1'b0; // clear on read
    end
end

// int_rx_last_pos
always @(posedge ACLK) begin
    if (ARESET)
        int_rx_last_pos <= 0;
    else if (ACLK_EN) begin
        if (rx_last_pos_ap_vld)
            int_rx_last_pos <= rx_last_pos;
    end
end

// int_rx_last_pos_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_rx_last_pos_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (rx_last_pos_ap_vld)
            int_rx_last_pos_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_RX_LAST_POS_CTRL)
            int_rx_last_pos_ap_vld <= 1'b0; // clear on read
    end
end

// int_prn_last_pos
always @(posedge ACLK) begin
    if (ARESET)
        int_prn_last_pos <= 0;
    else if (ACLK_EN) begin
        if (prn_last_pos_ap_vld)
            int_prn_last_pos <= prn_last_pos;
    end
end

// int_prn_last_pos_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_prn_last_pos_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (prn_last_pos_ap_vld)
            int_prn_last_pos_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_PRN_LAST_POS_CTRL)
            int_prn_last_pos_ap_vld <= 1'b0; // clear on read
    end
end

//synthesis translate_off
always @(posedge ACLK) begin
    if (ACLK_EN) begin
        if (int_gie & ~int_isr[0] & int_ier[0] & ap_done)
            $display ("// Interrupt Monitor : interrupt for ap_done detected @ \"%0t\"", $time);
        if (int_gie & ~int_isr[1] & int_ier[1] & ap_ready)
            $display ("// Interrupt Monitor : interrupt for ap_ready detected @ \"%0t\"", $time);
    end
end
//synthesis translate_on

//------------------------Memory logic-------------------

endmodule
