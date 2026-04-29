#include "src/register.cpp"
#include "src/instruction_memory.cpp"
#include "src/adder.cpp"
#include "src/register_bank.cpp"
#include "src/signal_extend.cpp"
#include "src/control_unit.cpp"
#include "src/if_id.cpp"
#include "src/id_ex.cpp"
#include "src/alu.cpp"
#include "src/mux2.cpp"
#include "src/ex_mem.cpp"
#include "src/data_memory.cpp"
#include "src/mux4.cpp"
#include "src/pc_end.cpp"
#include "src/and.cpp"
#include "src/mem_wb.cpp"
#include "src/forwarding_unit.cpp"
#include "src/hazard_detection.cpp"
#include "src/mux_control.cpp"
#include "src/load_data.cpp"

#include <bitset>
#include <systemc.h>

SC_MODULE(test_cpu) {
  const int CLOCK_SIZE_NS = 100;
  sc_signal<bool> clk;
  sc_signal<bool> vcc, earth;
  sc_signal<sc_int<32>> zero;

  // Primeiro estágio
  reg<32> pc{"pc"};
  instruction_memory mem_ins{"mem_ins"};
  adder inc{"inc"};
  sc_signal<sc_uint<32>> fourConstant, pcCurrValue, palavra;
  sc_signal<sc_uint<32>> inc_result_out;
  sc_signal<bool> resetPc;

  // Registradores IF/ID
  if_id bar_if_id{"bar_if_id"};
    sc_signal<sc_uint<32>> ifid_pc_saida, ifid_inst_saida;
    sc_signal<sc_uint<5>> read1, read2, write1;
    sc_signal<sc_int<16>> immediate;
    sc_signal<sc_uint<26>> absolute;


  // Segundo estágio
  register_bank b_reg{"b_reg"};
  control_unit controle{"control"};
  signal_extend sign_ext{"sign_ext"};
  hazard_detection_unit detec_conflitos{"detec_conflitos"};
  mux_control mux_sinais_controle{"mux_sinais_controle"};
  mux2<sc_uint<5>> mux_reg_sel{"mux_reg_sel"};
  sc_signal<sc_uint<5>> selected_read2;
  sc_signal<bool> pcWrite, if_id_write, mux_controle_sel;
  sc_signal<sc_int<32>> ext_immidiate, b_reg_result1, b_reg_result2;

  // Sinais de saída da parte de contole
  sc_signal<bool> isJump, regWrite, op2Sel,
    dataRead, dataWrite, memToReg, regSel;
  sc_signal<sc_uint<11>> opUla;
  sc_signal<sc_uint<2>> flagSel;

  sc_signal<bool> isJump_out, regWrite_out, op2Sel_out,
    dataRead_out, dataWrite_out, memToReg_out, regSel_out;
  sc_signal<sc_uint<11>> opUla_out;
  sc_signal<sc_uint<2>> flagSel_out;

  id_ex bar_id_ex{"bar_id_ex"};

    sc_signal<bool> id_ex_isJump_out, id_ex_regWrite_out, id_ex_op2Sel_out,
      id_ex_dataRead_out, id_ex_dataWrite_out, id_ex_memToReg_out;
    sc_signal<sc_uint<11>> id_ex_opUla_out;
    sc_signal<sc_uint<2>> id_ex_flagSel_out;
    sc_signal<sc_int<32>> id_ex_read1_out, id_ex_read2_out, id_ex_immediate_out;
    sc_signal<sc_uint<32>> id_ex_pc_out;
    sc_signal<sc_uint<5>> id_ex_rd_out, id_ex_rt_out, id_ex_rs_out;
    sc_signal<sc_uint<26>> id_ex_absolute_out;

  // Terceiro estágio
  alu ula_ex{"ula_ex"};

  mux4<sc_int<32>> ula_src1_mux{"ula_src1_mux"};
  sc_signal<sc_int<32>> ula_src1_mux_out;

  mux4<sc_int<32>> ula_src2_mux{"ula_src2_mux"};
  sc_signal<sc_int<32>> ula_src2_mux_out;

  mux2<sc_int<32>> op2_mux{"op2_mux"};

  forwarding_unit ex_unid_adiantamento{"ex_unid_adiantamento"};
  sc_signal<sc_uint<2>> forwardA, forwardB;

  sc_signal<sc_int<32>> op2_mux_out;
  sc_signal<sc_int<32>> ula_result_out;
  sc_signal<bool> ula_zero_out, ula_negative_out;

  ex_mem bar_ex_mem{"bar_ex_mem"};
  sc_signal<bool> ex_mem_isJump_out, ex_mem_regWrite_out,
    ex_mem_dataRead_out, ex_mem_dataWrite_out, ex_mem_memToReg_out;
  sc_signal<sc_uint<2>> ex_mem_flagSel_out;
  sc_signal<sc_uint<32>> ex_mem_pc_out;
  sc_signal<sc_int<32>> ex_mem_ula_result_out, ex_mem_reg_data_out;
  sc_signal<bool> ex_mem_ula_zero_out, ex_mem_ula_negative_out;
  sc_signal<sc_uint<26>> ex_mem_absolute_out;
  sc_signal<sc_uint<5>> ex_mem_rd_out;

  // Quarto estágio
  data_memory mem_mem_dados{"mem_mem_dados"};
  mux4<bool> mux_flag_sel{"mux_flag_sel"};
  pc_end mem_pc_end{"mem_pc_end"};
  and_port jump_gate{"gate_gate"};
  mux2<sc_uint<32>> mux_pc_next_value{"mux_pc_next_value"};

  sc_signal<sc_int<32>> mem_dados_result_out;
  sc_signal<sc_uint<32>> pc_end_result_out;
  sc_signal<bool> mux_flag_sel_out;
  sc_signal<bool> jump_gate_out;
  sc_signal<sc_uint<32>> pc_next_value_out;

  // Quinto estágio
  mem_wb bar_mem_wb{"bar_mem_wb"};
  mux2<sc_int<32>> mux_mem_to_reg{"mux_mem_to_reg"};

  sc_signal<bool> mem_wb_regWrite_out, mem_wb_memToReg_out;
  sc_signal<sc_uint<5>> mem_wb_rd_out;
  sc_signal<sc_int<32>> mem_wb_ula_result_out, mem_wb_mem_data_out;
  sc_signal<sc_int<32>> mux_mem_to_reg_out;

  void clock_gen() {
    while (true) {
      clk.write(false);
      wait(CLOCK_SIZE_NS/2, SC_NS);
      clk.write(true);
      wait(CLOCK_SIZE_NS/2, SC_NS);
    }
  }

    void printContadorDePrograma() {
        std::cout << "Sinais do PC:\n";
        std::cout << "\tPC.we: " << pcWrite.read() << std::endl;
        std::cout << "\tPC.rst: " << resetPc.read() << std::endl;
        std::cout << "\tPC.d_in: " << std::dec << pc_next_value_out.read() << std::endl;
        std::cout << "\tPC.d_out: " << std::dec << pcCurrValue.read() << std::endl;
    }

    void printMemoriaDeInstrucoes() {
        std::cout << "Memoria de Instrucoes:\n";
        std::cout << "\tMEM_INS.endereco: " << std::hex << "0x" << pcCurrValue.read() << std::endl;
        std::cout << "\tMEM_INS.palavra: " << "0b" << std::bitset<32>{palavra.read()} << std::endl;
    }

    void printIfId() {
        std::cout << "IF/ID:\n";
        std::cout << "\tEntradas:\n";
        std::cout << "\t\t.rst: " << jump_gate_out.read() << std::endl;
        std::cout << "\t\t.write: " << if_id_write.read() << std::endl;
        std::cout << "\t\t.pc: " << std::hex << "0x" << pcCurrValue.read() << std::endl;
        std::cout << "\t\t.instrucao: " << "0b" << std::bitset<32>{palavra.read()} << std::endl;
        std::cout << "\tSaídas:\n";
        std::cout << "\t\t.pc: " << std::hex << "0x" << ifid_pc_saida.read() << std::endl;
        std::cout << "\t\t.instrucao: " << "0b" << std::bitset<32>{ifid_inst_saida.read()} << std::endl;
        std::cout << "\t\t.read1: " << read1.read() << std::endl;
        std::cout << "\t\t.read2: " << read2.read() << std::endl;
        std::cout << "\t\t.write1: " << write1.read() << std::endl;
        std::cout << "\t\t.immediate: " << immediate.read() << std::endl;
        std::cout << "\t\t.absolute: " << absolute.read() << std::endl;
    }

    void printIdEx() {
        std::cout << "ID/EX:\n";
        std::cout << "\tEntradas:\n";
        std::cout << "\t\t.rst: " << jump_gate_out.read() << std::endl;
        std::cout << "\t\t.isJump: " << isJump_out.read() << std::endl;
        std::cout << "\t\t.regWrite: " << regWrite_out.read() << std::endl;
        std::cout << "\t\t.op2Sel: " << op2Sel_out.read() << std::endl;
        std::cout << "\t\t.dataRead: " << dataRead_out.read() << std::endl;
        std::cout << "\t\t.dataWrite: " << dataWrite_out.read() << std::endl;
        std::cout << "\t\t.memToReg: " << memToReg_out.read() << std::endl;
        std::cout << "\t\t.opUla: " << opUla_out.read() << std::endl;
        std::cout << "\t\t.flagSel: " << flagSel_out.read() << std::endl;
        std::cout << "\t\t.read1: " << b_reg_result1.read() << std::endl;
        std::cout << "\t\t.read2: " << b_reg_result2.read() << std::endl;
        std::cout << "\t\t.immediate: " << ext_immidiate.read() << std::endl;
        std::cout << "\t\t.pc: " << ifid_pc_saida.read() << std::endl;
        std::cout << "\t\t.rd: " << write1.read() << std::endl;
        std::cout << "\t\t.rt: " << read1.read() << std::endl;
        std::cout << "\t\t.rs: " << read2.read() << std::endl;
        std::cout << "\t\t.absolute: "  << "0b" << (std::bitset<26>{absolute.read()}) << std::endl;
        std::cout << "\tSaídas:\n";
        std::cout << "\t\t.isJump: " << id_ex_isJump_out.read() << std::endl;
        std::cout << "\t\t.regWrite: " << id_ex_regWrite_out.read() << std::endl;
        std::cout << "\t\t.op2Sel: " << id_ex_op2Sel_out.read() << std::endl;
        std::cout << "\t\t.dataRead: " << id_ex_dataRead_out.read() << std::endl;
        std::cout << "\t\t.dataWrite: " << id_ex_dataWrite_out.read() << std::endl;
        std::cout << "\t\t.memToReg: " << id_ex_memToReg_out.read() << std::endl;
        std::cout << "\t\t.opUla: " << id_ex_opUla_out.read() << std::endl;
        std::cout << "\t\t.flagSel: " << id_ex_flagSel_out.read() << std::endl;
        std::cout << "\t\t.read1: " << id_ex_read1_out.read() << std::endl;
        std::cout << "\t\t.read2: " << id_ex_read2_out.read() << std::endl;
        std::cout << "\t\t.immediate: " << id_ex_immediate_out.read() << std::endl;
        std::cout << "\t\t.pc: " << id_ex_pc_out.read() << std::endl;
        std::cout << "\t\t.rd: " << id_ex_rd_out.read() << std::endl;
        std::cout << "\t\t.rt: " << id_ex_rt_out.read() << std::endl;
        std::cout << "\t\t.rs: " << id_ex_rs_out.read() << std::endl;
        std::cout << "\t\t.absolute: "  << "0b" << (std::bitset<26>{id_ex_absolute_out.read()}) << std::endl;
    }

    void printExMem() {
        std::cout << "EX/MEM:\n";
        std::cout << "\tEntradas:\n";
        std::cout << "\t\t.isJump: " << id_ex_isJump_out.read() << std::endl;
        std::cout << "\t\t.regWrite: " << id_ex_regWrite_out.read() << std::endl;
        std::cout << "\t\t.dataRead: " << id_ex_dataRead_out.read() << std::endl;
        std::cout << "\t\t.dataWrite: " << id_ex_dataWrite_out.read() << std::endl;
        std::cout << "\t\t.memToReg: " << id_ex_memToReg_out.read() << std::endl;
        std::cout << "\t\t.flagSel: "  << id_ex_flagSel_out.read()  << std::endl;
        std::cout << "\t\t.ula_zero: "  << ula_zero_out.read()  << std::endl;
        std::cout << "\t\t.ula_negative: "  << ula_negative_out.read()  << std::endl;
        std::cout << "\t\t.ula_result: "  << ula_result_out.read()  << std::endl;
        std::cout << "\t\t.reg_data: "  << id_ex_read2_out.read()  << std::endl;
        std::cout << "\t\t.pc: "  << id_ex_pc_out.read()  << std::endl;
        std::cout << "\t\t.absolute: "  << id_ex_absolute_out.read()  << std::endl;
        std::cout << "\t\t.rd: "  << id_ex_rd_out.read()  << std::endl;
        std::cout << "\tSaídas:\n";
        std::cout << "\t\t.isJump: " << ex_mem_isJump_out.read() << std::endl;
        std::cout << "\t\t.regWrite: " << ex_mem_regWrite_out.read() << std::endl;
        std::cout << "\t\t.dataRead: " << ex_mem_dataRead_out.read() << std::endl;
        std::cout << "\t\t.dataWrite: " << ex_mem_dataWrite_out.read() << std::endl;
        std::cout << "\t\t.memToReg: " << ex_mem_memToReg_out.read() << std::endl;
        std::cout << "\t\t.flagSel: "  << ex_mem_flagSel_out.read()  << std::endl;
        std::cout << "\t\t.ula_zero: "  << ex_mem_ula_zero_out.read()  << std::endl;
        std::cout << "\t\t.ula_negative: "  << ex_mem_ula_negative_out.read()  << std::endl;
        std::cout << "\t\t.ula_result: "  << ex_mem_ula_result_out.read()  << std::endl;
        std::cout << "\t\t.reg_data: "  << ex_mem_reg_data_out.read()  << std::endl;
        std::cout << "\t\t.pc: "  << ex_mem_pc_out.read()  << std::endl;
        std::cout << "\t\t.absolute: "  << ex_mem_absolute_out.read()  << std::endl;
        std::cout << "\t\t.rd: "  << ex_mem_rd_out.read()  << std::endl;
    }

    void printMemWb() {
        std::cout << "MEM/WB:\n";
        std::cout << "\tEntradas:\n";
        std::cout << "\t\t.regWrite: " << ex_mem_regWrite_out.read() << std::endl;
        std::cout << "\t\t.memToReg: " << ex_mem_memToReg_out.read() << std::endl;
        std::cout << "\t\t.ula_result: " << ex_mem_ula_result_out.read() << std::endl;
        std::cout << "\t\t.mem_data: " << mem_dados_result_out.read() << std::endl;
        std::cout << "\t\t.rd: "  << ex_mem_rd_out.read()  << std::endl;
        std::cout << "\tSaídas:\n";
        std::cout << "\t\t.regWrite: " << mem_wb_regWrite_out.read() << std::endl;
        std::cout << "\t\t.memToReg: " << mem_wb_memToReg_out.read() << std::endl;
        std::cout << "\t\t.ula_result: " << mem_wb_ula_result_out.read() << std::endl;
        std::cout << "\t\t.mem_data: " << mem_wb_mem_data_out.read() << std::endl;
        std::cout << "\t\t.rd: "  << mem_wb_rd_out.read()  << std::endl;
    }

    void printParteControle() {
        std::cout << "Parte Controle:\n";
        std::cout << "\tEntradas:\n";
        std::cout << "\t\t.instrucao: " << "0b" << std::bitset<32>{ifid_inst_saida.read()} << std::endl;
        std::cout << "\tSaídas:\n";
        std::cout << "\t\t.isJump: " << isJump.read() << std::endl;
        std::cout << "\t\t.regWrite: " << regWrite.read() << std::endl;
        std::cout << "\t\t.op2Sel: " << op2Sel.read() << std::endl;
        std::cout << "\t\t.dataRead: " << dataRead.read() << std::endl;
        std::cout << "\t\t.dataWrite: " << dataWrite.read() << std::endl;
        std::cout << "\t\t.memToReg: "  << memToReg.read()  << std::endl;
        std::cout << "\t\t.opUla: "  << opUla.read()  << std::endl;
        std::cout << "\t\t.flagSel: "  << flagSel.read()  << std::endl;
        std::cout << "\t\t.regSel: "  << regSel.read()  << std::endl;
    }

    void printUnidAdiantamento() {
        std::cout << "Unidade de Adiantamento:\n";
        std::cout << "\tEntradas\n";
        std::cout << "\t\tID_EX.rs: " << id_ex_rs_out.read() << std::endl;
        std::cout << "\t\tID_EX.rt: " << id_ex_rt_out.read() << std::endl;
        std::cout << "\t\tEX_MEM.rd: " << ex_mem_rd_out.read() << std::endl;
        std::cout << "\t\tEX_MEM.RegWrite: " << ex_mem_regWrite_out.read() << std::endl;
        std::cout << "\t\tMEM_WB.rd: " << mem_wb_rd_out.read() << std::endl;
        std::cout << "\t\tMEM_WB.RegWrite: " << mem_wb_regWrite_out.read() << std::endl;
        std::cout << "\tSaídas\n";
        std::cout << "\t\tForwardA: " << forwardA.read() << std::endl;
        std::cout << "\t\tForwardB: " << forwardB.read() << std::endl;
    }

    void printUnidDetecConflito() {
        std::cout << "Unidade de Detecção de Conflitos:\n";
        std::cout << "\tEntradas\n";
        std::cout << "\t\tIF_ID_rs: " << read1.read() << std::endl;
        std::cout << "\t\tIF_ID_rt: " << read2.read() << std::endl;
        std::cout << "\t\tID_EX_rt: " << id_ex_rt_out.read() << std::endl;
        std::cout << "\t\tID_EX_MemRead: " << id_ex_dataRead_out.read() << std::endl;
        std::cout << "\tSaídas\n";
        std::cout << "\t\tPCWrite: " << pcWrite.read() << std::endl;
        std::cout << "\t\tIF_ID_Write: " << if_id_write.read() << std::endl;
        std::cout << "\t\tControlMux: " << mux_controle_sel.read() << std::endl;
    }

    void printBancoReg() {
        std::cout << "Banco de Registradores:\n";
        std::cout << "\tEntradas:\n";
        std::cout << "\t\t.regWrite: " << mem_wb_regWrite_out.read() << std::endl;
        std::cout << "\t\t.rs1: " << read1.read() << std::endl;
        std::cout << "\t\t.rs2: " << selected_read2.read() << std::endl;
        std::cout << "\t\t.rd: " << mem_wb_rd_out.read() << std::endl;
        std::cout << "\t\t.wd: " << mux_mem_to_reg_out.read() << std::endl;
        std::cout << "\tSaídas:\n";
        std::cout << "\t\t.rd1: " << b_reg_result1.read() << std::endl;
        std::cout << "\t\t.rd2: " << b_reg_result2.read() << std::endl;
    }

  // A cada ciclo de clock avança uma instrução
  void test() {
    for(int i=0; i<15; i++) {
        wait(CLOCK_SIZE_NS, SC_NS);
        std::cout << "srt[" << std::dec << i << "]----------------------------------------" << std::endl;
        //printContadorDePrograma();
        //printMemoriaDeInstrucoes();
        //printIfId();
        //printIdEx();
        //printExMem();
        //printMemWb();
        //printParteControle();
        //printUnidAdiantamento();
        //printUnidDetecConflito();
        //printBancoReg();
        std::cout << "end[" << std::dec << i << "]---------------------------------------- Press enter to continue..." << std::endl;
        cin.get();
    }


    sc_stop();
  }

  SC_CTOR(test_cpu) :
    pc("pc"),
    mem_ins("mem_ins"),
    inc("inc"),
    bar_if_id("bar_if_id"),
    ula_ex("ula_ex"),
    op2_mux("op2_mux"),
    b_reg("b_reg"),
    controle("control"),
    sign_ext("sign_ext"),
    bar_id_ex("bar_id_ex")
  {
    fourConstant.write(4);
    resetPc.write(false);
    vcc.write(true);
    earth.write(false);
    zero.write(0);

    pc.clk(clk);
    pc.rst(resetPc);
    pc.we(pcWrite);
    pc.d_in(pc_next_value_out);
    pc.d_out(pcCurrValue);

    inc.a(pc.d_out);
    inc.b(fourConstant);
    inc.d_out(inc_result_out);

    mem_ins.address(pcCurrValue);
    mem_ins.word(palavra);

    bar_if_id.clk(clk);
    bar_if_id.rst(jump_gate_out);
    bar_if_id.earth(earth);
    bar_if_id.vcc(vcc);
    bar_if_id.in_pc(pcCurrValue);
    bar_if_id.in_inst(palavra);
    bar_if_id.out_pc(ifid_pc_saida);
    bar_if_id.out_inst(ifid_inst_saida);
    bar_if_id.read1(read1);
    bar_if_id.read2(read2);
    bar_if_id.write1(write1);
    bar_if_id.immediate(immediate);
    bar_if_id.absolute(absolute);
    bar_if_id.if_id_write(if_id_write);

    mux_sinais_controle.sel(mux_controle_sel);
    mux_sinais_controle.isJump(isJump);
    mux_sinais_controle.regWrite(regWrite);
    mux_sinais_controle.op2Sel(op2Sel);
    mux_sinais_controle.dataRead(dataRead);
    mux_sinais_controle.dataWrite(dataWrite);
    mux_sinais_controle.memToReg(memToReg);
    mux_sinais_controle.opUla(opUla);
    mux_sinais_controle.flagSel(flagSel);
    mux_sinais_controle.isJump_out(isJump_out);
    mux_sinais_controle.regWrite_out(regWrite_out);
    mux_sinais_controle.op2Sel_out(op2Sel_out);
    mux_sinais_controle.dataRead_out(dataRead_out);
    mux_sinais_controle.dataWrite_out(dataWrite_out);
    mux_sinais_controle.memToReg_out(memToReg_out);
    mux_sinais_controle.opUla_out(opUla_out);
    mux_sinais_controle.flagSel_out(flagSel_out);

    detec_conflitos.PCWrite(pcWrite);
    detec_conflitos.IF_ID_Write(if_id_write);
    detec_conflitos.ControlMux(mux_controle_sel);
    detec_conflitos.IF_ID_rs(read1);
    detec_conflitos.IF_ID_rt(read2);
    detec_conflitos.ID_EX_rt(id_ex_rd_out);
    detec_conflitos.ID_EX_MemRead(id_ex_dataRead_out);

    mux_reg_sel.sel(regSel);
    mux_reg_sel.a(read2);
    mux_reg_sel.b(write1);
    mux_reg_sel.out(selected_read2);

    controle.word(ifid_inst_saida);
    controle.isJump(isJump);
    controle.regWrite(regWrite);
    controle.op2Sel(op2Sel);
    controle.dataRead(dataRead);
    controle.dataWrite(dataWrite);
    controle.memToReg(memToReg);
    controle.opUla(opUla);
    controle.flagSel(flagSel);
    controle.regSel(regSel);

    sign_ext.d_in(immediate);
    sign_ext.d_out(ext_immidiate);

    b_reg.clk(clk);
    b_reg.we(mem_wb_regWrite_out);
    b_reg.rs1(read1);
    b_reg.rs2(read2);
    b_reg.rd(mem_wb_rd_out);
    b_reg.wd(mux_mem_to_reg_out);
    b_reg.rd1(b_reg_result1);
    b_reg.rd2(b_reg_result2);

    bar_id_ex.clk(clk);
    bar_id_ex.rst(jump_gate_out);
    bar_id_ex.earth(earth);
    bar_id_ex.vcc(vcc);
    bar_id_ex.isJump(isJump_out);
    bar_id_ex.regWrite(regWrite_out);
    bar_id_ex.op2Sel(op2Sel_out);
    bar_id_ex.dataRead(dataRead_out);
    bar_id_ex.dataWrite(dataWrite_out);
    bar_id_ex.memToReg(memToReg_out);
    bar_id_ex.opAlu(opUla_out);
    bar_id_ex.flagSel(flagSel_out);
    bar_id_ex.read1(b_reg_result1);
    bar_id_ex.read2(b_reg_result2);
    bar_id_ex.immediate(ext_immidiate);
    bar_id_ex.pc(ifid_pc_saida);
    bar_id_ex.rd(selected_read2);
    bar_id_ex.rt(read2);
    bar_id_ex.rs(read1);
    bar_id_ex.absolute(absolute);

    bar_id_ex.isJump_out(id_ex_isJump_out);
    bar_id_ex.regWrite_out(id_ex_regWrite_out);
    bar_id_ex.op2Sel_out(id_ex_op2Sel_out);
    bar_id_ex.dataRead_out(id_ex_dataRead_out);
    bar_id_ex.dataWrite_out(id_ex_dataWrite_out);
    bar_id_ex.memToReg_out(id_ex_memToReg_out);
    bar_id_ex.opAlu_out(id_ex_opUla_out);
    bar_id_ex.flagSel_out(id_ex_flagSel_out);
    bar_id_ex.read1_out(id_ex_read1_out);
    bar_id_ex.read2_out(id_ex_read2_out);
    bar_id_ex.immediate_out(id_ex_immediate_out);
    bar_id_ex.pc_out(id_ex_pc_out);
    bar_id_ex.rd_out(id_ex_rd_out);
    bar_id_ex.rt_out(id_ex_rt_out);
    bar_id_ex.rs_out(id_ex_rs_out);
    bar_id_ex.absolute_out(id_ex_absolute_out);

    ula_src1_mux.sel(forwardA);

    ula_src1_mux.a(id_ex_read1_out);
    ula_src1_mux.b(mux_mem_to_reg_out);
    ula_src1_mux.c(ex_mem_ula_result_out);
    ula_src1_mux.d(zero);
    ula_src1_mux.out(ula_src1_mux_out);

    ula_src2_mux.sel(forwardB);
    ula_src2_mux.a(id_ex_read2_out);
    ula_src2_mux.b(mux_mem_to_reg_out);
    ula_src2_mux.c(ex_mem_ula_result_out);
    ula_src2_mux.d(zero);
    ula_src2_mux.out(ula_src2_mux_out);

    op2_mux.sel(id_ex_op2Sel_out);
    op2_mux.a(ula_src2_mux_out);
    op2_mux.b(id_ex_immediate_out);
    op2_mux.out(op2_mux_out);

    ula_ex.a(ula_src1_mux_out);
    ula_ex.b(op2_mux_out);
    ula_ex.op(id_ex_opUla_out);
    ula_ex.R(ula_result_out);
    ula_ex.zero(ula_zero_out);
    ula_ex.negative(ula_negative_out);

    ex_unid_adiantamento.ID_EX_rs(id_ex_rs_out);
    ex_unid_adiantamento.ID_EX_rt(id_ex_rt_out);
    ex_unid_adiantamento.EX_MEM_rd(ex_mem_rd_out);
    ex_unid_adiantamento.EX_MEM_RegWrite(ex_mem_regWrite_out);
    ex_unid_adiantamento.MEM_WB_rd(mem_wb_rd_out);
    ex_unid_adiantamento.MEM_WB_RegWrite(mem_wb_regWrite_out);
    ex_unid_adiantamento.ForwardA(forwardA);
    ex_unid_adiantamento.ForwardB(forwardB);

    bar_ex_mem.clk(clk);
    bar_ex_mem.earth(earth);
    bar_ex_mem.vcc(vcc);
    bar_ex_mem.isJump(id_ex_isJump_out);
    bar_ex_mem.regWrite(id_ex_regWrite_out);
    bar_ex_mem.dataRead(id_ex_dataRead_out);
    bar_ex_mem.dataWrite(id_ex_dataWrite_out);
    bar_ex_mem.memToReg(id_ex_memToReg_out);
    bar_ex_mem.flagSel(id_ex_flagSel_out);
    bar_ex_mem.alu_zero(ula_zero_out);
    bar_ex_mem.alu_negative(ula_negative_out);
    bar_ex_mem.alu_result(ula_result_out);
    bar_ex_mem.reg_data(ula_src2_mux_out);
    bar_ex_mem.pc(id_ex_pc_out);
    bar_ex_mem.absolute(id_ex_absolute_out);
    bar_ex_mem.rd(id_ex_rd_out);

    bar_ex_mem.isJump_out(ex_mem_isJump_out);
    bar_ex_mem.regWrite_out(ex_mem_regWrite_out);
    bar_ex_mem.dataRead_out(ex_mem_dataRead_out);
    bar_ex_mem.dataWrite_out(ex_mem_dataWrite_out);
    bar_ex_mem.memToReg_out(ex_mem_memToReg_out);
    bar_ex_mem.alu_zero_out(ex_mem_ula_zero_out);
    bar_ex_mem.alu_negative_out(ex_mem_ula_negative_out);
    bar_ex_mem.flagSel_out(ex_mem_flagSel_out);
    bar_ex_mem.pc_out(ex_mem_pc_out);
    bar_ex_mem.alu_result_out(ex_mem_ula_result_out);
    bar_ex_mem.reg_data_out(ex_mem_reg_data_out);
    bar_ex_mem.absolute_out(ex_mem_absolute_out);
    bar_ex_mem.rd_out(ex_mem_rd_out);

    mem_mem_dados.clk(clk);
    mem_mem_dados.dataRead(ex_mem_dataRead_out);
    mem_mem_dados.dataWrite(ex_mem_dataWrite_out);
    mem_mem_dados.address(ex_mem_ula_result_out);
    mem_mem_dados.data(ex_mem_reg_data_out);
    mem_mem_dados.resposta(mem_dados_result_out);

    mem_pc_end.pc(ex_mem_pc_out);
    mem_pc_end.absolut(ex_mem_absolute_out);
    mem_pc_end.resposta(pc_end_result_out);

    mux_flag_sel.sel(ex_mem_flagSel_out);
    // ORDEM IMPORTA AQUI: consulte enum na parte de controle
    mux_flag_sel.a(vcc);
    mux_flag_sel.b(ex_mem_ula_zero_out);
    mux_flag_sel.c(ex_mem_ula_negative_out);
    mux_flag_sel.d(earth);
    mux_flag_sel.out(mux_flag_sel_out);

    jump_gate.a(ex_mem_isJump_out);
    jump_gate.b(mux_flag_sel_out);
    jump_gate.out(jump_gate_out);

    mux_pc_next_value.sel(jump_gate_out);
    mux_pc_next_value.a(inc_result_out);
    mux_pc_next_value.b(pc_end_result_out);
    mux_pc_next_value.out(pc_next_value_out);

    bar_mem_wb.clk(clk);
    bar_mem_wb.rst(earth);
    bar_mem_wb.earth(earth);
    bar_mem_wb.vcc(vcc);
    bar_mem_wb.regWrite(ex_mem_regWrite_out);
    bar_mem_wb.memToReg(ex_mem_memToReg_out);
    bar_mem_wb.alu_result(ex_mem_ula_result_out);
    bar_mem_wb.mem_data(mem_dados_result_out);
    bar_mem_wb.rd(ex_mem_rd_out);
    bar_mem_wb.regWrite_out(mem_wb_regWrite_out);
    bar_mem_wb.memToReg_out(mem_wb_memToReg_out);
    bar_mem_wb.alu_result_out(mem_wb_ula_result_out);
    bar_mem_wb.mem_data_out(mem_wb_mem_data_out);
    bar_mem_wb.rd_out(mem_wb_rd_out);

    mux_mem_to_reg.sel(mem_wb_memToReg_out);
    // ORDEM IMPORTA AQUI: consulte enum na parte de controle
    mux_mem_to_reg.a(mem_wb_ula_result_out);
    mux_mem_to_reg.b(mem_wb_mem_data_out);
    mux_mem_to_reg.out(mux_mem_to_reg_out);


    SC_THREAD(clock_gen);
    SC_THREAD(test);
  }
};

int sc_main(int argc, char **argv) {

  test_cpu tb("tb");
  load(tb.mem_ins.mem, tb.mem_mem_dados.mem);
  sc_start();
  return 0;
}