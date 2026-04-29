#include "../src/flip_flop_d.cpp"
#include <systemc.h>

SC_MODULE(test_flip_flop_d) {
  sc_signal<bool> clk, rst, a, b, we;

  flip_flop_d uut{"flip_flop_d"};

  void clock_gen() {
    while (true) {
      clk.write(false);
      wait(5, SC_NS);
      clk.write(true);
      wait(5, SC_NS);
    }
  }

  void test() {
    rst.write(true);
    a.write(false);
    we.write(true);
    wait(10, SC_NS);  // reset ativo
    rst.write(false);

    // Primeiro pulso de clock com a = 1
    a.write(true);
    wait(10, SC_NS);
    // Aqui b leu o valor anterior e a é igual a true
    std::cout << "[1] b = " << b.read() << std::endl;

    // Segundo pulso com a = 0
    a.write(false);
    wait(10, SC_NS);

    // a == false e b deve ser o valor anterior
    std::cout << "[2] b = " << b.read() << std::endl;

    a.write(true);
    rst.write(true);
    wait(10, SC_NS);

    // a == true mas b é o valor anterior
    std::cout << "[3] b = " << b.read() << std::endl;

    wait(10, SC_NS); // b deve ser false pois mesmo o valor anterior sendo true, reset estava ativo
    std::cout << "[4] b = " << b.read() << std::endl;

    sc_stop();
  }

  SC_CTOR(test_flip_flop_d) : uut("flip_flop_d") {
    uut.clk(clk);
    uut.rst(rst);
    uut.we(we);
    uut.a(a);
    uut.b(b);

    SC_THREAD(clock_gen);
    SC_THREAD(test);
  }
};

int sc_main(int argc, char** argv) {
  test_flip_flop_d tb("tb");
  sc_start();
  return 0;
}
