#include "../src/id_ex.cpp"
#include <bitset>

SC_MODULE(test_id_ex) {
    const int CLOCK_SIZE_NS = 10;
    sc_signal<bool> clk;
    sc_signal<bool> vcc, earth;
    
    id_ex bar_id_ex;

    sc_signal<bool> id_ex_isJump, id_ex_regWrite, id_ex_op2Sel,
        id_ex_dataRead, id_ex_dataWrite, id_ex_memToReg;
    sc_signal<sc_uint<11>> id_ex_opAlu;
    sc_signal<sc_uint<2>> id_ex_flagSel;
    sc_signal<sc_int<32>> id_ex_read1, id_ex_read2, id_ex_immediate;
    sc_signal<sc_uint<32>> id_ex_pc;
    sc_signal<sc_uint<5>> id_ex_rd, id_ex_rt, id_ex_rs;
    sc_signal<sc_uint<26>> id_ex_absolute;
    
    sc_signal<bool> id_ex_isJump_out, id_ex_regWrite_out, id_ex_op2Sel_out,
        id_ex_dataRead_out, id_ex_dataWrite_out, id_ex_memToReg_out;
    sc_signal<sc_uint<11>> id_ex_opAlu_out;
    sc_signal<sc_uint<2>> id_ex_flagSel_out;
    sc_signal<sc_int<32>> id_ex_read1_out, id_ex_read2_out, id_ex_immediate_out;
    sc_signal<sc_uint<32>> id_ex_pc_out;
    sc_signal<sc_uint<5>> id_ex_rd_out, id_ex_rt_out, id_ex_rs_out;
    sc_signal<sc_uint<26>> id_ex_absolute_out;

    
    void clock_gen() {
        while (true) {
          clk.write(false);
          wait(CLOCK_SIZE_NS/2, SC_NS);
          clk.write(true);
          wait(CLOCK_SIZE_NS/2, SC_NS);
        }
      }

      void printIdEx() {
        std::cout << "\tID/EX:\n";
        std::cout << "\t\tEntradas:\n";
        std::cout << "\t\t\t.rst: " << earth.read() << std::endl;
        std::cout << "\t\t\t.isJump: " << id_ex_isJump.read() << std::endl;
        std::cout << "\t\t\t.regWrite: " << id_ex_regWrite.read() << std::endl;
        std::cout << "\t\t\t.op2Sel: " << id_ex_op2Sel.read() << std::endl;
        std::cout << "\t\t\t.dataRead: " << id_ex_dataRead.read() << std::endl;
        std::cout << "\t\t\t.dataWrite: " << id_ex_dataWrite.read() << std::endl;
        std::cout << "\t\t\t.memToReg: " << id_ex_memToReg.read() << std::endl;
        std::cout << "\t\t\t.opAlu: " << id_ex_opAlu.read() << std::endl;
        std::cout << "\t\t\t.flagSel: " << id_ex_flagSel.read() << std::endl;
        std::cout << "\t\t\t.read1: " << id_ex_read1.read() << std::endl;
        std::cout << "\t\t\t.read2: " << id_ex_read2.read() << std::endl;
        std::cout << "\t\t\t.immediate: " << id_ex_immediate.read() << std::endl;
        std::cout << "\t\t\t.pc: " << id_ex_pc.read() << std::endl;
        std::cout << "\t\t\t.rd: " << id_ex_rd.read() << std::endl;
        std::cout << "\t\t\t.rt: " << id_ex_rt.read() << std::endl;
        std::cout << "\t\t\t.rs: " << id_ex_rs.read() << std::endl;
        std::cout << "\t\t\t.absolute: "  << "0b" << (std::bitset<26>{id_ex_absolute.read()}) << std::endl;
        std::cout << "\t\tSaídas:\n";
        std::cout << "\t\t\t.isJump: " << id_ex_isJump_out.read() << std::endl;
        std::cout << "\t\t\t.regWrite: " << id_ex_regWrite_out.read() << std::endl;
        std::cout << "\t\t\t.op2Sel: " << id_ex_op2Sel_out.read() << std::endl;
        std::cout << "\t\t\t.dataRead: " << id_ex_dataRead_out.read() << std::endl;
        std::cout << "\t\t\t.dataWrite: " << id_ex_dataWrite_out.read() << std::endl;
        std::cout << "\t\t\t.memToReg: " << id_ex_memToReg_out.read() << std::endl;
        std::cout << "\t\t\t.opAlu: " << id_ex_opAlu_out.read() << std::endl;
        std::cout << "\t\t\t.flagSel: " << id_ex_flagSel_out.read() << std::endl;
        std::cout << "\t\t\t.read1: " << id_ex_read1_out.read() << std::endl;
        std::cout << "\t\t\t.read2: " << id_ex_read2_out.read() << std::endl;
        std::cout << "\t\t\t.immediate: " << id_ex_immediate_out.read() << std::endl;
        std::cout << "\t\t\t.pc: " << id_ex_pc_out.read() << std::endl;
        std::cout << "\t\t\t.rd: " << id_ex_rd_out.read() << std::endl;
        std::cout << "\t\t\t.rt: " << id_ex_rt_out.read() << std::endl;
        std::cout << "\t\t\t.rs: " << id_ex_rs_out.read() << std::endl;
        std::cout << "\t\t\t.absolute: "  << "0b" << (std::bitset<26>{id_ex_absolute_out.read()}) << std::endl;
    }

    void test() {
        wait(CLOCK_SIZE_NS, SC_NS);

        for(int i=0; i<5; i++) {
            wait(CLOCK_SIZE_NS, SC_NS);
            id_ex_isJump.write(i%2);
            id_ex_regWrite.write(i%2);
            id_ex_op2Sel.write(i%2);
            id_ex_dataRead.write(i%2);
            id_ex_dataWrite.write(i%2);
            id_ex_memToReg.write(i%2);
            id_ex_opAlu.write(i);
            id_ex_flagSel.write(i);
            id_ex_read1.write(i);
            id_ex_read2.write(i);
            id_ex_immediate.write(i);
            id_ex_pc.write(i);
            id_ex_rd.write(i);
            id_ex_rt.write(i);
            id_ex_rs.write(i);
            id_ex_absolute.write(i);
            std::cout << "srt[" << i << "]----------------------------------------" << std::endl;
            printIdEx();
            std::cout << "end[" << i << "]---------------------------------------- Press enter to continue..." << std::endl;
            cin.get();
        }

        sc_stop();
      }

    SC_CTOR(test_id_ex) : bar_id_ex("bar_id_ex") {
        vcc.write(true);
        earth.write(false);
        bar_id_ex.clk(clk);
        bar_id_ex.rst(earth);
        bar_id_ex.vcc(vcc);
        bar_id_ex.earth(earth);
        bar_id_ex.isJump(id_ex_isJump);
        bar_id_ex.regWrite(id_ex_regWrite);
        bar_id_ex.op2Sel(id_ex_op2Sel);
        bar_id_ex.dataRead(id_ex_dataRead);
        bar_id_ex.dataWrite(id_ex_dataWrite);
        bar_id_ex.memToReg(id_ex_memToReg);
        bar_id_ex.opAlu(id_ex_opAlu);
        bar_id_ex.flagSel(id_ex_flagSel);
        bar_id_ex.read1(id_ex_read1);
        bar_id_ex.read2(id_ex_read2);
        bar_id_ex.immediate(id_ex_immediate);
        bar_id_ex.pc(id_ex_pc);
        bar_id_ex.rd(id_ex_rd);
        bar_id_ex.rt(id_ex_rt);
        bar_id_ex.rs(id_ex_rs);
        bar_id_ex.absolute(id_ex_absolute);
    
        bar_id_ex.isJump_out(id_ex_isJump_out);
        bar_id_ex.regWrite_out(id_ex_regWrite_out);
        bar_id_ex.op2Sel_out(id_ex_op2Sel_out);
        bar_id_ex.dataRead_out(id_ex_dataRead_out);
        bar_id_ex.dataWrite_out(id_ex_dataWrite_out);
        bar_id_ex.memToReg_out(id_ex_memToReg_out);
        bar_id_ex.opAlu_out(id_ex_opAlu_out);
        bar_id_ex.flagSel_out(id_ex_flagSel_out);
        bar_id_ex.read1_out(id_ex_read1_out);
        bar_id_ex.read2_out(id_ex_read2_out);
        bar_id_ex.immediate_out(id_ex_immediate_out);
        bar_id_ex.pc_out(id_ex_pc_out);
        bar_id_ex.rd_out(id_ex_rd_out);
        bar_id_ex.rt_out(id_ex_rt_out);
        bar_id_ex.rs_out(id_ex_rs_out);
        bar_id_ex.absolute_out(id_ex_absolute_out);

        SC_THREAD(clock_gen);
        SC_THREAD(test);
    }
};

int sc_main(int argc, char **argv) {
    test_id_ex tb("tb");
    sc_start();
    return 0;
  }