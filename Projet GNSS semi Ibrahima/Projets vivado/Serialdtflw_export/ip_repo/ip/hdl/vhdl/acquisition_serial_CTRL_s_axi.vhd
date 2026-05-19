-- ==============================================================
-- Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2023.2 (64-bit)
-- Tool Version Limit: 2023.10
-- Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
-- Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
-- 
-- ==============================================================
library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

entity acquisition_serial_CTRL_s_axi is
generic (
    C_S_AXI_ADDR_WIDTH    : INTEGER := 8;
    C_S_AXI_DATA_WIDTH    : INTEGER := 32);
port (
    ACLK                  :in   STD_LOGIC;
    ARESET                :in   STD_LOGIC;
    ACLK_EN               :in   STD_LOGIC;
    AWADDR                :in   STD_LOGIC_VECTOR(C_S_AXI_ADDR_WIDTH-1 downto 0);
    AWVALID               :in   STD_LOGIC;
    AWREADY               :out  STD_LOGIC;
    WDATA                 :in   STD_LOGIC_VECTOR(C_S_AXI_DATA_WIDTH-1 downto 0);
    WSTRB                 :in   STD_LOGIC_VECTOR(C_S_AXI_DATA_WIDTH/8-1 downto 0);
    WVALID                :in   STD_LOGIC;
    WREADY                :out  STD_LOGIC;
    BRESP                 :out  STD_LOGIC_VECTOR(1 downto 0);
    BVALID                :out  STD_LOGIC;
    BREADY                :in   STD_LOGIC;
    ARADDR                :in   STD_LOGIC_VECTOR(C_S_AXI_ADDR_WIDTH-1 downto 0);
    ARVALID               :in   STD_LOGIC;
    ARREADY               :out  STD_LOGIC;
    RDATA                 :out  STD_LOGIC_VECTOR(C_S_AXI_DATA_WIDTH-1 downto 0);
    RRESP                 :out  STD_LOGIC_VECTOR(1 downto 0);
    RVALID                :out  STD_LOGIC;
    RREADY                :in   STD_LOGIC;
    interrupt             :out  STD_LOGIC;
    doppler_out           :in   STD_LOGIC_VECTOR(31 downto 0);
    doppler_out_ap_vld    :in   STD_LOGIC;
    codephase_out         :in   STD_LOGIC_VECTOR(31 downto 0);
    codephase_out_ap_vld  :in   STD_LOGIC;
    sat_detected          :in   STD_LOGIC_VECTOR(31 downto 0);
    sat_detected_ap_vld   :in   STD_LOGIC;
    fd_step               :out  STD_LOGIC_VECTOR(31 downto 0);
    max_power_out         :in   STD_LOGIC_VECTOR(31 downto 0);
    max_power_out_ap_vld  :in   STD_LOGIC;
    mean_power_out        :in   STD_LOGIC_VECTOR(31 downto 0);
    mean_power_out_ap_vld :in   STD_LOGIC;
    rx_count              :in   STD_LOGIC_VECTOR(31 downto 0);
    rx_count_ap_vld       :in   STD_LOGIC;
    prn_count             :in   STD_LOGIC_VECTOR(31 downto 0);
    prn_count_ap_vld      :in   STD_LOGIC;
    rx_last_seen          :in   STD_LOGIC_VECTOR(31 downto 0);
    rx_last_seen_ap_vld   :in   STD_LOGIC;
    prn_last_seen         :in   STD_LOGIC_VECTOR(31 downto 0);
    prn_last_seen_ap_vld  :in   STD_LOGIC;
    rx_last_pos           :in   STD_LOGIC_VECTOR(31 downto 0);
    rx_last_pos_ap_vld    :in   STD_LOGIC;
    prn_last_pos          :in   STD_LOGIC_VECTOR(31 downto 0);
    prn_last_pos_ap_vld   :in   STD_LOGIC;
    ap_start              :out  STD_LOGIC;
    ap_done               :in   STD_LOGIC;
    ap_ready              :in   STD_LOGIC;
    ap_idle               :in   STD_LOGIC
);
end entity acquisition_serial_CTRL_s_axi;

