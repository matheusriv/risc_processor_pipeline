#ifndef MUX_CONTROL_CPP
#define MUX_CONTROL_CPP

#include <systemc.h>

SC_MODULE(mux_control) {
  sc_in<bool> sel;

  sc_in<bool> isJump, regWrite, op2Sel,
    dataRead, dataWrite, memToReg;
  sc_in<sc_uint<11>> opUla;
  sc_in<sc_uint<2>> flagSel;

  
  sc_out<bool> isJump_out, regWrite_out, op2Sel_out,
    dataRead_out, dataWrite_out, memToReg_out;
  sc_out<sc_uint<11>> opUla_out;
  sc_out<sc_uint<2>> flagSel_out;

  void process();

  SC_CTOR(mux_control);
};


void mux_control::process() {
  if(!sel.read()) {
    isJump_out.write(isJump.read());
    regWrite_out.write(regWrite.read());
    op2Sel_out.write(op2Sel.read());
    dataRead_out.write(dataRead.read());
    dataWrite_out.write(dataWrite.read());
    memToReg_out.write(memToReg.read());
    opUla_out.write(opUla.read());
    flagSel_out.write(flagSel.read());
  } else {
    isJump_out.write(false);
    regWrite_out.write(false);
    op2Sel_out.write(0);
    dataRead_out.write(false);
    dataWrite_out.write(false);
    memToReg_out.write(0);
    opUla_out.write(0);
    flagSel_out.write(0);
  }
}

mux_control::mux_control(sc_module_name name) : sc_module(name) {
  SC_METHOD(process);
  sensitive << sel
   << isJump
   << regWrite
   << op2Sel
   << dataRead
   << dataWrite
   << memToReg
   << opUla
   << flagSel;
}

#endif // !MUX_CONTROL_CPP