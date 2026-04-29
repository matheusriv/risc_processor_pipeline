#ifndef MEM_WB_CPP
#define MEM_WB_CPP

#include <systemc.h>
#include "register.cpp"
#include "register_int.cpp"
#include "flip_flop_d.cpp"

SC_MODULE(mem_wb) {
  sc_in<bool> clk;
  sc_in<bool> rst;
  sc_in<bool> earth, vcc;


  sc_in<bool> regWrite, memToReg;

  sc_in<sc_int<32>> alu_result, mem_data;
  sc_in<sc_uint<5>> rd;

  flip_flop_d regWrite_reg{"regWrite_reg"};
  flip_flop_d memToReg_reg{"memToReg_reg"};

  register_int<32> alu_result_reg{"alu_result_reg"};
  register_int<32> mem_data_reg{"mem_data_reg"};
  reg<5> rd_reg{"rd_reg"};


  sc_out<bool> regWrite_out, memToReg_out;
  sc_out<sc_int<32>> alu_result_out, mem_data_out;
  sc_out<sc_uint<5>> rd_out;

  void connect_flip_flop_d(flip_flop_d &ff, sc_in<bool> &in, sc_out<bool> &out);

  template<int N>
  void connect_register(reg<N> &reg, sc_in<sc_uint<N>> &in, sc_out<sc_uint<N>> &out);

  template<int N>
  void connect_register(register_int<N> &reg, sc_in<sc_int<N>> &in, sc_out<sc_int<N>> &out);

  SC_CTOR(mem_wb);
};


void mem_wb::connect_flip_flop_d(flip_flop_d &ff, sc_in<bool> &in, sc_out<bool> &out) {
  ff.clk(clk);
  ff.rst(rst);
  ff.a(in);
  ff.b(out);
  ff.we(vcc);
}

template<int N>
void mem_wb::connect_register(reg<N> &reg, sc_in<sc_uint<N>> &in, sc_out<sc_uint<N>> &out) {
  reg.clk(clk);
  reg.rst(rst);
  reg.we(vcc);
  reg.d_in(in);
  reg.d_out(out);
}

template<int N>
void mem_wb::connect_register(register_int<N> &reg, sc_in<sc_int<N>> &in, sc_out<sc_int<N>> &out) {
  reg.clk(clk);
  reg.rst(rst);
  reg.we(vcc);
  reg.d_in(in);
  reg.d_out(out);
}

mem_wb::mem_wb(sc_module_name name) : sc_module(name) {
  connect_flip_flop_d(regWrite_reg, regWrite, regWrite_out);
    connect_flip_flop_d(memToReg_reg, memToReg, memToReg_out);
    connect_register(alu_result_reg, alu_result, alu_result_out);
    connect_register(mem_data_reg, mem_data, mem_data_out);
    connect_register(rd_reg, rd, rd_out);
}

#endif // !MEM_WB_CPP