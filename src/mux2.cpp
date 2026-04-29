#ifndef MUX2_CPP
#define MUX2_CPP

#include "sysc/communication/sc_signal_ports.h"
#include <systemc.h>

template<typename T>
SC_MODULE(mux2) {
  sc_in<bool> sel;
  sc_in<T> a, b;
  sc_out<T> out;

  void process() {
    if(!sel.read()) {
        out.write(a.read());
    } else {
        out.write(b.read());
    }
  }

  SC_CTOR(mux2) {
    SC_METHOD(process);
    sensitive << sel << a << b;
  };
};

#endif // !MUX2_CPP
