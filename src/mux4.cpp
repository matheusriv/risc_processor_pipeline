#ifndef MUX4_CPP
#define MUX4_CPP

#include "sysc/communication/sc_signal_ports.h"
#include "sysc/kernel/sc_module.h"
#include <systemc.h>

template <typename T>
SC_MODULE(mux4) {
  sc_in<sc_uint<2>> sel;
  sc_in<T> a, b, c, d;
  sc_out<T> out;

  void process() {
    switch (sel.read()) {
        case 0: out.write(a.read()); break;
        case 1: out.write(b.read()); break;
        case 2: out.write(c.read()); break;
        case 3: out.write(d.read()); break;
        default: out.write(0); break;
    }
  };

  SC_CTOR(mux4) {
    SC_METHOD(process);
    sensitive << sel << a << b << c << d;
  };
};

#endif // !MUX4_CPP
