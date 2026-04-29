#ifndef REGISTER_BANK_CPP
#define REGISTER_BANK_CPP

#include <systemc.h>

SC_MODULE(register_bank) {
  sc_in<bool> clk;
  sc_in<bool> we;

  sc_in<sc_uint<5>> rs1, rs2, rd;
  sc_in<sc_int<32>> wd;
  sc_out<sc_int<32>> rd1, rd2;

  sc_signal<sc_int<32>> regs[32];

  void write();
  void read();

  SC_CTOR(register_bank);
};

void register_bank::write() {
  while (true) {
    wait(clk.negedge_event());
    if (we.read() && rd.read() != 0){  
      regs[rd.read()].write(wd.read());
      std::cout << "[Banco de Registradores] Escrevendo valor " << wd.read() << " no reg[" << rd.read() << "]" << std::endl;
    }
  }
}

void register_bank::read() {
  rd1.write(regs[rs1.read()].read());
  rd2.write(regs[rs2.read()].read());
}

register_bank::register_bank(sc_module_name name) : sc_module(name) {
  SC_THREAD(write);
  SC_METHOD(read);
  sensitive << rs1 << rs2;
  for (int i = 0; i < 32; i++) {
    sensitive << regs[i];
  }
}

#endif // !REGISTER_BANK_CPP