-- ------------------------Address Info-------------------
-- Protocol Used: ap_ctrl_hs
--
-- 0x00 : Control signals
--        bit 0  - ap_start (Read/Write/COH)
--        bit 1  - ap_done (Read/COR)
--        bit 2  - ap_idle (Read)
--        bit 3  - ap_ready (Read/COR)
--        bit 7  - auto_restart (Read/Write)
--        bit 9  - interrupt (Read)
--        others - reserved
-- 0x04 : Global Interrupt Enable Register
--        bit 0  - Global Interrupt Enable (Read/Write)
--        others - reserved
-- 0x08 : IP Interrupt Enable Register (Read/Write)
--        bit 0 - enable ap_done interrupt (Read/Write)
--        bit 1 - enable ap_ready interrupt (Read/Write)
--        others - reserved
-- 0x0c : IP Interrupt Status Register (Read/TOW)
--        bit 0 - ap_done (Read/TOW)
--        bit 1 - ap_ready (Read/TOW)
--        others - reserved
-- 0x10 : Data signal of doppler_out
--        bit 31~0 - doppler_out[31:0] (Read)
-- 0x14 : Control signal of doppler_out
--        bit 0  - doppler_out_ap_vld (Read/COR)
--        others - reserved
-- 0x20 : Data signal of codephase_out
--        bit 31~0 - codephase_out[31:0] (Read)
-- 0x24 : Control signal of codephase_out
--        bit 0  - codephase_out_ap_vld (Read/COR)
--        others - reserved
-- 0x30 : Data signal of sat_detected
--        bit 31~0 - sat_detected[31:0] (Read)
-- 0x34 : Control signal of sat_detected
--        bit 0  - sat_detected_ap_vld (Read/COR)
--        others - reserved
-- 0x40 : Data signal of fd_step
--        bit 31~0 - fd_step[31:0] (Read/Write)
-- 0x44 : reserved
-- 0x48 : Data signal of max_power_out
--        bit 31~0 - max_power_out[31:0] (Read)
-- 0x4c : Control signal of max_power_out
--        bit 0  - max_power_out_ap_vld (Read/COR)
--        others - reserved
-- 0x58 : Data signal of mean_power_out
--        bit 31~0 - mean_power_out[31:0] (Read)
-- 0x5c : Control signal of mean_power_out
--        bit 0  - mean_power_out_ap_vld (Read/COR)
--        others - reserved
-- 0x68 : Data signal of rx_count
--        bit 31~0 - rx_count[31:0] (Read)
-- 0x6c : Control signal of rx_count
--        bit 0  - rx_count_ap_vld (Read/COR)
--        others - reserved
-- 0x78 : Data signal of prn_count
--        bit 31~0 - prn_count[31:0] (Read)
-- 0x7c : Control signal of prn_count
--        bit 0  - prn_count_ap_vld (Read/COR)
--        others - reserved
-- 0x88 : Data signal of rx_last_seen
--        bit 31~0 - rx_last_seen[31:0] (Read)
-- 0x8c : Control signal of rx_last_seen
--        bit 0  - rx_last_seen_ap_vld (Read/COR)
--        others - reserved
-- 0x98 : Data signal of prn_last_seen
--        bit 31~0 - prn_last_seen[31:0] (Read)
-- 0x9c : Control signal of prn_last_seen
--        bit 0  - prn_last_seen_ap_vld (Read/COR)
--        others - reserved
-- 0xa8 : Data signal of rx_last_pos
--        bit 31~0 - rx_last_pos[31:0] (Read)
-- 0xac : Control signal of rx_last_pos
--        bit 0  - rx_last_pos_ap_vld (Read/COR)
--        others - reserved
-- 0xb8 : Data signal of prn_last_pos
--        bit 31~0 - prn_last_pos[31:0] (Read)
-- 0xbc : Control signal of prn_last_pos
--        bit 0  - prn_last_pos_ap_vld (Read/COR)
--        others - reserved
-- (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

architecture behave of acquisition_serial_CTRL_s_axi is
    type states is (wridle, wrdata, wrresp, wrreset, rdidle, rddata, rdreset);  -- read and write fsm states
    signal wstate  : states := wrreset;
    signal rstate  : states := rdreset;
    signal wnext, rnext: states;
    constant ADDR_AP_CTRL               : INTEGER := 16#00#;
    constant ADDR_GIE                   : INTEGER := 16#04#;
    constant ADDR_IER                   : INTEGER := 16#08#;
    constant ADDR_ISR                   : INTEGER := 16#0c#;
    constant ADDR_DOPPLER_OUT_DATA_0    : INTEGER := 16#10#;
    constant ADDR_DOPPLER_OUT_CTRL      : INTEGER := 16#14#;
    constant ADDR_CODEPHASE_OUT_DATA_0  : INTEGER := 16#20#;
    constant ADDR_CODEPHASE_OUT_CTRL    : INTEGER := 16#24#;
    constant ADDR_SAT_DETECTED_DATA_0   : INTEGER := 16#30#;
    constant ADDR_SAT_DETECTED_CTRL     : INTEGER := 16#34#;
    constant ADDR_FD_STEP_DATA_0        : INTEGER := 16#40#;
    constant ADDR_FD_STEP_CTRL          : INTEGER := 16#44#;
    constant ADDR_MAX_POWER_OUT_DATA_0  : INTEGER := 16#48#;
    constant ADDR_MAX_POWER_OUT_CTRL    : INTEGER := 16#4c#;
    constant ADDR_MEAN_POWER_OUT_DATA_0 : INTEGER := 16#58#;
    constant ADDR_MEAN_POWER_OUT_CTRL   : INTEGER := 16#5c#;
    constant ADDR_RX_COUNT_DATA_0       : INTEGER := 16#68#;
    constant ADDR_RX_COUNT_CTRL         : INTEGER := 16#6c#;
    constant ADDR_PRN_COUNT_DATA_0      : INTEGER := 16#78#;
    constant ADDR_PRN_COUNT_CTRL        : INTEGER := 16#7c#;
    constant ADDR_RX_LAST_SEEN_DATA_0   : INTEGER := 16#88#;
    constant ADDR_RX_LAST_SEEN_CTRL     : INTEGER := 16#8c#;
    constant ADDR_PRN_LAST_SEEN_DATA_0  : INTEGER := 16#98#;
    constant ADDR_PRN_LAST_SEEN_CTRL    : INTEGER := 16#9c#;
    constant ADDR_RX_LAST_POS_DATA_0    : INTEGER := 16#a8#;
    constant ADDR_RX_LAST_POS_CTRL      : INTEGER := 16#ac#;
    constant ADDR_PRN_LAST_POS_DATA_0   : INTEGER := 16#b8#;
    constant ADDR_PRN_LAST_POS_CTRL     : INTEGER := 16#bc#;
    constant ADDR_BITS         : INTEGER := 8;

    signal waddr               : UNSIGNED(ADDR_BITS-1 downto 0);
    signal wmask               : UNSIGNED(C_S_AXI_DATA_WIDTH-1 downto 0);
    signal aw_hs               : STD_LOGIC;
    signal w_hs                : STD_LOGIC;
    signal rdata_data          : UNSIGNED(C_S_AXI_DATA_WIDTH-1 downto 0);
    signal ar_hs               : STD_LOGIC;
    signal raddr               : UNSIGNED(ADDR_BITS-1 downto 0);
    signal AWREADY_t           : STD_LOGIC;
    signal WREADY_t            : STD_LOGIC;
    signal ARREADY_t           : STD_LOGIC;
    signal RVALID_t            : STD_LOGIC;
    -- internal registers
    signal int_ap_idle         : STD_LOGIC := '0';
    signal int_ap_ready        : STD_LOGIC := '0';
    signal task_ap_ready       : STD_LOGIC;
    signal int_ap_done         : STD_LOGIC := '0';
    signal task_ap_done        : STD_LOGIC;
    signal int_task_ap_done    : STD_LOGIC := '0';
    signal int_ap_start        : STD_LOGIC := '0';
    signal int_interrupt       : STD_LOGIC := '0';
    signal int_auto_restart    : STD_LOGIC := '0';
    signal auto_restart_status : STD_LOGIC := '0';
    signal auto_restart_done   : STD_LOGIC;
    signal int_gie             : STD_LOGIC := '0';
    signal int_ier             : UNSIGNED(1 downto 0) := (others => '0');
    signal int_isr             : UNSIGNED(1 downto 0) := (others => '0');
    signal int_doppler_out_ap_vld : STD_LOGIC;
    signal int_doppler_out     : UNSIGNED(31 downto 0) := (others => '0');
    signal int_codephase_out_ap_vld : STD_LOGIC;
    signal int_codephase_out   : UNSIGNED(31 downto 0) := (others => '0');
    signal int_sat_detected_ap_vld : STD_LOGIC;
    signal int_sat_detected    : UNSIGNED(31 downto 0) := (others => '0');
    signal int_fd_step         : UNSIGNED(31 downto 0) := (others => '0');
    signal int_max_power_out_ap_vld : STD_LOGIC;
    signal int_max_power_out   : UNSIGNED(31 downto 0) := (others => '0');
    signal int_mean_power_out_ap_vld : STD_LOGIC;
    signal int_mean_power_out  : UNSIGNED(31 downto 0) := (others => '0');
    signal int_rx_count_ap_vld : STD_LOGIC;
    signal int_rx_count        : UNSIGNED(31 downto 0) := (others => '0');
    signal int_prn_count_ap_vld : STD_LOGIC;
    signal int_prn_count       : UNSIGNED(31 downto 0) := (others => '0');
    signal int_rx_last_seen_ap_vld : STD_LOGIC;
    signal int_rx_last_seen    : UNSIGNED(31 downto 0) := (others => '0');
    signal int_prn_last_seen_ap_vld : STD_LOGIC;
    signal int_prn_last_seen   : UNSIGNED(31 downto 0) := (others => '0');
    signal int_rx_last_pos_ap_vld : STD_LOGIC;
    signal int_rx_last_pos     : UNSIGNED(31 downto 0) := (others => '0');
    signal int_prn_last_pos_ap_vld : STD_LOGIC;
    signal int_prn_last_pos    : UNSIGNED(31 downto 0) := (others => '0');


begin
-- ----------------------- Instantiation------------------


-- ----------------------- AXI WRITE ---------------------
    AWREADY_t <=  '1' when wstate = wridle else '0';
    AWREADY   <=  AWREADY_t;
    WREADY_t  <=  '1' when wstate = wrdata else '0';
    WREADY    <=  WREADY_t;
    BRESP     <=  "00";  -- OKAY
    BVALID    <=  '1' when wstate = wrresp else '0';
    wmask     <=  (31 downto 24 => WSTRB(3), 23 downto 16 => WSTRB(2), 15 downto 8 => WSTRB(1), 7 downto 0 => WSTRB(0));
    aw_hs     <=  AWVALID and AWREADY_t;
    w_hs      <=  WVALID and WREADY_t;

    -- write FSM
    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                wstate <= wrreset;
            elsif (ACLK_EN = '1') then
                wstate <= wnext;
            end if;
        end if;
    end process;

    process (wstate, AWVALID, WVALID, BREADY)
    begin
        case (wstate) is
        when wridle =>
            if (AWVALID = '1') then
                wnext <= wrdata;
            else
                wnext <= wridle;
            end if;
        when wrdata =>
            if (WVALID = '1') then
                wnext <= wrresp;
            else
                wnext <= wrdata;
            end if;
        when wrresp =>
            if (BREADY = '1') then
                wnext <= wridle;
            else
                wnext <= wrresp;
            end if;
        when others =>
            wnext <= wridle;
        end case;
    end process;

    waddr_proc : process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (aw_hs = '1') then
                    waddr <= UNSIGNED(AWADDR(ADDR_BITS-1 downto 0));
                end if;
            end if;
        end if;
    end process;

-- ----------------------- AXI READ ----------------------
    ARREADY_t <= '1' when (rstate = rdidle) else '0';
    ARREADY <= ARREADY_t;
    RDATA   <= STD_LOGIC_VECTOR(rdata_data);
    RRESP   <= "00";  -- OKAY
    RVALID_t  <= '1' when (rstate = rddata) else '0';
    RVALID    <= RVALID_t;
    ar_hs   <= ARVALID and ARREADY_t;
    raddr   <= UNSIGNED(ARADDR(ADDR_BITS-1 downto 0));

    -- read FSM
    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                rstate <= rdreset;
            elsif (ACLK_EN = '1') then
                rstate <= rnext;
            end if;
        end if;
    end process;

    process (rstate, ARVALID, RREADY, RVALID_t)
    begin
        case (rstate) is
        when rdidle =>
            if (ARVALID = '1') then
                rnext <= rddata;
            else
                rnext <= rdidle;
            end if;
        when rddata =>
            if (RREADY = '1' and RVALID_t = '1') then
                rnext <= rdidle;
            else
                rnext <= rddata;
            end if;
        when others =>
            rnext <= rdidle;
        end case;
    end process;

    rdata_proc : process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (ar_hs = '1') then
                    rdata_data <= (others => '0');
                    case (TO_INTEGER(raddr)) is
                    when ADDR_AP_CTRL =>
                        rdata_data(9) <= int_interrupt;
                        rdata_data(7) <= int_auto_restart;
                        rdata_data(3) <= int_ap_ready;
                        rdata_data(2) <= int_ap_idle;
                        rdata_data(1) <= int_task_ap_done;
                        rdata_data(0) <= int_ap_start;
                    when ADDR_GIE =>
                        rdata_data(0) <= int_gie;
                    when ADDR_IER =>
                        rdata_data(1 downto 0) <= int_ier;
                    when ADDR_ISR =>
                        rdata_data(1 downto 0) <= int_isr;
                    when ADDR_DOPPLER_OUT_DATA_0 =>
                        rdata_data <= RESIZE(int_doppler_out(31 downto 0), 32);
                    when ADDR_DOPPLER_OUT_CTRL =>
                        rdata_data(0) <= int_doppler_out_ap_vld;
                    when ADDR_CODEPHASE_OUT_DATA_0 =>
                        rdata_data <= RESIZE(int_codephase_out(31 downto 0), 32);
                    when ADDR_CODEPHASE_OUT_CTRL =>
                        rdata_data(0) <= int_codephase_out_ap_vld;
                    when ADDR_SAT_DETECTED_DATA_0 =>
                        rdata_data <= RESIZE(int_sat_detected(31 downto 0), 32);
                    when ADDR_SAT_DETECTED_CTRL =>
                        rdata_data(0) <= int_sat_detected_ap_vld;
                    when ADDR_FD_STEP_DATA_0 =>
                        rdata_data <= RESIZE(int_fd_step(31 downto 0), 32);
                    when ADDR_MAX_POWER_OUT_DATA_0 =>
                        rdata_data <= RESIZE(int_max_power_out(31 downto 0), 32);
                    when ADDR_MAX_POWER_OUT_CTRL =>
                        rdata_data(0) <= int_max_power_out_ap_vld;
                    when ADDR_MEAN_POWER_OUT_DATA_0 =>
                        rdata_data <= RESIZE(int_mean_power_out(31 downto 0), 32);
                    when ADDR_MEAN_POWER_OUT_CTRL =>
                        rdata_data(0) <= int_mean_power_out_ap_vld;
                    when ADDR_RX_COUNT_DATA_0 =>
                        rdata_data <= RESIZE(int_rx_count(31 downto 0), 32);
                    when ADDR_RX_COUNT_CTRL =>
                        rdata_data(0) <= int_rx_count_ap_vld;
                    when ADDR_PRN_COUNT_DATA_0 =>
                        rdata_data <= RESIZE(int_prn_count(31 downto 0), 32);
                    when ADDR_PRN_COUNT_CTRL =>
                        rdata_data(0) <= int_prn_count_ap_vld;
                    when ADDR_RX_LAST_SEEN_DATA_0 =>
                        rdata_data <= RESIZE(int_rx_last_seen(31 downto 0), 32);
                    when ADDR_RX_LAST_SEEN_CTRL =>
                        rdata_data(0) <= int_rx_last_seen_ap_vld;
                    when ADDR_PRN_LAST_SEEN_DATA_0 =>
                        rdata_data <= RESIZE(int_prn_last_seen(31 downto 0), 32);
                    when ADDR_PRN_LAST_SEEN_CTRL =>
                        rdata_data(0) <= int_prn_last_seen_ap_vld;
                    when ADDR_RX_LAST_POS_DATA_0 =>
                        rdata_data <= RESIZE(int_rx_last_pos(31 downto 0), 32);
                    when ADDR_RX_LAST_POS_CTRL =>
                        rdata_data(0) <= int_rx_last_pos_ap_vld;
                    when ADDR_PRN_LAST_POS_DATA_0 =>
                        rdata_data <= RESIZE(int_prn_last_pos(31 downto 0), 32);
                    when ADDR_PRN_LAST_POS_CTRL =>
                        rdata_data(0) <= int_prn_last_pos_ap_vld;
                    when others =>
                        NULL;
                    end case;
                end if;
            end if;
        end if;
    end process;

-- ----------------------- Register logic ----------------
    interrupt            <= int_interrupt;
    ap_start             <= int_ap_start;
    task_ap_done         <= (ap_done and not auto_restart_status) or auto_restart_done;
    task_ap_ready        <= ap_ready and not int_auto_restart;
    auto_restart_done    <= auto_restart_status and (ap_idle and not int_ap_idle);
    fd_step              <= STD_LOGIC_VECTOR(int_fd_step);

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_interrupt <= '0';
            elsif (ACLK_EN = '1') then
                if (int_gie = '1' and (int_isr(0) or int_isr(1)) = '1') then
                    int_interrupt <= '1';
                else
                    int_interrupt <= '0';
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ap_start <= '0';
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_AP_CTRL and WSTRB(0) = '1' and WDATA(0) = '1') then
                    int_ap_start <= '1';
                elsif (ap_ready = '1') then
                    int_ap_start <= int_auto_restart; -- clear on handshake/auto restart
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ap_done <= '0';
            elsif (ACLK_EN = '1') then
                if (true) then
                    int_ap_done <= ap_done;
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_task_ap_done <= '0';
            elsif (ACLK_EN = '1') then
                if (task_ap_done = '1') then
                    int_task_ap_done <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_AP_CTRL) then
                    int_task_ap_done <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ap_idle <= '0';
            elsif (ACLK_EN = '1') then
                if (true) then
                    int_ap_idle <= ap_idle;
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ap_ready <= '0';
            elsif (ACLK_EN = '1') then
                if (task_ap_ready = '1') then
                    int_ap_ready <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_AP_CTRL) then
                    int_ap_ready <= '0';
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_auto_restart <= '0';
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_AP_CTRL and WSTRB(0) = '1') then
                    int_auto_restart <= WDATA(7);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                auto_restart_status <= '0';
            elsif (ACLK_EN = '1') then
                if (int_auto_restart = '1') then
                    auto_restart_status <= '1';
                elsif (ap_idle = '1') then
                    auto_restart_status <= '0';
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_gie <= '0';
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_GIE and WSTRB(0) = '1') then
                    int_gie <= WDATA(0);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ier <= (others=>'0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_IER and WSTRB(0) = '1') then
                    int_ier <= UNSIGNED(WDATA(1 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_isr(0) <= '0';
            elsif (ACLK_EN = '1') then
                if (int_ier(0) = '1' and ap_done = '1') then
                    int_isr(0) <= '1';
                elsif (w_hs = '1' and waddr = ADDR_ISR and WSTRB(0) = '1') then
                    int_isr(0) <= int_isr(0) xor WDATA(0); -- toggle on write
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_isr(1) <= '0';
            elsif (ACLK_EN = '1') then
                if (int_ier(1) = '1' and ap_ready = '1') then
                    int_isr(1) <= '1';
                elsif (w_hs = '1' and waddr = ADDR_ISR and WSTRB(0) = '1') then
                    int_isr(1) <= int_isr(1) xor WDATA(1); -- toggle on write
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_doppler_out <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (doppler_out_ap_vld = '1') then
                    int_doppler_out <= UNSIGNED(doppler_out);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_doppler_out_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (doppler_out_ap_vld = '1') then
                    int_doppler_out_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_DOPPLER_OUT_CTRL) then
                    int_doppler_out_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_codephase_out <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (codephase_out_ap_vld = '1') then
                    int_codephase_out <= UNSIGNED(codephase_out);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_codephase_out_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (codephase_out_ap_vld = '1') then
                    int_codephase_out_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_CODEPHASE_OUT_CTRL) then
                    int_codephase_out_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_sat_detected <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (sat_detected_ap_vld = '1') then
                    int_sat_detected <= UNSIGNED(sat_detected);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_sat_detected_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (sat_detected_ap_vld = '1') then
                    int_sat_detected_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_SAT_DETECTED_CTRL) then
                    int_sat_detected_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_FD_STEP_DATA_0) then
                    int_fd_step(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_fd_step(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_max_power_out <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (max_power_out_ap_vld = '1') then
                    int_max_power_out <= UNSIGNED(max_power_out);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_max_power_out_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (max_power_out_ap_vld = '1') then
                    int_max_power_out_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_MAX_POWER_OUT_CTRL) then
                    int_max_power_out_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_mean_power_out <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (mean_power_out_ap_vld = '1') then
                    int_mean_power_out <= UNSIGNED(mean_power_out);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_mean_power_out_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (mean_power_out_ap_vld = '1') then
                    int_mean_power_out_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_MEAN_POWER_OUT_CTRL) then
                    int_mean_power_out_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_rx_count <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (rx_count_ap_vld = '1') then
                    int_rx_count <= UNSIGNED(rx_count);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_rx_count_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (rx_count_ap_vld = '1') then
                    int_rx_count_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_RX_COUNT_CTRL) then
                    int_rx_count_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_prn_count <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (prn_count_ap_vld = '1') then
                    int_prn_count <= UNSIGNED(prn_count);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_prn_count_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (prn_count_ap_vld = '1') then
                    int_prn_count_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_PRN_COUNT_CTRL) then
                    int_prn_count_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_rx_last_seen <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (rx_last_seen_ap_vld = '1') then
                    int_rx_last_seen <= UNSIGNED(rx_last_seen);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_rx_last_seen_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (rx_last_seen_ap_vld = '1') then
                    int_rx_last_seen_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_RX_LAST_SEEN_CTRL) then
                    int_rx_last_seen_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_prn_last_seen <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (prn_last_seen_ap_vld = '1') then
                    int_prn_last_seen <= UNSIGNED(prn_last_seen);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_prn_last_seen_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (prn_last_seen_ap_vld = '1') then
                    int_prn_last_seen_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_PRN_LAST_SEEN_CTRL) then
                    int_prn_last_seen_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_rx_last_pos <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (rx_last_pos_ap_vld = '1') then
                    int_rx_last_pos <= UNSIGNED(rx_last_pos);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_rx_last_pos_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (rx_last_pos_ap_vld = '1') then
                    int_rx_last_pos_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_RX_LAST_POS_CTRL) then
                    int_rx_last_pos_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_prn_last_pos <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (prn_last_pos_ap_vld = '1') then
                    int_prn_last_pos <= UNSIGNED(prn_last_pos);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_prn_last_pos_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (prn_last_pos_ap_vld = '1') then
                    int_prn_last_pos_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_PRN_LAST_POS_CTRL) then
                    int_prn_last_pos_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;


-- ----------------------- Memory logic ------------------

end architecture behave;
