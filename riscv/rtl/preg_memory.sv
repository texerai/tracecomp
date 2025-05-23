/* Copyright (c) 2024 Maveric NU. All rights reserved. */

// ------------------------------------------------------------------------------------------
// This is a nonarchitectural register file for memory stage pipelining.
// ------------------------------------------------------------------------------------------

module preg_memory
// Parameters.
#(
    parameter DATA_WIDTH  = 64,
              ADDR_WIDTH  = 64,
              REG_ADDR_W  = 5
)
// Port decleration. 
(   
    //Input interface. 
    input  logic                      i_clk,
    input  logic                      i_arst,
    input  logic                      i_stall_wb,
    input  logic [              2:0 ] i_result_src,
    input  logic                      i_reg_we,
    input  logic [ ADDR_WIDTH - 1:0 ] i_pc_plus4,
    input  logic [ ADDR_WIDTH - 1:0 ] i_pc_target,
    input  logic [ DATA_WIDTH - 1:0 ] i_imm_ext,
    input  logic [ DATA_WIDTH - 1:0 ] i_alu_result,
    input  logic [ DATA_WIDTH - 1:0 ] i_read_data,
    input  logic                      i_ecall_instr,
    input  logic [              3:0 ] i_cause,
    input  logic [ REG_ADDR_W - 1:0 ] i_rd_addr,
    
    // Output interface.
    output logic [              2:0 ] o_result_src,
    output logic                      o_reg_we,
    output logic [ ADDR_WIDTH - 1:0 ] o_pc_plus4,
    output logic [ ADDR_WIDTH - 1:0 ] o_pc_target,
    output logic [ DATA_WIDTH - 1:0 ] o_imm_ext,
    output logic [ DATA_WIDTH - 1:0 ] o_alu_result,
    output logic [ DATA_WIDTH - 1:0 ] o_read_data,
    output logic                      o_ecall_instr,
    output logic [              3:0 ] o_cause,
    output logic [ REG_ADDR_W - 1:0 ] o_rd_addr
);

    // Write logic.
    always_ff @( posedge i_clk, posedge i_arst ) begin 
        if ( i_arst ) begin
            o_result_src  <= '0;
            o_reg_we      <= '0;
            o_pc_plus4    <= '0;
            o_pc_target   <= '0;
            o_imm_ext     <= '0;
            o_alu_result  <= '0;
            o_read_data   <= '0;
            o_ecall_instr <= '0;
            o_cause       <= '0;
            o_rd_addr     <= '0;
        end
        else if ( ~ i_stall_wb ) begin
            o_result_src  <= i_result_src;
            o_reg_we      <= i_reg_we;
            o_pc_plus4    <= i_pc_plus4;
            o_pc_target   <= i_pc_target;
            o_imm_ext     <= i_imm_ext;
            o_alu_result  <= i_alu_result;
            o_read_data   <= i_read_data;
            o_ecall_instr <= i_ecall_instr;
            o_cause       <= i_cause;
            o_rd_addr     <= i_rd_addr;
        end
    end


endmodule