#include "../src/mux2.cpp"
#include <systemc.h>

SC_MODULE(test_mux2) {
  sc_signal<bool> sel;
  sc_signal<sc_uint<32>>  a, b;
  sc_signal<sc_uint<32>> out;

  mux2<sc_uint<32>> uut{"mux2"};

  void test() {
    a.write(10);
    b.write(20);

    for (int i = 0; i < 2; ++i) {
      sel.write(i);
      wait(1, SC_NS);
      std::cout << "sel = " << i << " -> out = " << out.read() << std::endl;
    }

    sc_stop();
  }

  SC_CTOR(test_mux2) {
    uut.sel(sel);
    uut.a(a);
    uut.b(b);
    uut.out(out);

    SC_THREAD(test);
  }
};

int sc_main(int argc, char **argv) {
  test_mux2 tb("tb");
  sc_start();

  return 0;
}
