#ifndef LOAD_DATA_CPP
#define LOAD_DATA_CPP

#include <systemc.h>

void load (sc_uint<8> *mem_instrucao, sc_int<32> *mem_dados) {
	mem_dados[0] = 100;
	mem_dados[1] = 45;
	mem_instrucao[0] = 0b10000000;
	mem_instrucao[1] = 0b00000001;
	mem_instrucao[2] = 0b00000000;
	mem_instrucao[3] = 0b00000000;
	mem_instrucao[4] = 0b10000000;
	mem_instrucao[5] = 0b00000010;
	mem_instrucao[6] = 0b00000000;
	mem_instrucao[7] = 0b00000001;
	mem_instrucao[8] = 0b01000000;
	mem_instrucao[9] = 0b00100010;
	mem_instrucao[10] = 0b00011000;
	mem_instrucao[11] = 0b00000001;
};

#endif // !LOAD_DATA_CPP