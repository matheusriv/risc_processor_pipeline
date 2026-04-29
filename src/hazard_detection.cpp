#ifndef HAZARD_DETECTION_UNIT_CPP
#define HAZARD_DETECTION_UNIT_CPP

#include <systemc.h>
SC_MODULE(hazard_detection_unit) {
    sc_in<sc_uint<5>> IF_ID_rs;
    sc_in<sc_uint<5>> IF_ID_rt;
    sc_in<sc_uint<5>> ID_EX_rt;
    sc_in<bool>       ID_EX_MemRead;

    sc_out<bool> PCWrite;
    sc_out<bool> IF_ID_Write;
    sc_out<bool> ControlMux;

    void detect_conflict();

    SC_CTOR(hazard_detection_unit) {
        SC_METHOD(detect_conflict);
        sensitive << IF_ID_rs << IF_ID_rt
                  << ID_EX_rt << ID_EX_MemRead;
    }
};

void hazard_detection_unit::detect_conflict() {
    if (ID_EX_MemRead.read() &&
        (ID_EX_rt.read() == IF_ID_rs.read() || ID_EX_rt.read() == IF_ID_rt.read())) {
        PCWrite.write(false);
        IF_ID_Write.write(false);
        ControlMux.write(true);
    } else {
        PCWrite.write(true);
        IF_ID_Write.write(true);
        ControlMux.write(false); 
    }
}

#endif // !HAZARD_DETECTION_UNIT_CPP