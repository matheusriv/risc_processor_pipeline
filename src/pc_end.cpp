#ifndef PC_END_CPP
#define PC_END_CPP

#include <systemc.h>

SC_MODULE(pc_end) {
  sc_in<sc_uint<32>> pc;
  sc_in<sc_uint<26>> absolut;

  sc_out<sc_uint<32>> resposta;

  void process();

  SC_CTOR(pc_end);
};

void pc_end::process() {
  sc_uint<4> pc_4msbs = pc.read().range(31, 28);
  resposta.write((pc_4msbs, absolut.read(), 0));
}

pc_end::pc_end(sc_module_name name) : sc_module(name) {
  SC_METHOD(process);
  sensitive << pc << absolut;
}

#endif // !PC_END_CPP