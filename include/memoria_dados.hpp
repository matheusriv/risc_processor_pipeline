#ifndef MEM_DADOS_HPP
#define MEM_DADOS_HPP

#include <systemc.h>

SC_MODULE(mem_dados) {
 
  sc_int<8> mem[256];
 
  sc_in<bool> clk;
  sc_in<bool> mem_write, mem_read;
  sc_in<sc_int<8>> address;
  sc_in<sc_int<8>> write_data;

  sc_out<sc_int<8>> read_data;

  void process();

  SC_CTOR(mem_dados);
};

#endif