#ifndef INSTRUCTION_MEMORY_CPP
#define INSTRUCTION_MEMORY_CPP

#include <systemc.h>

SC_MODULE(instruction_memory) {
  sc_uint<8> mem[2048];

  sc_in<sc_uint<32>> address;
  sc_out<sc_uint<32>> word;

  void process();

  SC_CTOR(instruction_memory);
};

void instruction_memory::process() {
    int start = address.read();
    word.write((mem[start], mem[start+1], mem[start+2], mem[start+3]));
}

instruction_memory::instruction_memory(sc_module_name name) : sc_module(name) {
  for(int i = 0; i < 2048; i++) {
      mem[i] = 0;
  }
  SC_METHOD(process);
  sensitive << address;
}

#endif // !INSTRUCTION_MEMORY_CPP