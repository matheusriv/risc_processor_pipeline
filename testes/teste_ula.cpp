#include "../include/ula.hpp"
#include "systemc.h"
#include "ostream"
#include <iostream>

SC_MODULE(test_ula) {
  sc_signal<sc_int<8>> A, B, R;
  sc_signal<sc_uint<4>> op;
  sc_signal<bool> zero;

  ula uut{"ula"};

  void test() {
    // Teste ADD
    A.write(10);
    B.write(6);
    op.write(ADD);
    wait(5, SC_NS);
    std::cout << "[ADD] (" << A.read() << " + " << B.read() << ") = " << R.read() << " | Zero: " << zero.read() << std::endl;

    // Teste SUB
    A.write(10);
    B.write(6);
    op.write(SUB);
    wait(5, SC_NS);
    std::cout << "[SUB] (" << A.read() << " - " << B.read() << ") = " << R.read() << " | Zero: " << zero.read() << std::endl;

    // Teste SUB com resultado ZERO
    A.write(9);
    B.write(9);
    op.write(SUB);
    wait(5, SC_NS);
    std::cout << "[SUB] (" << A.read() << " - " << B.read() << ") = " << R.read() << " | Zero: " << zero.read() << std::endl;

    // Teste AND (12 = 1100 em binário, 10 = 1010 em binário. AND deve dar 8 = 1000)
    A.write(12); B.write(10); op.write(AND);
    wait(5, SC_NS);
    std::cout << "[AND] (" << A.read() << " & " << B.read() << ") = " << R.read() << " | Zero: " << zero.read() << std::endl;

    // Teste OR (OR deve dar 14 = 1110)
    A.write(12); B.write(10); op.write(OR);
    wait(5, SC_NS);
    std::cout << "[OR]  (" << A.read() << " | " << B.read() << ") = " << R.read() << " | Zero: " << zero.read() << std::endl;

    // Teste XOR (XOR deve dar 6 = 0110)
    A.write(12); B.write(10); op.write(XOR);
    wait(5, SC_NS);
    std::cout << "[XOR] (" << A.read() << " ^ " << B.read() << ") = " << R.read() << " | Zero: " << zero.read() << std::endl;

    // Teste NOT (Inverte os bits de A)
    A.write(15); B.write(0); op.write(NOT);
    wait(5, SC_NS);
    std::cout << "[NOT] (~" << A.read() << ") = " << R.read() << " | Zero: " << zero.read() << std::endl;

    sc_stop(); // Encerra a simulação
  }

  SC_CTOR(test_ula) : uut("ula") {
    uut.data1(A);
    uut.data2(B);
    uut.op(op);
    uut.result(R);
    uut.zero(zero);

    SC_THREAD(test);
  }
};

int sc_main(int argc, char **argv) {
  test_ula tb("tb");
  sc_start();

  return 0;
}