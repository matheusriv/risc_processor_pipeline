#include "../src/mux4.cpp"
#include <systemc.h>

SC_MODULE(test_mux4) {
  sc_signal<sc_uint<2>> sel;
  sc_signal<sc_uint<32>> a, b, c, d;
  sc_signal<sc_uint<32>> out;

  mux4<sc_uint<32>> uut{"mux4"};

  void test() {
    a.write(10);
    b.write(20);
    c.write(30);
    d.write(40);

    for (int i = 0; i < 4; ++i) {
      sel.write(i);
      wait(1, SC_NS);
      std::cout << "sel = " << i << " -> out = " << out.read() << std::endl;
    }

    sc_stop();
  }

  SC_CTOR(test_mux4) {
    uut.sel(sel);
    uut.a(a);
    uut.b(b);
    uut.c(c);
    uut.d(d);
    uut.out(out);

    SC_THREAD(test);
  }
};

int sc_main(int argc, char **argv) {
  test_mux4 tb("tb");
  sc_start();

  return 0;
}
