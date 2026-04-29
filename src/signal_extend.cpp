#ifndef SIGNAL_EXTEND_CPP
#define SIGNAL_EXTEND_CPP

#include <systemc.h>

SC_MODULE(signal_extend) {
  sc_in<sc_int<16>> d_in;
  sc_out<sc_int<32>> d_out;

  void process();

  SC_CTOR(signal_extend);
};

void signal_extend::process() {
  d_out.write(d_in.read());
}

signal_extend::signal_extend(sc_module_name name) : sc_module(name) {
  SC_METHOD(process);
  sensitive << d_in;
}

#endif // !SIGNAL_EXTEND_CPP
