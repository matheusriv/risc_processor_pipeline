#include "../src/alu.cpp"
#include <systemc.h>
#include <iostream>

SC_MODULE(test_alu) {
  sc_signal<sc_int<32>> a, b, R;
  sc_signal<sc_uint<11>> op;
  sc_signal<bool> zero, negative;

  alu uut{"alu"};

  void test() {
    // Testes de Subtração
    a.write(10);
    b.write(6);
    op.write(ALUOP::SUB);
    wait(5, SC_NS);
    std::cout << "[SUB] (" << a.read() << " - " << b.read() << ") = " << R.read() << " | Negative: " << negative.read() << std::endl;

    a.write(10);
    b.write(9);
    wait(5, SC_NS);
    std::cout << "[SUB] (" << a.read() << " - " << b.read() << ") = " << R.read() << " | Zero: " << zero.read() << std::endl;

    a.write(9);
    b.write(9);
    op.write(ALUOP::SUB);
    wait(5, SC_NS);
    std::cout << "[SUB] (" << a.read() << " - " << b.read() << ") = " << R.read() << " | Zero: " << zero.read() << std::endl;


    a.write(6);
    b.write(10);
    op.write(ALUOP::SUB);
    wait(5, SC_NS);
    std::cout << "[SUB] (" << a.read() << " - " << b.read() << ") = " << R.read() << " | Negative: " << negative.read() << std::endl;

    // Teste de Adição
    a.write(15);
    b.write(20);
    op.write(ALUOP::ADD);
    wait(5, SC_NS);
    std::cout << "[ADD] (" << a.read() << " + " << b.read() << ") = " << R.read() << std::endl;

    // Teste AND Lógico
    a.write(12); // 1100
    b.write(10); // 1010
    op.write(ALUOP::AND);
    wait(5, SC_NS);
    std::cout << "[AND] (" << a.read() << " & " << b.read() << ") = " << R.read() << std::endl;

    // Teste OR Lógico
    a.write(12);
    b.write(10);
    op.write(ALUOP::OR);
    wait(5, SC_NS);
    std::cout << "[OR]  (" << a.read() << " | " << b.read() << ") = " << R.read() << std::endl;
    /*
    // Teste XOR Lógico
    a.write(12);
    b.write(10);
    op.write(ALUOP::XOR);
    wait(5, SC_NS);
    std::cout << "[XOR] (" << a.read() << " ^ " << b.read() << ") = " << R.read() << std::endl;

    // Teste NOT Lógico
    a.write(0);
    op.write(ALUOP::NOT);
    wait(5, SC_NS);
    std::cout << "[NOT] (~" << a.read() << ") = " << R.read() << std::endl;
    */

    sc_stop();
  }

  SC_CTOR(test_alu) : uut("alu") {
    uut.a(a);
    uut.b(b);
    uut.op(op);
    uut.R(R);
    uut.zero(zero);
    uut.negative(negative);

    SC_THREAD(test);
  }
};

int sc_main(int argc, char **argv) {
  test_alu tb("tb");
  sc_start();

  return 0;
}
