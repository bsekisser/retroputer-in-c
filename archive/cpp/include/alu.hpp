#pragma once

/* **** */

enum {
	_ALU_FLAG_Z = 0,
	_ALU_FLAG_V = 1,
	_ALU_FLAG_C = 2,
	_ALU_FLAG_N = 3,
};

enum COMMANDS {
    ADD = 0x01,
    SUB = 0x02,
    MUL = 0x03,
    DIV = 0x04,
    MOD = 0x05,
    SMUL = 0x06,
    SDIV = 0x07,
    SMOD = 0x08,
    NEG = 0x09,
    SHL = 0x0A,
    SHR = 0x0B,
    AND = 0x0C,
    OR = 0x0D,
    XOR = 0x0E,
    NOT = 0x0F,
};

enum SIZES {
    BYTE = 0b00,
    WORD = 0b01,
    ADDR = 0b10,
    RET_8 = 0b0000000000,
    RET_16 = 0b0100000000,
    RET_19 = 0b1000000000,
    RET_32 = 0b1100000000,
    OP1_8 = 0b0000000000,
    OP1_16 = 0b0001000000,
    OP1_19 = 0b0010000000,
    OP1_32 = 0b0011000000,
    OP2_8 = 0b0000000000,
    OP2_16 = 0b0000010000,
    OP2_19 = 0b0000100000,
    OP2_32 = 0b0000110000,
};

/* **** */

#include "buss.hpp"
#include "buss_receiver.hpp"

/* **** */

typedef struct alu_stats_t* alu_stats_p;
typedef struct alu_stats_t {
	unsigned ops;
	unsigned op[16];
}alu_stats_t;

class alu {
	class buss& _command;
	class buss& _execute;
	class buss& _flags;
	class buss& _op1;
	class buss& _op2;
	class buss& _ret;
	class buss_receiver* signaled_receiver;
	alu_stats_t _stats;
public:
	alu();
	class buss& flags_buss() { return(_flags); };
	class buss& command_buss() { return(_command); };
	class buss& execute_buss() { return(_execute); };
	class buss& op1_buss() { return(_op1); };
	class buss& op2_buss() { return(_op2); };
	class buss& ret_buss() { return(_ret); };
	alu_stats_p stats() { return(&_stats); };
};
