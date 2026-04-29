#ifndef FORWARDING_UNIT_CPP
#define FORWARDING_UNIT_CPP

#include <systemc.h>

SC_MODULE(forwarding_unit) {
    sc_in<sc_uint<5>> ID_EX_rs;
    sc_in<sc_uint<5>> ID_EX_rt;
    sc_in<sc_uint<5>> EX_MEM_rd;
    sc_in<bool>       EX_MEM_RegWrite;
    sc_in<sc_uint<5>> MEM_WB_rd;
    sc_in<bool>       MEM_WB_RegWrite;

    sc_out<sc_uint<2>> ForwardA;
    sc_out<sc_uint<2>> ForwardB;

    void compute_forwarding();

    SC_CTOR(forwarding_unit){
        SC_METHOD(compute_forwarding);
        sensitive << ID_EX_rs << ID_EX_rt
                  << EX_MEM_rd << EX_MEM_RegWrite
                  << MEM_WB_rd << MEM_WB_RegWrite;
    }
};

// 2 -> EX_MEM
// 1 -> MEM_WB
// 0 -> sem adiantamento
void forwarding_unit::compute_forwarding(){ 
     if (EX_MEM_RegWrite.read() &&
         EX_MEM_rd.read() != 0 &&
         EX_MEM_rd.read() == ID_EX_rs.read()) {
         ForwardA.write(2); 
     } else if (MEM_WB_RegWrite.read() &&
                MEM_WB_rd.read() != 0 &&
                MEM_WB_rd.read() == ID_EX_rs.read()) {
         ForwardA.write(1); 
     } else {
        ForwardA.write(0);
     }
 
     if (EX_MEM_RegWrite.read() &&
         EX_MEM_rd.read() != 0 &&
         EX_MEM_rd.read() == ID_EX_rt.read()) {
         ForwardB.write(2); 
     } else if (MEM_WB_RegWrite.read() &&
                MEM_WB_rd.read() != 0 &&
                MEM_WB_rd.read() == ID_EX_rt.read()) {
         ForwardB.write(1); 
     } else {
        ForwardB.write(0);
     }
}

#endif // !FORWARDING_UNIT_CPP