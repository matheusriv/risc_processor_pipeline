#include "../include/mem_dados.hpp"

void mem_dados::process() {
  if (mem_read.read()) {
    read_data.write(mem[address.read()]);
  } else if (mem_write.read()) {
    mem[address.read()] = write_data.read();
  }
}

mem_dados::mem_dados(sc_module_name name) : sc_module(name) {
  SC_METHOD(process);
  sensitive << clk.pos();
}