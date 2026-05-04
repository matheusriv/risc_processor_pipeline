#ifndef CONTROL_UNIT_CPP
#define CONTROL_UNIT_CPP

#include <systemc.h>
#include "alu.cpp"

enum OP2_SEL {
    RT,
    IMEDIATE,
};

enum FLAG_SEL {
    VCC,
    ZERO,
    NEG,
    EARTH,
};

enum MEM_TO_REG {
    ULA_RESULT,
    MEMORY
};

enum OP_TYPES {
    I_TYPE,
    R_TYPE,
    S_TYPE,
    J_TYPE
};

enum S_TYPE_OP {
    READ,
    WRITE
};

enum J_TYPE_OP {
    ABSOLUTE,
    IF_ZERO,
    IF_NEG
};

enum REG_SEL {
    READ2_REG,
    WRITE_REG
};

SC_MODULE(control_unit) {
  sc_in<sc_uint<32>> word;

  sc_out<bool> isJump, regWrite, op2Sel,
    dataRead, dataWrite, memToReg, regSel;
  sc_out<sc_uint<11>> opUla;
  sc_out<sc_uint<2>> flagSel;

  void process();

  SC_CTOR(control_unit);
};

void control_unit::process() {
  sc_uint<6> opcode = word.read().range(5, 0);
  sc_uint<2> type = opcode.range(5, 4);

  if(type == OP_TYPES::R_TYPE) { // Tipo R (operacoes logicas e aritmeticas)
    sc_uint<11> funct = word.read().range(31, 21);
    isJump.write(false);
    regWrite.write(true);
    op2Sel.write(OP2_SEL::RT);
    opUla.write(funct);
    dataRead.write(false);
    dataWrite.write(false);
    regSel.write(REG_SEL::WRITE_REG);
    memToReg.write(MEM_TO_REG::ULA_RESULT);
  } else if (type == OP_TYPES::S_TYPE) { // Tipo S (ler/escrever na memoria)
    sc_uint<2> spec = opcode.range(1, 0);
    isJump.write(false);
    op2Sel.write(OP2_SEL::IMEDIATE);
    opUla.write(ALUOP::ADD);

    if(spec == S_TYPE_OP::READ) {
        regWrite.write(true);
        dataRead.write(true);
        dataWrite.write(false);
        regSel.write(REG_SEL::READ2_REG);
        memToReg.write(MEM_TO_REG::MEMORY);
    } else {
        regWrite.write(false);
        dataRead.write(false);
        dataWrite.write(true);
        regSel.write(REG_SEL::WRITE_REG);
    }
  } else if (type == OP_TYPES::J_TYPE) { // Tipo J (desvios)
    sc_uint<2> spec = opcode.range(1, 0);
    isJump.write(true);
    dataWrite.write(false);
    regWrite.write(false);

    if(spec == J_TYPE_OP::ABSOLUTE) flagSel.write(FLAG_SEL::VCC);
    else if(spec == J_TYPE_OP::IF_ZERO) flagSel.write(FLAG_SEL::ZERO);
    else flagSel.write(FLAG_SEL::NEG);

  } else if (type == OP_TYPES::I_TYPE) { // instrucoes imediatas
    isJump.write(false);
    regWrite.write(true);
    op2Sel.write(OP2_SEL::IMEDIATE);
    dataRead.write(false);
    dataWrite.write(false);
    regSel.write(REG_SEL::READ2_REG);
    memToReg.write(MEM_TO_REG::ULA_RESULT);

    if (opcode == 1) opUla.write(ALUOP::ADD);      // addi
    else if (opcode == 2) opUla.write(ALUOP::AND); // andi
    else if (opcode == 3) opUla.write(ALUOP::OR);  // ori
    else if (opcode == 4) opUla.write(ALUOP::XOR); // xori
    else opUla.write(ALUOP::ADD);
  } else {
    isJump.write(false);
    regWrite.write(false);
    op2Sel.write(OP2_SEL::RT);
    opUla.write(ALUOP::ADD);
    dataRead.write(false);
    dataWrite.write(false);
    regSel.write(REG_SEL::WRITE_REG);
    memToReg.write(MEM_TO_REG::ULA_RESULT);
  }
}

control_unit::control_unit(sc_module_name name) : sc_module(name) {
  SC_METHOD(process);
  sensitive << word;
}

#endif // CONTROL_UNIT_CPP
