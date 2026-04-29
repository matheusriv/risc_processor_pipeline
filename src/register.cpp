#ifndef REGISTER_CPP
#define REGISTER_CPP

#include <systemc.h>

template<int N = 32>
SC_MODULE(reg) {
  sc_in<bool> clk;
  sc_in<bool> rst;
  sc_in<bool> we; // 1 to write
  sc_in<sc_uint<N>> d_in;
  sc_out<sc_uint<N>> d_out;

  void process() {
    while(true) {
        wait(clk.posedge_event());
        if(rst.read()) {
            d_out.write(0);
        } else if (we.read()) {
            d_out.write(d_in.read());
        }
    }
  };

  SC_CTOR(reg) {
    SC_THREAD(process);
  };
};

#endif
