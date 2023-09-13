#pragma once

/* **** */

typedef struct alu_t** alu_h;
typedef struct alu_t* alu_p;

enum {
	_ALU_OP_ADD,
	_ALU_OP_AND,
	_ALU_OP_DIV,
	_ALU_OP_MOD,
	_ALU_OP_MUL,
	_ALU_OP_NEG,
	_ALU_OP_NOT,
	_ALU_OP_OR,
	_ALU_OP_SDIV,
	_ALU_OP_SMUL,
	_ALU_OP_SHL,
	_ALU_OP_SHR,
	_ALU_OP_SUB,
	_ALU_OP_XOR,
	_ALU_OP_COUNT,
};

enum {
	_ALU_FLAG_ZERO = 0,
	_ALU_FLAG_OVERFLOW = 1,
	_ALU_FLAG_CARRY = 2,
	_ALU_FLAG_NEGATIVE = 3,
};

#define _ALU_FLAG_(_x) _ALU_FLAG_##_x
#define ALU_FLAG_BEXT(_in, _x) BEXT(_in, _ALU_FLAG_(_x))
#define ALU_FLAG_BSET_AS(_in, _x, _as) BSET_AS(_in, _ALU_FLAG_(_x), _as)

typedef struct alu_stats_t* alu_stats_p;
typedef struct alu_stats_t {
	unsigned ops;
	unsigned op[_ALU_OP_COUNT];
}alu_stats_t;
