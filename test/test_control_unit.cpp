#include "../src/control_unit.cpp"
#include "../src/utils.cpp"
#include <systemc.h>

SC_MODULE(test_control_unit) {
  sc_signal<sc_uint<32>> word;

  sc_signal<bool> isJump, regWrite, op2Sel,
    dataRead, dataWrite, memToReg, regSel;
  sc_signal<sc_uint<11>> opUla;
  sc_signal<sc_uint<2>> flagSel;

  control_unit uut{"uut"};

  void print_signals(sc_uint<32> &signal_value) {
    word.write(signal_value);
    wait(10, SC_NS);

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "isJump: " << isJump.read() << std::endl;
    std::cout << "regWrite: " << regWrite.read() << std::endl;
    std::cout << "op2Sel: " << op2Sel.read() << std::endl;
    std::cout << "dataRead: " << dataRead.read() << std::endl;
    std::cout << "dataWrite: " << dataWrite.read() << std::endl;
    std::cout << "memToReg: " << memToReg.read() << std::endl;
    std::cout << "regSel: " << regSel.read() << std::endl;
    std::cout << "opUla: " << opUla.read() << std::endl;
    std::cout << "flagSel: " << flagSel.read() << std::endl;
    std::cout << "----------------------------------------" << std::endl;
  }

  void test() {
    sc_uint<32> r_type =       "00000000000_00001_00100_00010_010000"_u32;
    sc_uint<32> s_type_read =  "0000000000000000_00100_00010_100000"_u32;
    sc_uint<32> s_type_write = "0000000000000000_00100_00010_100001"_u32;
    sc_uint<32> jump =         "00000000000000000000000000_110000"_u32;
    sc_uint<32> i_type_addi =  "0000000000000000_00100_00010_000001"_u32;
    sc_uint<32> i_type_andi =  "0000000000000000_00100_00010_000010"_u32;

    // Esperado: isJump=0, regWrite=1, op2Sel=0 (Registrador), dataRead=0, dataWrite=0, memToReg=0 (Dado da ULA), regSel=1 (Usa rd)
    std::cout << "Teste 1: R-Type (ex: add, sub, and) - Opcode 010000" << std::endl;
    print_signals(r_type);

    // Esperado: isJump=0, regWrite=1, op2Sel=1 (Imediato para offset), dataRead=1, dataWrite=0, memToReg=1 (Dado da Memória), regSel=0 (Usa rt)
    std::cout << "Teste 2: S-Type Read / Load (ld) - Opcode 100000" << std::endl;
    print_signals(s_type_read);

    // Esperado: isJump=0, regWrite=0, op2Sel=1 (Imediato para offset), dataRead=0, dataWrite=1, memToReg=0 (Don't care)
    std::cout << "Teste 3: S-Type Write / Store (st) - Opcode 100001" << std::endl;
    print_signals(s_type_write);

    // Esperado: isJump=1, regWrite=0, dataWrite=0, flagSel=0 (Jump incondicional)
    std::cout << "Teste 4: Jump incondicional (j) - Opcode 110000" << std::endl;
    print_signals(jump);

    // Esperado: isJump=0, regWrite=1, op2Sel=1 (Usa imediato), dataRead=0, dataWrite=0, memToReg=0 (Dado da ULA), regSel=0 (Usa rt)
    std::cout << "Teste 5: I-Type Add Immediato (addi) - Opcode 000001" << std::endl;
    print_signals(i_type_addi);

    // Esperado: Semelhante ao addi (muda apenas os sinais do opUla interno)
    std::cout << "Teste 6: I-Type And Lógico Immediato (andi) - Opcode 000010" << std::endl;
    print_signals(i_type_andi);

    sc_stop();
  }

  SC_CTOR(test_control_unit) : uut("uut") {
    uut.word(word);
    uut.isJump(isJump);
    uut.regWrite(regWrite);
    uut.op2Sel(op2Sel);
    uut.dataRead(dataRead);
    uut.dataWrite(dataWrite);
    uut.memToReg(memToReg);
    uut.regSel(regSel);
    uut.opUla(opUla);
    uut.flagSel(flagSel);
    SC_THREAD(test);
  }
};

int sc_main(int argc, char **argv) {
  test_control_unit tb("tb");
  sc_start();
  return 0;
}
