#pragma once

/* **** */

enum {
	_alu_add = 0x1,
	_alu_sub = 0x2,
	_alu_mul = 0x3,
	_alu_div = 0x4,
	_alu_mod = 0x5,
	_alu_smul = 0x6,
	_alu_sdiv = 0x7,
	_alu_smod = 0x8,
	_alu_neg = 0x9,
	_alu_shl = 0xa,
	_alu_shr = 0xb,
	_alu_and = 0xc,
	_alu_orr = 0xd,
	_alu_xor = 0xe,
	_alu_not = 0xf,
};

const char* aluop_action[16] = {
	"???", "+", "-", "*",
	"/", "%", "*", "/",
	"%", "-", "<<<", ">>>",
	"&", "|", "^", "~",
	};

const char* aluop_name[16] = {
	"xxx", "add", "sub", "mul",
	"div", "mod", "smul", "sdiv",
	"smod", "neg", "shl", "shr",
	"and", "or", "xor", "not",
	};

typedef struct alubox_op_t* alubox_op_p;
typedef struct alubox_op_t {
	unsigned r;
	size_t size;
	uint32_t v;
}alubox_op_t;

typedef struct alubox_t* alubox_p;
typedef struct alubox_t {
	unsigned wb_r;
	alubox_op_t op1;
	alubox_op_t op2;
}alubox_t;

/* **** */

#include "core_flags.h"
#include "core_utility.h"

/* **** */

static inline uint32_t __alubox_shift(size_t size)
{
	unsigned _shift[4] = { 8, 16, 19, 32 };
	return(_shift[size & 3]);
}

static inline uint32_t _alubox_carry_bit(size_t size)
{
	return(__alubox_shift(size));
}

static inline uint32_t _alubox_carry_bit_value(size_t size)
{
	return(_BV(_alubox_carry_bit(size)));
}

static inline uint32_t _alubox_mask(size_t size)
{
	return(mlBF(__alubox_shift(size) - 1, 0));
//	return(_BM(__alubox_shift(size) - 1));
}

static inline uint32_t _alubox_op_v(alubox_op_p op)
{
	const size_t size = op->size;
	const uint32_t v = mlBFEXT(op->v, __alubox_shift(size), 0);

	if(0) LOG("size: 0x%08zx, v: 0x%08x(0x%08x)", op->size, v, op->v);

	return(v);
}

static inline uint32_t _alubox_sign_bit(size_t size)
{
	return(__alubox_shift(size) - 1);
}

static inline uint32_t core_alubox_op(alubox_op_p op, size_t size, uint32_t v)
{
	op->size = size;
	op->v = v;
//	op->v = mlBFEXTs(v, __alubox_shift(op->size), 0);

	if(0) LOG("size: 0x%08zx, v: 0x%08x(0x%08x)", op->size, op->v, v);

	return(op->v);
}

static inline uint32_t core_alubox_op_r_src(core_p core, alubox_op_p op, unsigned r, size_t size)
{
	op->r = r;
	return(core_alubox_op(op, size, _gpr(core, r, 0)));
}

