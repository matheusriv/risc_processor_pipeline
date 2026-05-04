#ifndef IFID_CPP
#define IFID_CPP

#include "register.cpp"
#include <systemc.h>

SC_MODULE(if_id) {
  sc_in<bool> clk;
  sc_in<bool> rst;
  sc_in<bool> earth, vcc;
  sc_in<bool> if_id_write;
  sc_in<sc_uint<32>> in_pc, in_inst;
  sc_out<sc_uint<32>> out_pc, out_inst;
  sc_out<sc_uint<5>> read1, read2, write1;
  sc_out<sc_int<16>> immediate;
  sc_out<sc_uint<26>> absolute;

  reg<32> ifid_pc{"ifid_pc"};
  reg<32> ifid_inst{"ifid_inst"};


  void process();

  SC_CTOR(if_id);
};

void if_id::process() {
  read1.write(ifid_inst.d_out.read().range(15, 11)); // rs
  read2.write(ifid_inst.d_out.read().range(10, 6));  // rt
  write1.write(ifid_inst.d_out.read().range(20, 16)); // rd
  immediate.write(static_cast<sc_int<16>>(ifid_inst.d_out.read().range(31, 16)));
  absolute.write(ifid_inst.d_out.read().range(31, 6)); 
}

if_id::if_id(sc_module_name name) : sc_module(name) {
    ifid_pc.clk(clk);
    ifid_pc.rst(rst);
    ifid_pc.we(if_id_write);
    ifid_pc.d_in(in_pc);
    ifid_pc.d_out(out_pc);

    ifid_inst.clk(clk);
    ifid_inst.rst(rst);
    ifid_inst.we(if_id_write);
    ifid_inst.d_in(in_inst);
    ifid_inst.d_out(out_inst);

  SC_METHOD(process);
  sensitive << ifid_inst.d_out << ifid_pc.d_out;
}

#endif // !IFID_CPP