#include "../src/register.cpp"
#include <systemc.h>

SC_MODULE(test_register) {
  sc_signal<bool> clk, rst, we;
  sc_signal<sc_uint<32>> d_in, d_out;

  reg<32> uut{"reg"};

  void clock_gen() {
    while (true) {
      clk.write(false);
      wait(5, SC_NS);
      clk.write(true);
      wait(5, SC_NS);
    }
  }

  void test() {
    we.write(true);
    
    for(int i=1; i<=10; i++) {
        d_in.write(i);
        if(i == 5) {
            rst.write(true);
        } else if (i == 6) {
            rst.write(false);
        }
        wait(10, SC_NS);
        cout << "Current input is: " << d_in.read() << endl;
        if(i == 5) {
            cout << "Store is reseted at " << i << endl;
        }
        cout << "Current output is: " << d_out.read() << endl;
    }    

    sc_stop();
  }

  SC_CTOR(test_register) : uut("reg") {
    uut.clk(clk);
    uut.rst(rst);
    uut.we(we);
    uut.d_in(d_in);
    uut.d_out(d_out);

    SC_THREAD(clock_gen);
    SC_THREAD(test);
  }
};

int sc_main(int argc, char **argv) {
  test_register tb("tb");
  sc_start();
  return 0;
}
