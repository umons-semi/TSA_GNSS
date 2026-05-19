
wire kernel_monitor_reset;
wire kernel_monitor_clock;
wire kernel_monitor_report;
assign kernel_monitor_reset = ~ap_rst_n;
assign kernel_monitor_clock = ap_clk;
assign kernel_monitor_report = 1'b0;
wire [2:0] axis_block_sigs;
wire [11:0] inst_idle_sigs;
wire [4:0] inst_block_sigs;
wire kernel_block;

assign axis_block_sigs[0] = ~grp_load_inputs_once_fu_283.grp_capture_inputs_to_mem_fu_128.read_inputs_to_stream_U0.rx_real_TDATA_blk_n;
assign axis_block_sigs[1] = ~grp_load_inputs_once_fu_283.grp_capture_inputs_to_mem_fu_128.read_inputs_to_stream_U0.prn_in_TDATA_blk_n;
assign axis_block_sigs[2] = ~grp_run_fd_dataflow_region_fu_333.reduce_all_powers_U0.grp_reduce_all_powers_Pipeline_REDUCE_ALL_fu_134.corr_out_TDATA_blk_n;

assign inst_idle_sigs[0] = grp_load_inputs_once_fu_283.grp_capture_inputs_to_mem_fu_128.read_inputs_to_stream_U0.ap_idle;
assign inst_block_sigs[0] = (grp_load_inputs_once_fu_283.grp_capture_inputs_to_mem_fu_128.read_inputs_to_stream_U0.ap_done & ~grp_load_inputs_once_fu_283.grp_capture_inputs_to_mem_fu_128.read_inputs_to_stream_U0.ap_continue) | ~grp_run_fd_dataflow_region_fu_333.entry_proc_U0.max_val_in_c_blk_n | ~grp_run_fd_dataflow_region_fu_333.entry_proc_U0.sum_corr_in_c_blk_n;
assign inst_idle_sigs[1] = grp_load_inputs_once_fu_283.grp_capture_inputs_to_mem_fu_128.store_stream_to_mem_U0.ap_idle;
assign inst_block_sigs[1] = (grp_load_inputs_once_fu_283.grp_capture_inputs_to_mem_fu_128.store_stream_to_mem_U0.ap_done & ~grp_load_inputs_once_fu_283.grp_capture_inputs_to_mem_fu_128.store_stream_to_mem_U0.ap_continue) | ~grp_run_fd_dataflow_region_fu_333.produce_all_fd_powers_U0.grp_process_one_fd_fu_108.grp_process_tau_df_v9_fu_101.grp_process_tau_tile_v9_fu_76.grp_process_tau_tile_v9_Pipeline_OUTPUT_TILE_fu_717.pow_s_blk_n | ~grp_run_fd_dataflow_region_fu_333.produce_all_fd_powers_U0.nb_fd_c_blk_n;
assign inst_idle_sigs[2] = grp_run_fd_dataflow_region_fu_333.entry_proc_U0.ap_idle;
assign inst_block_sigs[2] = (grp_run_fd_dataflow_region_fu_333.entry_proc_U0.ap_done & ~grp_run_fd_dataflow_region_fu_333.entry_proc_U0.ap_continue) | ~grp_run_fd_dataflow_region_fu_333.reduce_all_powers_U0.grp_reduce_all_powers_Pipeline_REDUCE_ALL_fu_134.pow_s_blk_n | ~grp_run_fd_dataflow_region_fu_333.reduce_all_powers_U0.nb_fd_blk_n | ~grp_run_fd_dataflow_region_fu_333.reduce_all_powers_U0.max_val_in_blk_n | ~grp_run_fd_dataflow_region_fu_333.reduce_all_powers_U0.sum_corr_in_blk_n;
assign inst_idle_sigs[3] = grp_run_fd_dataflow_region_fu_333.produce_all_fd_powers_U0.ap_idle;
assign inst_block_sigs[3] = (grp_run_fd_dataflow_region_fu_333.produce_all_fd_powers_U0.ap_done & ~grp_run_fd_dataflow_region_fu_333.produce_all_fd_powers_U0.ap_continue) | ~grp_load_inputs_once_fu_283.grp_capture_inputs_to_mem_fu_128.read_inputs_to_stream_U0.sig_s_blk_n | ~grp_load_inputs_once_fu_283.grp_capture_inputs_to_mem_fu_128.read_inputs_to_stream_U0.prn_s_blk_n;
assign inst_idle_sigs[4] = grp_run_fd_dataflow_region_fu_333.reduce_all_powers_U0.ap_idle;
assign inst_block_sigs[4] = (grp_run_fd_dataflow_region_fu_333.reduce_all_powers_U0.ap_done & ~grp_run_fd_dataflow_region_fu_333.reduce_all_powers_U0.ap_continue) | ~grp_load_inputs_once_fu_283.grp_capture_inputs_to_mem_fu_128.store_stream_to_mem_U0.sig_s_blk_n | ~grp_load_inputs_once_fu_283.grp_capture_inputs_to_mem_fu_128.store_stream_to_mem_U0.prn_s_blk_n;

assign inst_idle_sigs[5] = 1'b0;
assign inst_idle_sigs[6] = grp_load_inputs_once_fu_283.ap_idle;
assign inst_idle_sigs[7] = grp_load_inputs_once_fu_283.grp_capture_inputs_to_mem_fu_128.ap_idle;
assign inst_idle_sigs[8] = grp_load_inputs_once_fu_283.grp_capture_inputs_to_mem_fu_128.read_inputs_to_stream_U0.ap_idle;
assign inst_idle_sigs[9] = grp_run_fd_dataflow_region_fu_333.ap_idle;
assign inst_idle_sigs[10] = grp_run_fd_dataflow_region_fu_333.reduce_all_powers_U0.ap_idle;
assign inst_idle_sigs[11] = grp_run_fd_dataflow_region_fu_333.reduce_all_powers_U0.grp_reduce_all_powers_Pipeline_REDUCE_ALL_fu_134.ap_idle;

acquisition_serial_hls_deadlock_idx0_monitor acquisition_serial_hls_deadlock_idx0_monitor_U (
    .clock(kernel_monitor_clock),
    .reset(kernel_monitor_reset),
    .axis_block_sigs(axis_block_sigs),
    .inst_idle_sigs(inst_idle_sigs),
    .inst_block_sigs(inst_block_sigs),
    .block(kernel_block)
);


always @ (kernel_block or kernel_monitor_reset) begin
    if (kernel_block == 1'b1 && kernel_monitor_reset == 1'b0) begin
        find_kernel_block = 1'b1;
    end
    else begin
        find_kernel_block = 1'b0;
    end
end
