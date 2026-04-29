#include "../src/signal_extend.cpp"
#include <systemc.h>

SC_MODULE(test_signal_extend) {
  sc_signal<sc_int<16>> d_in;
  sc_signal<sc_int<32>> d_out;

  signal_extend uut{"uut"};


  void test() {
    d_in.write(-10);
    wait(10, SC_NS);
    std::cout << "Signed extended value " << d_out.read() << std::endl;

    d_in.write(10);
    wait(10, SC_NS);
    std::cout << "Signed extended value "  << d_out.read() << std::endl;

    sc_stop();
  }

  SC_CTOR(test_signal_extend) : uut("uut") {
    uut.d_in(d_in);
    uut.d_out(d_out);
    SC_THREAD(test);
  }
};

int sc_main(int argc, char **argv) {
  test_signal_extend tb("tb");
  sc_start();
  return 0;
}
