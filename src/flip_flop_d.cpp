#ifndef FLIP_FLOP_D_CPP
#define FLIP_FLOP_D_CPP

#include "sysc/communication/sc_signal_ports.h"
#include "sysc/kernel/sc_module.h"
#include <systemc.h>

SC_MODULE(flip_flop_d) {
  sc_in<bool> clk;
  sc_in<bool> rst;
  sc_in<bool> we; // 1 for write
  sc_in<bool> a; // entrada
  sc_out<bool> b; // saída;

  void process();

  SC_CTOR(flip_flop_d);
};

void flip_flop_d::process() {
  while(true) {
    wait(clk.posedge_event());
    if(rst.read()) {
        b.write(false);
    } else if (we.read()) {
        b.write(a.read());
    }
  }
}

flip_flop_d::flip_flop_d(sc_module_name name) : sc_module(name) {
  SC_THREAD(process);
}

#endif // !FLIP_FLOP_D_CPP