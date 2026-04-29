#ifndef ADDER_CPP
#define ADDER_CPP

#include <systemc.h>

SC_MODULE(adder) {
  sc_in<sc_uint<32>> a;
  sc_in<sc_uint<32>> b;
  sc_out<sc_uint<32>> d_out;

  void process();

  SC_CTOR(adder);
};

void adder::process() {
  d_out.write(a.read() + b.read());
}

adder::adder(sc_module_name name) : sc_module(name) {
  SC_METHOD(process);
  sensitive << a << b;
}

#endif // ADDER_CPP
