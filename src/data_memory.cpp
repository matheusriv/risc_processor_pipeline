#ifndef DATA_MEMORY_CPP
#define DATA_MEMORY_CPP

#include <systemc.h>

SC_MODULE(data_memory) {
 
  sc_int<32> mem[2048];
 

  sc_in<bool> clk;
  sc_in<bool> dataRead, dataWrite;
  sc_in<sc_int<32>> address;
  sc_in<sc_int<32>> data;

  sc_out<sc_int<32>> resposta;

  void process();

  SC_CTOR(data_memory);
};

void data_memory::process() {
  if (dataRead.read()) {
    resposta.write(mem[address.read()]);
  } else if (dataWrite.read()) {
    mem[address.read()] = data.read();
  }
}

data_memory::data_memory(sc_module_name name) : sc_module(name) {
  for(int i = 0; i < 2048; i++) {
      mem[i] = 0;
  }
  SC_METHOD(process);
  sensitive << dataRead << dataWrite << address << data;
}

#endif // !DATA_MEMORY_CPP