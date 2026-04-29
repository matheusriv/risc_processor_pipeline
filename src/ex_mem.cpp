#ifndef EX_MEM_CPP
#define EX_MEM_CPP

#include <systemc.h>
#include "register.cpp"
#include "register_int.cpp"
#include "flip_flop_d.cpp"

SC_MODULE(ex_mem) {
  sc_in<bool> clk;
  sc_in<bool> earth, vcc;

  sc_in<bool> isJump, regWrite,
    dataRead, dataWrite, memToReg;
  sc_in<sc_uint<2>> flagSel;

  sc_in<bool> alu_zero, alu_negative;
  sc_in<sc_int<32>> alu_result, reg_data;
  sc_in<sc_uint<32>> pc;
  sc_in<sc_uint<26>> absolute;
  sc_in<sc_uint<5>> rd;

  flip_flop_d isJump_reg{"isJump_reg"};
  flip_flop_d regWrite_reg{"regWrite_reg"};
  flip_flop_d dataRead_reg{"dataRead_reg"};
  flip_flop_d dataWrite_reg{"dataWrite_reg"};
  flip_flop_d memToReg_reg{"memToReg_reg"};
  flip_flop_d alu_zero_reg{"alu_zero_reg"};
  flip_flop_d alu_negative_reg{"alu_negative_reg"};

    reg<2> flagSel_reg{"flagSel_reg"};
    reg<32> pc_reg{"pc_reg"};
    register_int<32> alu_result_reg{"alu_result_reg"};
    register_int<32> reg_data_reg{"reg_data_reg"};
    reg<26> absolute_reg{"absolute_reg"};
    reg<5> rd_reg{"rd_reg"};


  sc_out<bool> isJump_out, regWrite_out,
    dataRead_out, dataWrite_out, memToReg_out, alu_zero_out, alu_negative_out;
  sc_out<sc_uint<2>> flagSel_out;
  sc_out<sc_uint<5>> rd_out;

  sc_out<sc_uint<32>> pc_out;
  sc_out<sc_int<32>> alu_result_out, reg_data_out;
  sc_out<sc_uint<26>> absolute_out;

  void connect_flip_flop_d(flip_flop_d &ff, sc_in<bool> &in, sc_out<bool> &out);

  template<int N>
  void connect_register(reg<N> &reg, sc_in<sc_uint<N>> &in, sc_out<sc_uint<N>> &out);

  template<int N>
  void connect_register(register_int<N> &reg, sc_in<sc_int<N>> &in, sc_out<sc_int<N>> &out);

  SC_CTOR(ex_mem);
};

void ex_mem::connect_flip_flop_d(flip_flop_d &ff, sc_in<bool> &in, sc_out<bool> &out) {
  ff.clk(clk);
  ff.rst(earth);
  ff.a(in);
  ff.b(out);
  ff.we(vcc);
}

template<int N>
void ex_mem::connect_register(reg<N> &reg, sc_in<sc_uint<N>> &in, sc_out<sc_uint<N>> &out) {
  reg.clk(clk);
  reg.rst(earth);
  reg.we(vcc);
  reg.d_in(in);
  reg.d_out(out);
}

template<int N>
void ex_mem::connect_register(register_int<N> &reg, sc_in<sc_int<N>> &in, sc_out<sc_int<N>> &out) {
  reg.clk(clk);
  reg.rst(earth);
  reg.we(vcc);
  reg.d_in(in);
  reg.d_out(out);
}

ex_mem::ex_mem(sc_module_name name) : sc_module(name) {
  connect_flip_flop_d(isJump_reg, isJump, isJump_out);
  connect_flip_flop_d(regWrite_reg, regWrite, regWrite_out);
    connect_flip_flop_d(dataRead_reg, dataRead, dataRead_out);
    connect_flip_flop_d(dataWrite_reg, dataWrite, dataWrite_out);
    connect_flip_flop_d(alu_zero_reg, alu_zero, alu_zero_out);
    connect_flip_flop_d(alu_negative_reg, alu_negative, alu_negative_out);
    connect_flip_flop_d(memToReg_reg, memToReg, memToReg_out);
    connect_register(alu_result_reg, alu_result, alu_result_out);
    connect_register(flagSel_reg, flagSel, flagSel_out);
    connect_register(pc_reg, pc, pc_out);
    connect_register(reg_data_reg, reg_data, reg_data_out);
    connect_register(absolute_reg, absolute, absolute_out);
    connect_register(rd_reg, rd, rd_out);
}

#endif // EX_MEM_CPP