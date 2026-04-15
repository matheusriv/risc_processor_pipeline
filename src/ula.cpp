#include "../include/ula.hpp"

void ula::process() {
  sc_int<8> a = data1.read();
  sc_int<8> b = data2.read();
  sc_int<8> r = 0;
  bool z = false;

  switch (op.read()) {
    case ADD: r = a + b; break;
    case SUB: r = a - b; break;
    case AND: r = a & b; break;
    case OR: r = a | b; break;
    case XOR: r = a ^ b; break;
    case NOT: r = ~a; break;
    default: r = 0;
  }

  result.write(r);
  zero.write(r == 0);
}

ula::ula(sc_module_name name) : sc_module(name) {
  SC_METHOD(process);
  sensitive << data1 << data2 << op;
}