static uint32_t core_alubox(core_p core, alubox_p alu, unsigned cmd)
{
	unsigned flags_in = rGPR(FLAGS);
	unsigned carry_in = BEXT(flags_in, _ALU_FLAG_C);

	if(0) LOG("flags_in: 0x%08x, carry_in: %01u", flags_in, carry_in);

	const unsigned sizeof_op1 = alu->op1.size;

	int16_t a = 0;
	if(1) {
		a = alu->op1.v;
		if(a < 0) a += _alubox_carry_bit_value(sizeof_op1);
		a &= _alubox_mask(sizeof_op1);
	} else
		a = _alubox_op_v(&alu->op1);

	if(0) LOG("op1: 0x%08x, a: 0x%08x, size: 0x%08zx", alu->op1.v, a, alu->op1.size);
	
	const unsigned sizeof_op2 = alu->op2.size;

	int16_t b = 0;
	if(1) {
		b = alu->op2.v;
		if(b < 0) b += _alubox_carry_bit_value(sizeof_op2);
		b &= _alubox_mask(sizeof_op2);
	} else
		b = _alubox_op_v(&alu->op2);

	if(0) LOG("op2: 0x%08x, b: 0x%08x, size: 0x%08zx", alu->op2.v, b, alu->op2.size);

	unsigned sign_a = BEXT(a, _alubox_sign_bit(sizeof_op1));
	unsigned sign_b = BEXT(b, _alubox_sign_bit(sizeof_op2));

	uint16_t flags = flags_in & ~_ALU_FLAGS_BV_CNVZ;
	int32_t ret = 0;

	switch(cmd & 15) {
		case _alu_add:
			ret = a + b + carry_in;
		break;
		case _alu_and:
			ret = a & b;
		break;
		case _alu_div:
		case _alu_mod:
		case _alu_sdiv:
		case _alu_smod:
			if(0 == b) {
				BSET(flags, _ALU_FLAG_C);
				BSET(flags, _ALU_FLAG_N);
				BSET(flags, _ALU_FLAG_V);
				BCLR(flags, _ALU_FLAG_Z);
				ret = 0;
				BSET(flags, _STATUS_FLAG_EXCEPTION);
			} else {
				switch(cmd & 15) {
					case _alu_div:
						ret = (uint16_t)a / (uint16_t)b;
					break;
					case _alu_mod:
						ret = (uint16_t)a % (uint16_t)b;
					break;
					case _alu_sdiv:
						ret = (int16_t)a / (int16_t)b;
					break;
					case _alu_smod:
						ret = (int16_t)a % (int16_t)b;
					break;
				}
			}
		break;
		case _alu_mul:
			ret = (uint16_t)a * (uint16_t)b;
		break;
		case _alu_neg:
			ret = -a;
		break;
		case _alu_not:
			ret = ~a;
		break;
		case _alu_orr:
			ret = a | b;
		break;
		case _alu_shl:
			ret = a << (b & 0x3f);
		break;
		case _alu_shr:
			ret = ((int16_t)a) >> (b & 0x3f);
		break;
		case _alu_smul:
			ret = (int16_t)a * (int16_t)b;
		break;
		case _alu_sub:
			ret = a + (-b) + carry_in;
			if(ret < 0) {
				BSET(flags, _ALU_FLAG_C);
			}
		break;
		case _alu_xor:
			ret = a ^ b;
		break;
		default:
			LOG_ACTION(exit(-1));
		break;
	}

	size_t sizeof_ret = (sizeof_op2 < sizeof_op1) ? sizeof_op1 : sizeof_op2;

	if(ret < 0)
		ret += _alubox_carry_bit_value(sizeof_ret);

	uint16_t result_out = ret & _alubox_mask(sizeof_ret);

	unsigned nf = BEXT(result_out, _alubox_sign_bit(sizeof_ret));
	BSET_AS(flags, _ALU_FLAG_N, nf);

//	BSET_AS(flags, _ALU_FLAG_C, !!(ret & ~_alubox_mask(sizeof_ret)));
	BSET_AS(flags, _ALU_FLAG_C, result_out != ret);

	if(sign_a != nf) switch(cmd) {
		case _alu_add:
//		case _alu_mul:
			BSET_AS(flags, _ALU_FLAG_V, sign_a == sign_b);
		break;
		case _alu_sub:
			BSET_AS(flags, _ALU_FLAG_V, sign_a != sign_b);
		break;
	}

	BSET_AS(flags, _ALU_FLAG_Z, 0 == result_out);
	rGPR(FLAGS) = flags;

//	if(0) LOG("flags: 0x%08x, ret = 0x%016" PRIx64 ", result = 0x%08x",
//		flags, ret, result_out);
	if(0) LOG("flags: 0x%08x, ret = 0x%08x, result = 0x%08x",
		flags, ret, result_out);

	return(result_out);
}

static uint32_t core_alubox_wb(core_p core, alubox_p alu, unsigned cmd)
{
	const uint16_t result = (uint16_t)core_alubox(core, alu, cmd);
	_gpr(core, alu->wb_r, &result);

	return((int16_t)result);
}
