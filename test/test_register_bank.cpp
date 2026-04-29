#include "../src/register_bank.cpp"
#include <systemc.h>

SC_MODULE(test_register_bank) {
  sc_signal<bool> clk, we;
  sc_signal<sc_uint<5>> rs1, rs2, rd;
  sc_signal<sc_int<32>> wd;
  sc_signal<sc_int<32>> rd1, rd2;

  register_bank uut{"register_bank"};

  void clock_gen() {
    while (true) {
      clk.write(false);
      wait(5, SC_NS);
      clk.write(true);
      wait(5, SC_NS);
    }
  }

  void test() {
    // Teste: Escrita normal no reg[1]
    we.write(true);
    rd.write(1);     
    wd.write(56);
    wait(12, SC_NS);

    // Teste: Escrita normal no reg[2]
    we.write(true);
    rd.write(2);
    wd.write(100);
    wait(10, SC_NS);

    // Teste: Tentativa de escrita com Write Enable desligado (deve falhar)
    we.write(false);
    rd.write(3);
    wd.write(777);
    wait(10, SC_NS);

    // --- Verificação de todas as leituras ---
    we.write(false);
    rs1.write(1);
    rs2.write(0);
    wait(10, SC_NS);
    std::cout << "reg[1] = " << rd1.read() << " (Esperado: 56)" << std::endl;
    std::cout << "reg[0] = " << rd2.read() << " (Esperado: 0)" << std::endl;

    rs1.write(2);
    rs2.write(3);
    wait(10, SC_NS);
    std::cout << "reg[2] = " << rd1.read() << " (Esperado: 100)" << std::endl;
    std::cout << "reg[3] = " << rd2.read() << " (Esperado: 0)" << std::endl;

    sc_stop();
  }

  SC_CTOR(test_register_bank) {
    uut.clk(clk);
    uut.we(we);
    uut.rs1(rs1);
    uut.rs2(rs2);
    uut.rd(rd);
    uut.wd(wd);
    uut.rd1(rd1);
    uut.rd2(rd2);

    SC_THREAD(clock_gen);
    SC_THREAD(test);
  }
};

int sc_main(int argc, char **argv) {
  test_register_bank tb("tb");
  sc_start();
  return 0;
}
