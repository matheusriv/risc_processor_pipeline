#ifndef AND_PORT_CPP
#define AND_PORT_CPP

#include <systemc.h>

SC_MODULE(and_port) {
    sc_in<bool> a, b;
    sc_out<bool> out;

  void process() {
    out.write(a.read() && b.read());
  };

  SC_CTOR(and_port) {
    SC_METHOD(process);
    sensitive << a << b;
  };
};

#endif