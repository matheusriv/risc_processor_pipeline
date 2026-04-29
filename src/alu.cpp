#ifndef ALU_CPP
#define ALU_CPP

#include <systemc.h>

enum ALUOP {
    ADD,
    SUB,
    AND,
    OR,
    XOR,
    NOT,
    EQUAL
};

SC_MODULE(alu) {
  sc_in<sc_int<32>> a, b;
  sc_in<sc_uint<11>> op;
  sc_out<sc_int<32>> R;
  sc_out<bool> zero, negative;

  void process();

  SC_CTOR(alu);
};

void alu::process() {
  sc_int<32> val_a = a.read();
  sc_int<32> val_b = b.read();
  sc_int<32> r = 0;
  bool z = false, n = false;

  switch (op.read()) {
    case ADD: r = val_a + val_b; break;
    case SUB: r = val_a - val_b; break;
    case AND: r = val_a & val_b; break;
    case OR: r = val_a | val_b; break;
    case XOR: r = val_a ^ val_b; break;
    case NOT: r = ~val_a; break;
    default: r = 0;
  }

  R.write(r);
  zero.write(r == 0);
  negative.write(r < 0);
}

alu::alu(sc_module_name name) : sc_module(name) {
  SC_METHOD(process);
  sensitive << a << b << op;
}

#endif // ALU_CPP
