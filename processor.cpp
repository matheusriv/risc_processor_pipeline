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
  // --- Sinais Globais e de Clock ---
  const int CLOCK_SIZE_NS = 100; // Constante para o período do clock em nanosegundos.
  sc_signal<bool> clk;           // Sinal de clock principal que sincroniza todo o processador.
  sc_signal<bool> vcc, earth;    // Sinais de alimentação ('1' lógico) e terra ('0' lógico) para entradas constantes.
  sc_signal<sc_int<32>> zero;    // Sinal constante com valor 0 de 32 bits.

  // --- Primeiro Estágio (IF - Busca de Instrução) ---
  reg<32> pc{"pc"};                                  // O registrador Contador de Programa (PC).
  instruction_memory mem_ins{"mem_ins"};             // Módulo da memória de instruções.
  adder inc{"inc"};                                  // Somador para calcular PC + 4.
  sc_signal<sc_uint<32>> four_constant;              // Sinal constante com o valor 4.
  sc_signal<sc_uint<32>> pc_curr_value;              // Saída do registrador PC, contendo o endereço da instrução atual.
  sc_signal<sc_uint<32>> palavra;                    // A instrução de 32 bits lida da memória de instruções.
  sc_signal<sc_uint<32>> inc_result_out;             // Resultado do somador (PC + 4).
  sc_signal<bool> reset_pc;                          // Sinal para resetar o PC.

  // --- Registradores de Pipeline IF/ID ---
  if_id bar_if_id{"bar_if_id"};                      // Módulo do registrador de pipeline entre os estágios IF e ID.
  sc_signal<sc_uint<32>> if_id_pc_saida;             // Saída do PC do registrador IF/ID (PC da instrução atual no estágio ID).
  sc_signal<sc_uint<32>> if_id_inst_saida;           // Saída da instrução do registrador IF/ID.
  sc_signal<sc_uint<5>> read1, read2, write1;        // Endereços dos registradores de leitura (rs, rt) e escrita (rd) decodificados da instrução.
  sc_signal<sc_int<16>> immediate;                   // Valor imediato de 16 bits extraído da instrução.
  sc_signal<sc_uint<26>> absolute;                   // Endereço absoluto de 26 bits para instruções de desvio (jump).

  // --- Segundo Estágio (ID - Decodificação e Busca de Registradores) ---
  register_bank b_reg{"b_reg"};                      // Módulo do banco de registradores.
  control_unit controle{"control"};                  // Módulo da unidade de controle principal.
  signal_extend sign_ext{"sign_ext"};                // Módulo para extensão de sinal do imediato.
  hazard_detection_unit detec_conflitos{"detec_conflitos"}; // Unidade de detecção de hazards de dados (conflitos).
  mux_control mux_sinais_controle{"mux_sinais_controle"};   // MUX para zerar os sinais de controle em caso de stall (bolha).
  mux2<sc_uint<5>> mux_reg_sel{"mux_reg_sel"};       // MUX para selecionar o registrador de destino (rd ou rt).
  sc_signal<sc_uint<5>> selected_read2;              // Saída do mux_reg_sel, o endereço do registrador de destino para a instrução atual.
  sc_signal<bool> pc_write, if_id_write;             // Sinais da unidade de hazard: pc_write para o PC, if_id_write para inserir uma bolha.
  sc_signal<bool> mux_controle_sel;                  // Sinal de seleção para o mux_sinais_controle, vindo da unidade de hazard.
  sc_signal<sc_int<32>> ext_immidiate;               // Valor imediato estendido para 32 bits.
  sc_signal<sc_int<32>> b_reg_result1, b_reg_result2; // Valores lidos do banco de registradores (dados de rs e rt).

  // --- Sinais de Saída da Unidade de Controle ---
  sc_signal<bool> is_jump;                           // '1' se a instrução for um desvio.
  sc_signal<bool> reg_write;                         // '1' para habilitar a escrita no banco de registradores (estágio WB).
  sc_signal<bool> op2_sel;                           // Seleciona o segundo operando da ULA (registrador ou imediato).
  sc_signal<bool> data_read;                         // '1' para ler da memória de dados.
  sc_signal<bool> data_write;                        // '1' para escrever na memória de dados.
  sc_signal<bool> mem_to_reg;                        // Seleciona se o dado para o registrador vem da ULA ou da memória.
  sc_signal<bool> reg_sel;                           // Seleciona o campo do registrador de destino (rd ou rt).
  sc_signal<sc_uint<11>> op_ula;                     // Código da operação para a ULA.
  sc_signal<sc_uint<2>> flag_sel;                    // Seleciona a condição para desvios condicionais (zero, negativo).

  // --- Sinais de Controle após o MUX de Hazard (podem ser zerados) ---
  sc_signal<bool> is_jump_out, reg_write_out, op2_sel_out,
    data_read_out, data_write_out, mem_to_reg_out, reg_sel_out;
  sc_signal<sc_uint<11>> op_ula_out;
  sc_signal<sc_uint<2>> flag_sel_out;

  // --- Registradores de Pipeline ID/EX ---
  id_ex bar_id_ex{"bar_id_ex"};                      // Módulo do registrador de pipeline entre ID e EX.
  // Sinais de saída do registrador ID/EX, passando informações para o estágio EX.
  sc_signal<bool> id_ex_is_jump_out, id_ex_reg_write_out, id_ex_op2_sel_out,
      id_ex_data_read_out, id_ex_data_write_out, id_ex_mem_to_reg_out;
  sc_signal<sc_uint<11>> id_ex_op_ula_out;
  sc_signal<sc_uint<2>> id_ex_flag_sel_out;
  sc_signal<sc_int<32>> id_ex_read1_out, id_ex_read2_out, id_ex_immediate_out; // Dados dos registradores e imediato.
  sc_signal<sc_uint<32>> id_ex_pc_out;               // Valor do PC.
  sc_signal<sc_uint<5>> id_ex_rd_out, id_ex_rt_out, id_ex_rs_out; // Endereços dos registradores.
  sc_signal<sc_uint<26>> id_ex_absolute_out;         // Endereço de desvio.

  // --- Terceiro Estágio (EX - Execução) ---
  alu ula_ex{"ula_ex"};                              // Módulo da Unidade Lógica e Aritmética (ULA).

  // MUXes para a Unidade de Adiantamento (Forwarding)
  mux4<sc_int<32>> ula_src1_mux{"ula_src1_mux"};     // MUX para o primeiro operando da ULA.
  sc_signal<sc_int<32>> ula_src1_mux_out;            // Saída do MUX do primeiro operando.

  mux4<sc_int<32>> ula_src2_mux{"ula_src2_mux"};     // MUX para o segundo operando da ULA.
  sc_signal<sc_int<32>> ula_src2_mux_out;            // Saída do MUX do segundo operando.

  mux2<sc_int<32>> op2_mux{"op2_mux"};               // MUX para selecionar entre o valor do registrador e o imediato como segundo operando.

  forwarding_unit ex_unid_adiantamento{"ex_unid_adiantamento"}; // Módulo da unidade de adiantamento.
  sc_signal<sc_uint<2>> forward_a, forward_b;        // Sinais de controle da unidade de adiantamento para os MUXes.

  sc_signal<sc_int<32>> op2_mux_out;                 // Saída do op2_mux, operando final para a ULA.
  sc_signal<sc_int<32>> ula_result_out;              // Resultado da operação da ULA.
  sc_signal<bool> ula_zero_out, ula_negative_out;    // Flags de status (zero, negativo) geradas pela ULA.

  // --- Registradores de Pipeline EX/MEM ---
  ex_mem bar_ex_mem{"bar_ex_mem"};                   // Módulo do registrador de pipeline entre EX e MEM.
  // Sinais de saída do registrador EX/MEM, passando informações para o estágio MEM.
  sc_signal<bool> ex_mem_is_jump_out, ex_mem_reg_write_out,
    ex_mem_data_read_out, ex_mem_data_write_out, ex_mem_mem_to_reg_out;
  sc_signal<sc_uint<2>> ex_mem_flag_sel_out;
  sc_signal<sc_uint<32>> ex_mem_pc_out;
  sc_signal<sc_int<32>> ex_mem_ula_result_out;       // Resultado da ULA.
  sc_signal<sc_int<32>> ex_mem_reg_data_out;         // Dado a ser escrito na memória (vindo de rt).
  sc_signal<bool> ex_mem_ula_zero_out, ex_mem_ula_negative_out; // Flags da ULA.
  sc_signal<sc_uint<26>> ex_mem_absolute_out;
  sc_signal<sc_uint<5>> ex_mem_rd_out;               // Endereço do registrador de destino.

  // --- Quarto Estágio (MEM - Acesso à Memória) ---
  data_memory mem_mem_dados{"mem_mem_dados"};        // Módulo da memória de dados.
  mux4<bool> mux_flag_sel{"mux_flag_sel"};           // MUX para selecionar a condição de desvio (sempre, se zero, se negativo).
  pc_end mem_pc_end{"mem_pc_end"};                   // Módulo para calcular o endereço de destino de um desvio absoluto.
  and_port jump_gate{"gate_gate"};                   // Porta AND para determinar se um desvio deve ocorrer (isJump E condição).
  mux2<sc_uint<32>> mux_pc_next_value{"mux_pc_next_value"}; // MUX que seleciona o próximo valor do PC (PC+4 ou endereço de desvio).

  sc_signal<sc_int<32>> mem_dados_result_out;        // Dado lido da memória de dados.
  sc_signal<sc_uint<32>> pc_end_result_out;          // Endereço de destino calculado para um desvio.
  sc_signal<bool> mux_flag_sel_out;                  // Saída do MUX de seleção de flag de desvio.
  sc_signal<bool> jump_gate_out;                     // Saída da porta AND, '1' se o desvio for tomado.
  sc_signal<sc_uint<32>> pc_next_value_out;          // Valor final a ser carregado no PC no próximo ciclo.
  sc_signal<bool> ex_mem_ula_not_zero_out;           // Flag Zero invertida (BNE).
  sc_signal<sc_uint<32>> actual_branch_target;       // O endereço ajustado (16 ou 26 bits) do desvio.
  sc_signal<bool> final_pc_write;                    // Sinal final que libera escrita no PC.

  // --- Quinto Estágio (WB - Write Back) e Registrador MEM/WB ---
  mem_wb bar_mem_wb{"bar_mem_wb"};                   // Módulo do registrador de pipeline entre MEM e WB.
  mux2<sc_int<32>> mux_mem_to_reg{"mux_mem_to_reg"}; // MUX que seleciona o dado a ser escrito de volta no banco de registradores.

  // Sinais de saída do registrador MEM/WB, passando informações para o estágio WB.
  sc_signal<bool> mem_wb_reg_write_out, mem_wb_mem_to_reg_out;
  sc_signal<sc_uint<5>> mem_wb_rd_out;               // Endereço do registrador de destino.
  sc_signal<sc_int<32>> mem_wb_ula_result_out;       // Resultado da ULA vindo do estágio anterior.
  sc_signal<sc_int<32>> mem_wb_mem_data_out;         // Dado lido da memória vindo do estágio anterior.
  sc_signal<sc_int<32>> mux_mem_to_reg_out;          // Saída do MUX, valor final a ser escrito no banco de registradores.

  void clock_gen() {
    while (true) {
      clk.write(false);
      wait(CLOCK_SIZE_NS/2, SC_NS);
      clk.write(true);
      wait(CLOCK_SIZE_NS/2, SC_NS);
    }
  }

  void calc_not_zero() {
      ex_mem_ula_not_zero_out.write(!ex_mem_ula_zero_out.read());
  }

  void calc_actual_target() {
      if (ex_mem_flag_sel_out.read() == 0) { // Se for j, flag_sel == VCC == 0
          actual_branch_target.write(ex_mem_absolute_out.read());
      } else { // Se for beq ou bne
          actual_branch_target.write(ex_mem_absolute_out.read() >> 10);
      }
  }

  // Impede que um falso conflito congele o PC quando um salto for tomado
  void calc_pc_write() {
      final_pc_write.write(pc_write.read() | jump_gate_out.read());
  }

  void printProgramCounter() {
      std::cout << "Sinais do PC:\n";
      std::cout << "\tPC.write_enable: " << final_pc_write.read() << std::endl;
      std::cout << "\tPC.reset: " << reset_pc.read() << std::endl;
      std::cout << "\tPC.data_in: " << std::dec << pc_next_value_out.read() << std::endl;
      std::cout << "\tPC.data_out: " << std::dec << pc_curr_value.read() << std::endl;
  }

  void printInstructionMemory() {
      std::cout << "Instruction Memory:\n";
      std::cout << "\tinstruction_memory.address: " << std::hex << "0x" << pc_curr_value.read() << std::endl;
      std::cout << "\tinstruction_memory.word: " << "0b" << std::bitset<32>{palavra.read()} << std::endl;
  }

  void printIfId() {
      std::cout << "IF/ID:\n";
      std::cout << "\tInputs:\n";
      std::cout << "\t\t.reset: " << jump_gate_out.read() << std::endl;
      std::cout << "\t\t.write: " << if_id_write.read() << std::endl;
      std::cout << "\t\t.pc: " << std::hex << "0x" << pc_curr_value.read() << std::endl;
      std::cout << "\t\t.instruction: " << "0b" << std::bitset<32>{palavra.read()} << std::endl;
      std::cout << "\tOutputs:\n";
      std::cout << "\t\t.pc: " << std::hex << "0x" << if_id_pc_saida.read() << std::endl;
      std::cout << "\t\t.instruction: " << "0b" << std::bitset<32>{if_id_inst_saida.read()} << std::endl;
      std::cout << "\t\t.read_addr_1: " << read1.read() << std::endl;
      std::cout << "\t\t.read_addr_2: " << read2.read() << std::endl;
      std::cout << "\t\t.write_addr: " << write1.read() << std::endl;
      std::cout << "\t\t.immediate: " << immediate.read() << std::endl;
      std::cout << "\t\t.absolute: " << absolute.read() << std::endl;
  }

  void printIdEx() {
      std::cout << "ID/EX:\n";
      std::cout << "\tInputs:\n";
      std::cout << "\t\t.reset: " << jump_gate_out.read() << std::endl;
      std::cout << "\t\t.is_jump: " << is_jump_out.read() << std::endl;
      std::cout << "\t\t.reg_write: " << reg_write_out.read() << std::endl;
      std::cout << "\t\t.op2_sel: " << op2_sel_out.read() << std::endl;
      std::cout << "\t\t.data_read: " << data_read_out.read() << std::endl;
      std::cout << "\t\t.data_write: " << data_write_out.read() << std::endl;
      std::cout << "\t\t.mem_to_reg: " << mem_to_reg_out.read() << std::endl;
      std::cout << "\t\t.op_ula: " << op_ula_out.read() << std::endl;
      std::cout << "\t\t.flag_sel: " << flag_sel_out.read() << std::endl;
      std::cout << "\t\t.read_data_1: " << b_reg_result1.read() << std::endl;
      std::cout << "\t\t.read_data_2: " << b_reg_result2.read() << std::endl;
      std::cout << "\t\t.immediate: " << ext_immidiate.read() << std::endl;
      std::cout << "\t\t.pc: " << if_id_pc_saida.read() << std::endl;
      std::cout << "\t\t.rs: " << read1.read() << std::endl;
      std::cout << "\t\t.rt: " << read2.read() << std::endl;
      std::cout << "\t\t.rd: " << write1.read() << std::endl;
      std::cout << "\t\t.absolute: "  << "0b" << (std::bitset<26>{absolute.read()}) << std::endl;
      std::cout << "\tOutputs:\n";
      std::cout << "\t\t.is_jump: " << id_ex_is_jump_out.read() << std::endl;
      std::cout << "\t\t.reg_write: " << id_ex_reg_write_out.read() << std::endl;
      std::cout << "\t\t.op2_sel: " << id_ex_op2_sel_out.read() << std::endl;
      std::cout << "\t\t.data_read: " << id_ex_data_read_out.read() << std::endl;
      std::cout << "\t\t.data_write: " << id_ex_data_write_out.read() << std::endl;
      std::cout << "\t\t.mem_to_reg: " << id_ex_mem_to_reg_out.read() << std::endl;
      std::cout << "\t\t.op_ula: " << id_ex_op_ula_out.read() << std::endl;
      std::cout << "\t\t.flag_sel: " << id_ex_flag_sel_out.read() << std::endl;
      std::cout << "\t\t.read_data_1: " << id_ex_read1_out.read() << std::endl;
      std::cout << "\t\t.read_data_2: " << id_ex_read2_out.read() << std::endl;
      std::cout << "\t\t.immediate: " << id_ex_immediate_out.read() << std::endl;
      std::cout << "\t\t.pc: " << id_ex_pc_out.read() << std::endl;
      std::cout << "\t\t.rd: " << id_ex_rd_out.read() << std::endl;
      std::cout << "\t\t.rt: " << id_ex_rt_out.read() << std::endl;
      std::cout << "\t\t.rs: " << id_ex_rs_out.read() << std::endl;
      std::cout << "\t\t.absolute: "  << "0b" << (std::bitset<26>{id_ex_absolute_out.read()}) << std::endl;
  }

  void printExMem() {
      std::cout << "EX/MEM:\n";
      std::cout << "\tInputs:\n";
      std::cout << "\t\t.is_jump: " << id_ex_is_jump_out.read() << std::endl;
      std::cout << "\t\t.reg_write: " << id_ex_reg_write_out.read() << std::endl;
      std::cout << "\t\t.data_read: " << id_ex_data_read_out.read() << std::endl;
      std::cout << "\t\t.data_write: " << id_ex_data_write_out.read() << std::endl;
      std::cout << "\t\t.mem_to_reg: " << id_ex_mem_to_reg_out.read() << std::endl;
      std::cout << "\t\t.flag_sel: "  << id_ex_flag_sel_out.read()  << std::endl;
      std::cout << "\t\t.ula_zero: "  << ula_zero_out.read()  << std::endl;
      std::cout << "\t\t.ula_negative: "  << ula_negative_out.read()  << std::endl;
      std::cout << "\t\t.ula_result: "  << ula_result_out.read()  << std::endl;
      std::cout << "\t\t.reg_data: "  << id_ex_read2_out.read()  << std::endl;
      std::cout << "\t\t.pc: "  << id_ex_pc_out.read()  << std::endl;
      std::cout << "\t\t.absolute: "  << id_ex_absolute_out.read()  << std::endl;
      std::cout << "\t\t.write_addr: "  << id_ex_rd_out.read()  << std::endl;
      std::cout << "\tOutputs:\n";
      std::cout << "\t\t.is_jump: " << ex_mem_is_jump_out.read() << std::endl;
      std::cout << "\t\t.reg_write: " << ex_mem_reg_write_out.read() << std::endl;
      std::cout << "\t\t.data_read: " << ex_mem_data_read_out.read() << std::endl;
      std::cout << "\t\t.data_write: " << ex_mem_data_write_out.read() << std::endl;
      std::cout << "\t\t.mem_to_reg: " << ex_mem_mem_to_reg_out.read() << std::endl;
      std::cout << "\t\t.flag_sel: "  << ex_mem_flag_sel_out.read()  << std::endl;
      std::cout << "\t\t.ula_zero: "  << ex_mem_ula_zero_out.read()  << std::endl;
      std::cout << "\t\t.ula_negative: "  << ex_mem_ula_negative_out.read()  << std::endl;
      std::cout << "\t\t.ula_result: "  << ex_mem_ula_result_out.read()  << std::endl;
      std::cout << "\t\t.reg_data: "  << ex_mem_reg_data_out.read()  << std::endl;
      std::cout << "\t\t.pc: "  << ex_mem_pc_out.read()  << std::endl;
      std::cout << "\t\t.absolute: "  << ex_mem_absolute_out.read()  << std::endl;
      std::cout << "\t\t.write_addr: "  << ex_mem_rd_out.read()  << std::endl;
  }

  void printMemWb() {
      std::cout << "MEM/WB:\n";
      std::cout << "\tInputs:\n";
      std::cout << "\t\t.reg_write: " << ex_mem_reg_write_out.read() << std::endl;
      std::cout << "\t\t.mem_to_reg: " << ex_mem_mem_to_reg_out.read() << std::endl;
      std::cout << "\t\t.ula_result: " << ex_mem_ula_result_out.read() << std::endl;
      std::cout << "\t\t.mem_data: " << mem_dados_result_out.read() << std::endl;
      std::cout << "\t\t.write_addr: "  << ex_mem_rd_out.read()  << std::endl;
      std::cout << "\tOutputs:\n";
      std::cout << "\t\t.reg_write: " << mem_wb_reg_write_out.read() << std::endl;
      std::cout << "\t\t.mem_to_reg: " << mem_wb_mem_to_reg_out.read() << std::endl;
      std::cout << "\t\t.ula_result: " << mem_wb_ula_result_out.read() << std::endl;
      std::cout << "\t\t.mem_data: " << mem_wb_mem_data_out.read() << std::endl;
      std::cout << "\t\t.write_addr: "  << mem_wb_rd_out.read()  << std::endl;
  }

  void printControlUnit() {
      std::cout << "Control Unit:\n";
      std::cout << "\tInputs:\n";
      std::cout << "\t\t.instruction: " << "0b" << std::bitset<32>{if_id_inst_saida.read()} << std::endl;
      std::cout << "\tOutputs:\n";
      std::cout << "\t\t.is_jump: " << is_jump.read() << std::endl;
      std::cout << "\t\t.reg_write: " << reg_write.read() << std::endl;
      std::cout << "\t\t.op2_sel: " << op2_sel.read() << std::endl;
      std::cout << "\t\t.data_read: " << data_read.read() << std::endl;
      std::cout << "\t\t.data_write: " << data_write.read() << std::endl;
      std::cout << "\t\t.mem_to_reg: "  << mem_to_reg.read()  << std::endl;
      std::cout << "\t\t.op_ula: "  << op_ula.read()  << std::endl;
      std::cout << "\t\t.flag_sel: "  << flag_sel.read()  << std::endl;
      std::cout << "\t\t.reg_sel: "  << reg_sel.read()  << std::endl;
  }

  void printForwardingUnit() {
      std::cout << "Forwarding Unit:\n";
      std::cout << "\tInputs:\n";
      std::cout << "\t\tID_EX.rs: " << id_ex_rs_out.read() << std::endl;
      std::cout << "\t\tID_EX.rt: " << id_ex_rt_out.read() << std::endl;
      std::cout << "\t\tEX_MEM.rd: " << ex_mem_rd_out.read() << std::endl;
      std::cout << "\t\tEX_MEM.reg_write: " << ex_mem_reg_write_out.read() << std::endl;
      std::cout << "\t\tMEM_WB.rd: " << mem_wb_rd_out.read() << std::endl;
      std::cout << "\t\tMEM_WB.reg_write: " << mem_wb_reg_write_out.read() << std::endl;
      std::cout << "\tOutputs:\n";
      std::cout << "\t\t.forward_a: " << forward_a.read() << std::endl;
      std::cout << "\t\t.forward_b: " << forward_b.read() << std::endl;
  }

  void printHazardDetection() {
      std::cout << "Hazard Detection Unit:\n";
      std::cout << "\tInputs:\n";
      std::cout << "\t\tIF_ID_rs: " << read1.read() << std::endl;
      std::cout << "\t\tIF_ID_rt: " << read2.read() << std::endl;
      std::cout << "\t\tID_EX_rt: " << id_ex_rt_out.read() << std::endl;
      std::cout << "\t\tID_EX.mem_read: " << id_ex_data_read_out.read() << std::endl;
      std::cout << "\tOutputs:\n";
      std::cout << "\t\t.pc_write: " << pc_write.read() << std::endl;
      std::cout << "\t\t.if_id_write: " << if_id_write.read() << std::endl;
      std::cout << "\t\t.control_mux_sel: " << mux_controle_sel.read() << std::endl;
  }

  void printRegisterBank() {
      std::cout << "Register Bank:\n";
      std::cout << "\tInputs:\n";
      std::cout << "\t\t.reg_write: " << mem_wb_reg_write_out.read() << std::endl;
      std::cout << "\t\t.read_addr_1: " << read1.read() << std::endl;
      std::cout << "\t\t.read_addr_2: " << selected_read2.read() << std::endl;
      std::cout << "\t\t.write_addr: " << mem_wb_rd_out.read() << std::endl;
      std::cout << "\t\t.write_data: " << mux_mem_to_reg_out.read() << std::endl;
      std::cout << "\tOutputs:\n";
      std::cout << "\t\t.read_data_1: " << b_reg_result1.read() << std::endl;
      std::cout << "\t\t.read_data_2: " << b_reg_result2.read() << std::endl;
  }
  
  void printInstructionMemoryState(int num_instructions) {
    std::cout << "\n--- Primeiras " << std::dec << num_instructions << " posições da Memória de Instruções ---" << std::endl;
    // Como cada instrução tem 4 bytes, iteramos num_instructions * 4 vezes
    for(int i = 0; i < num_instructions * 4; i += 4) {
      std::cout << "Endereço " << std::dec << i << ": 0b" 
                << std::bitset<8>(mem_ins.mem[i]) 
                << std::bitset<8>(mem_ins.mem[i+1]) 
                << std::bitset<8>(mem_ins.mem[i+2]) 
                << std::bitset<8>(mem_ins.mem[i+3]) << std::endl;
    }
  }

  void printDataMemoryState(int num_positions) {
    std::cout << "\n--- Primeiras " << std::dec << num_positions << " posições da Memória de Dados ---" << std::endl;
    for(int i = 0; i < num_positions; i++) {
      std::cout << "Endereço " << std::dec << i << ": " << mem_mem_dados.mem[i] << std::endl;
    }
  }

  void printRegisterBankState() {
    std::cout << "\n--- Estado do Banco de Registradores ---" << std::endl;
    for(int i = 0; i < 32; i++) {
      std::cout << "reg[" << std::dec << i << "]: " << b_reg.regs[i].read() << std::endl;
    }
  }

  // A cada ciclo de clock avança uma instrução
  void test() {
    printInstructionMemoryState(15);
    std::cout << "\n" << std::endl;

    bool auto_run = false;

    for(int i=0; i<80; i++) {
      wait(CLOCK_SIZE_NS, SC_NS);
      std::cout << "clock[" << std::dec << i << "]---------------------------------------- start" << std::endl;
      printProgramCounter();
      //printInstructionMemory();
      printIfId();
      //printIdEx();
      //printExMem();
      //printMemWb();
      printControlUnit();
      printHazardDetection();
      //printForwardingUnit();
      //printRegisterBank();

      if (!auto_run) {
        std::cout << "clock[" << std::dec << i << "]---------------------------------------- end. Press Enter to next cycle or 'c' to auto-run..." << std::endl;
        std::string input;
        std::getline(std::cin, input);
        if (input == "c" || input == "C") {
          auto_run = true; // Ativa a execução automática para os próximos ciclos
        }
      } else {
        std::cout << "clock[" << std::dec << i << "]---------------------------------------- end." << std::endl;
      }
    }

    printDataMemoryState(10);
    printRegisterBankState();

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
    four_constant.write(4);
    reset_pc.write(false);
    vcc.write(true);
    earth.write(false);
    zero.write(0);

    pc.clk(clk);
    pc.rst(reset_pc);
    pc.we(final_pc_write);
    pc.d_in(pc_next_value_out);
    pc.d_out(pc_curr_value);

    inc.a(pc.d_out);
    inc.b(four_constant);
    inc.d_out(inc_result_out);

    mem_ins.address(pc_curr_value);
    mem_ins.word(palavra);

    bar_if_id.clk(clk);
    bar_if_id.rst(jump_gate_out);
    bar_if_id.earth(earth);
    bar_if_id.vcc(vcc);
    bar_if_id.in_pc(pc_curr_value);
    bar_if_id.in_inst(palavra);
    bar_if_id.out_pc(if_id_pc_saida);
    bar_if_id.out_inst(if_id_inst_saida);
    bar_if_id.read1(read1);
    bar_if_id.read2(read2);
    bar_if_id.write1(write1);
    bar_if_id.immediate(immediate);
    bar_if_id.absolute(absolute);
    bar_if_id.if_id_write(if_id_write);

    mux_sinais_controle.sel(mux_controle_sel);
    mux_sinais_controle.isJump(is_jump);
    mux_sinais_controle.regWrite(reg_write);
    mux_sinais_controle.op2Sel(op2_sel);
    mux_sinais_controle.dataRead(data_read);
    mux_sinais_controle.dataWrite(data_write);
    mux_sinais_controle.memToReg(mem_to_reg);
    mux_sinais_controle.opUla(op_ula);
    mux_sinais_controle.flagSel(flag_sel);
    mux_sinais_controle.isJump_out(is_jump_out);
    mux_sinais_controle.regWrite_out(reg_write_out);
    mux_sinais_controle.op2Sel_out(op2_sel_out);
    mux_sinais_controle.dataRead_out(data_read_out);
    mux_sinais_controle.dataWrite_out(data_write_out);
    mux_sinais_controle.memToReg_out(mem_to_reg_out);
    mux_sinais_controle.opUla_out(op_ula_out);
    mux_sinais_controle.flagSel_out(flag_sel_out);

    detec_conflitos.PCWrite(pc_write);
    detec_conflitos.IF_ID_Write(if_id_write);
    detec_conflitos.ControlMux(mux_controle_sel);
    detec_conflitos.IF_ID_rs(read1);
    detec_conflitos.IF_ID_rt(read2);
    detec_conflitos.ID_EX_rt(id_ex_rd_out);
    detec_conflitos.ID_EX_MemRead(id_ex_data_read_out);

    mux_reg_sel.sel(reg_sel);
    mux_reg_sel.a(read2);
    mux_reg_sel.b(write1);
    mux_reg_sel.out(selected_read2);

    controle.word(if_id_inst_saida);
    controle.isJump(is_jump);
    controle.regWrite(reg_write);
    controle.op2Sel(op2_sel);
    controle.dataRead(data_read);
    controle.dataWrite(data_write);
    controle.memToReg(mem_to_reg);
    controle.opUla(op_ula);
    controle.flagSel(flag_sel);
    controle.regSel(reg_sel);

    sign_ext.d_in(immediate);
    sign_ext.d_out(ext_immidiate);

    b_reg.clk(clk);
    b_reg.we(mem_wb_reg_write_out);
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
    bar_id_ex.isJump(is_jump_out);
    bar_id_ex.regWrite(reg_write_out);
    bar_id_ex.op2Sel(op2_sel_out);
    bar_id_ex.dataRead(data_read_out);
    bar_id_ex.dataWrite(data_write_out);
    bar_id_ex.memToReg(mem_to_reg_out);
    bar_id_ex.opAlu(op_ula_out);
    bar_id_ex.flagSel(flag_sel_out);
    bar_id_ex.read1(b_reg_result1);
    bar_id_ex.read2(b_reg_result2);
    bar_id_ex.immediate(ext_immidiate);
    bar_id_ex.pc(if_id_pc_saida);
    bar_id_ex.rd(selected_read2);
    bar_id_ex.rt(read2);
    bar_id_ex.rs(read1);
    bar_id_ex.absolute(absolute);

    bar_id_ex.isJump_out(id_ex_is_jump_out);
    bar_id_ex.regWrite_out(id_ex_reg_write_out);
    bar_id_ex.op2Sel_out(id_ex_op2_sel_out);
    bar_id_ex.dataRead_out(id_ex_data_read_out);
    bar_id_ex.dataWrite_out(id_ex_data_write_out);
    bar_id_ex.memToReg_out(id_ex_mem_to_reg_out);
    bar_id_ex.opAlu_out(id_ex_op_ula_out);
    bar_id_ex.flagSel_out(id_ex_flag_sel_out);
    bar_id_ex.read1_out(id_ex_read1_out);
    bar_id_ex.read2_out(id_ex_read2_out);
    bar_id_ex.immediate_out(id_ex_immediate_out);
    bar_id_ex.pc_out(id_ex_pc_out);
    bar_id_ex.rd_out(id_ex_rd_out);
    bar_id_ex.rt_out(id_ex_rt_out);
    bar_id_ex.rs_out(id_ex_rs_out);
    bar_id_ex.absolute_out(id_ex_absolute_out);

    ula_src1_mux.sel(forward_a);

    ula_src1_mux.a(id_ex_read1_out);
    ula_src1_mux.b(mux_mem_to_reg_out);
    ula_src1_mux.c(ex_mem_ula_result_out);
    ula_src1_mux.d(zero);
    ula_src1_mux.out(ula_src1_mux_out);

    ula_src2_mux.sel(forward_b);
    ula_src2_mux.a(id_ex_read2_out);
    ula_src2_mux.b(mux_mem_to_reg_out);
    ula_src2_mux.c(ex_mem_ula_result_out);
    ula_src2_mux.d(zero);
    ula_src2_mux.out(ula_src2_mux_out);

    op2_mux.sel(id_ex_op2_sel_out);
    op2_mux.a(ula_src2_mux_out);
    op2_mux.b(id_ex_immediate_out);
    op2_mux.out(op2_mux_out);

    ula_ex.a(ula_src1_mux_out);
    ula_ex.b(op2_mux_out);
    ula_ex.op(id_ex_op_ula_out);
    ula_ex.R(ula_result_out);
    ula_ex.zero(ula_zero_out);
    ula_ex.negative(ula_negative_out);

    ex_unid_adiantamento.ID_EX_rs(id_ex_rs_out);
    ex_unid_adiantamento.ID_EX_rt(id_ex_rt_out);
    ex_unid_adiantamento.EX_MEM_rd(ex_mem_rd_out);
    ex_unid_adiantamento.EX_MEM_RegWrite(ex_mem_reg_write_out);
    ex_unid_adiantamento.MEM_WB_rd(mem_wb_rd_out);
    ex_unid_adiantamento.MEM_WB_RegWrite(mem_wb_reg_write_out);
    ex_unid_adiantamento.ForwardA(forward_a);
    ex_unid_adiantamento.ForwardB(forward_b);

    bar_ex_mem.clk(clk);
    bar_ex_mem.rst(jump_gate_out);
    bar_ex_mem.earth(earth);
    bar_ex_mem.vcc(vcc);
    bar_ex_mem.isJump(id_ex_is_jump_out);
    bar_ex_mem.regWrite(id_ex_reg_write_out);
    bar_ex_mem.dataRead(id_ex_data_read_out);
    bar_ex_mem.dataWrite(id_ex_data_write_out);
    bar_ex_mem.memToReg(id_ex_mem_to_reg_out);
    bar_ex_mem.flagSel(id_ex_flag_sel_out);
    bar_ex_mem.alu_zero(ula_zero_out);
    bar_ex_mem.alu_negative(ula_negative_out);
    bar_ex_mem.alu_result(ula_result_out);
    bar_ex_mem.reg_data(ula_src2_mux_out);
    bar_ex_mem.pc(id_ex_pc_out);
    bar_ex_mem.absolute(id_ex_absolute_out);
    bar_ex_mem.rd(id_ex_rd_out);

    bar_ex_mem.isJump_out(ex_mem_is_jump_out);
    bar_ex_mem.regWrite_out(ex_mem_reg_write_out);
    bar_ex_mem.dataRead_out(ex_mem_data_read_out);
    bar_ex_mem.dataWrite_out(ex_mem_data_write_out);
    bar_ex_mem.memToReg_out(ex_mem_mem_to_reg_out);
    bar_ex_mem.alu_zero_out(ex_mem_ula_zero_out);
    bar_ex_mem.alu_negative_out(ex_mem_ula_negative_out);
    bar_ex_mem.flagSel_out(ex_mem_flag_sel_out);
    bar_ex_mem.pc_out(ex_mem_pc_out);
    bar_ex_mem.alu_result_out(ex_mem_ula_result_out);
    bar_ex_mem.reg_data_out(ex_mem_reg_data_out);
    bar_ex_mem.absolute_out(ex_mem_absolute_out);
    bar_ex_mem.rd_out(ex_mem_rd_out);

    mem_mem_dados.clk(clk);
    mem_mem_dados.dataRead(ex_mem_data_read_out);
    mem_mem_dados.dataWrite(ex_mem_data_write_out);
    mem_mem_dados.address(ex_mem_ula_result_out);
    mem_mem_dados.data(ex_mem_reg_data_out);
    mem_mem_dados.resposta(mem_dados_result_out);

    mem_pc_end.pc(ex_mem_pc_out);
    mem_pc_end.absolut(ex_mem_absolute_out);
    mem_pc_end.resposta(pc_end_result_out);

    mux_flag_sel.sel(ex_mem_flag_sel_out);
    // ORDEM IMPORTA AQUI: consulte enum na parte de controle
    mux_flag_sel.a(vcc);
    mux_flag_sel.b(ex_mem_ula_zero_out);
    mux_flag_sel.c(ex_mem_ula_not_zero_out);
    mux_flag_sel.d(earth);
    mux_flag_sel.out(mux_flag_sel_out);

    jump_gate.a(ex_mem_is_jump_out);
    jump_gate.b(mux_flag_sel_out);
    jump_gate.out(jump_gate_out);

    mux_pc_next_value.sel(jump_gate_out);
    mux_pc_next_value.a(inc_result_out);
    mux_pc_next_value.b(actual_branch_target);
    mux_pc_next_value.out(pc_next_value_out);

    bar_mem_wb.clk(clk);
    bar_mem_wb.
    rst(earth);
    bar_mem_wb.earth(earth);
    bar_mem_wb.vcc(vcc);
    bar_mem_wb.regWrite(ex_mem_reg_write_out);
    bar_mem_wb.memToReg(ex_mem_mem_to_reg_out);
    bar_mem_wb.alu_result(ex_mem_ula_result_out);
    bar_mem_wb.mem_data(mem_dados_result_out);
    bar_mem_wb.rd(ex_mem_rd_out);
    bar_mem_wb.regWrite_out(mem_wb_reg_write_out);
    bar_mem_wb.memToReg_out(mem_wb_mem_to_reg_out);
    bar_mem_wb.alu_result_out(mem_wb_ula_result_out);
    bar_mem_wb.mem_data_out(mem_wb_mem_data_out);
    bar_mem_wb.rd_out(mem_wb_rd_out);

    mux_mem_to_reg.sel(mem_wb_mem_to_reg_out);
    // ORDEM IMPORTA AQUI: consulte enum na parte de controle
    mux_mem_to_reg.a(mem_wb_ula_result_out);
    mux_mem_to_reg.b(mem_wb_mem_data_out);
    mux_mem_to_reg.out(mux_mem_to_reg_out);


    SC_THREAD(clock_gen);
    SC_THREAD(test);

    SC_METHOD(calc_not_zero);
    sensitive << ex_mem_ula_zero_out;

    SC_METHOD(calc_actual_target);
    sensitive << ex_mem_flag_sel_out << pc_end_result_out << ex_mem_absolute_out << ex_mem_is_jump_out;

    SC_METHOD(calc_pc_write);
    sensitive << pc_write << jump_gate_out;
  }
};

int sc_main(int argc, char **argv) {
  test_cpu tb("tb");
  load(tb.mem_ins.mem, tb.mem_mem_dados.mem);
  sc_start();
  return 0;
}