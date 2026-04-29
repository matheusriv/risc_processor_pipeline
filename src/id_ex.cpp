#ifndef ID_EX_CPP
#define ID_EX_CPP

#include "register.cpp"
#include "register_int.cpp"
#include "flip_flop_d.cpp"
#include <systemc.h>

SC_MODULE(id_ex) {
  sc_in<bool> clk;
  sc_in<bool> rst;
  sc_in<bool> earth, vcc;


  sc_in<bool> isJump, regWrite, op2Sel,
    dataRead, dataWrite, memToReg;
  sc_in<sc_uint<11>> opAlu;
  sc_in<sc_uint<2>> flagSel;

  sc_in<sc_int<32>> read1, read2, immediate;
  sc_in<sc_uint<32>> pc;
  sc_in<sc_uint<5>> rd, rt, rs;
  sc_in<sc_uint<26>> absolute;

  flip_flop_d isJump_reg{"isJump_reg"};
  flip_flop_d regWrite_reg{"regWrite_reg"};
  flip_flop_d op2Sel_reg{"op2Sel_reg"};
  flip_flop_d dataRead_reg{"dataRead_reg"};
  flip_flop_d dataWrite_reg{"dataWrite_reg"};
  flip_flop_d memToReg_reg{"memToReg_reg"};

    reg<11> opAlu_reg{"opAlu_reg"};
    reg<2> flagSel_reg{"flagSel_reg"};
    register_int<32> read1_reg{"read1_reg"};
    register_int<32> read2_reg{"read2_reg"};
    register_int<32> immediate_reg{"immediate_reg"};
    reg<32> pc_reg{"pc_reg"};
    reg<5> rd_reg{"rd_reg"};
    reg<5> rt_reg{"rt_reg"};
    reg<5> rs_reg{"rs_reg"};
    reg<26> absolute_reg{"absolute_reg"};


  sc_out<bool> isJump_out, regWrite_out, op2Sel_out,
    dataRead_out, dataWrite_out, memToReg_out;
  sc_out<sc_uint<11>> opAlu_out;
  sc_out<sc_uint<2>> flagSel_out;

  sc_out<sc_int<32>> read1_out, read2_out, immediate_out;
  sc_out<sc_uint<32>> pc_out;
  sc_out<sc_uint<5>> rd_out, rt_out, rs_out;
  sc_out<sc_uint<26>> absolute_out;

  void connect_flip_flop_d(flip_flop_d &ff, sc_in<bool> &in, sc_out<bool> &out);

  template<int N>
  void connect_register(reg<N> &reg, sc_in<sc_uint<N>> &in, sc_out<sc_uint<N>> &out);

  template<int N>
    void connect_register(register_int<N> &reg, sc_in<sc_int<N>> &in, sc_out<sc_int<N>> &out);

  SC_CTOR(id_ex);
};

void id_ex::connect_flip_flop_d(flip_flop_d &ff, sc_in<bool> &in, sc_out<bool> &out) {
  ff.clk(clk);
  ff.rst(rst);
  ff.a(in);
  ff.b(out);
  ff.we(vcc);
}

template<int N>
void id_ex::connect_register(reg<N> &reg, sc_in<sc_uint<N>> &in, sc_out<sc_uint<N>> &out) {
  reg.clk(clk);
  reg.rst(rst);
  reg.we(vcc);
  reg.d_in(in);
  reg.d_out(out);
}

template<int N>
void id_ex::connect_register(register_int<N> &reg, sc_in<sc_int<N>> &in, sc_out<sc_int<N>> &out) {
  reg.clk(clk);
  reg.rst(rst);
  reg.we(vcc);
  reg.d_in(in);
  reg.d_out(out);
}

id_ex::id_ex(sc_module_name name) : sc_module(name) {
  connect_flip_flop_d(isJump_reg, isJump, isJump_out);
  connect_flip_flop_d(regWrite_reg, regWrite, regWrite_out);
  connect_flip_flop_d(op2Sel_reg, op2Sel, op2Sel_out);
  connect_flip_flop_d(dataRead_reg, dataRead, dataRead_out);
  connect_flip_flop_d(dataWrite_reg, dataWrite, dataWrite_out);
  connect_flip_flop_d(memToReg_reg, memToReg, memToReg_out);
  connect_register(opAlu_reg, opAlu, opAlu_out);
  connect_register(flagSel_reg, flagSel, flagSel_out);
  connect_register(read1_reg, read1, read1_out);
  connect_register(read2_reg, read2, read2_out);
  connect_register(immediate_reg, immediate, immediate_out);
  connect_register(pc_reg, pc, pc_out);
  connect_register(rd_reg, rd, rd_out);
  connect_register(rt_reg, rt, rt_out);
  connect_register(rs_reg, rs, rs_out);
  connect_register(absolute_reg, absolute, absolute_out);
}

#endif // ID_EX_CPP