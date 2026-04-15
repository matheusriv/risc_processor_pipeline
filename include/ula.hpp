#ifndef ULA_HPP
#define ULA_HPP

#include <systemc.h>

enum ULAOP {
    ADD,
    SUB,
    AND,
    OR,
    XOR,
    NOT,
    EQUAL
};

SC_MODULE(ula) {
  sc_in<sc_int<8>> data1, data2;
  sc_in<sc_uint<4>> op;
  sc_out<sc_int<8>> result;
  sc_out<bool> zero;

  void process();

  SC_CTOR(ula);
};

#endif // !ULA_HPP