#include "config.h"
#include "alu.h"

/* **** */

#include "buss.h"

#include "main.h"

/* **** */

#include "bitfield.h"
#include "err.h"
#include "handle.h"
#include "log.h"

/* **** */

typedef union alu_flags_t* alu_flags_p;
typedef union alu_flags_t {
	unsigned raw_flags;
	struct {
		unsigned carry:1;
		unsigned negative:1;
		unsigned overflow:1;
		unsigned zero:1;
	};
}alu_flags_t;

typedef struct alu_result_t* alu_result_p;
typedef struct alu_result_t {
	unsigned long long result;
	alu_flags_t flags;
}alu_result_t;

typedef struct alu_t {
	buss_p command;
	buss_p execute;
	buss_p flags;
	buss_p op[2];
	buss_p ret;

	alu_stats_t stats;
	buss_receiver_t signaled;
}alu_t;

/* **** */

const unsigned mask[4] = { 0x000000ff, 0x0000ffff, 0x0007ffff, 0xffffffff };
const unsigned carry_bit[4] = { 8, 16, 19, 32 };
const unsigned sign_bit[4] = { 7, 15, 18, 31 };

extern inline long long int __alubox(alu_flags_p flags, unsigned opc, signed a, signed b, unsigned carry_in) {
	long long int ret = 0;

	switch(opc & 15) {
		case _ALU_OP_ADD:
			ret = a + b + carry_in;
		break;
		case _ALU_OP_AND:
			ret = a & b;
		break;
		case _ALU_OP_DIV:
		case _ALU_OP_MOD:
			if(0 == b) {
				flags->carry = 1;
				flags->negative = 1;
				flags->overflow = 1;
				flags->zero = 1;
				ret = 0;
			} else {
				if(opc == _ALU_OP_DIV)
					ret = a / b;
				else
					ret = a % b;
			}
		break;
		case _ALU_OP_MUL:
			ret = a * b;
		break;
		case _ALU_OP_NEG:
			ret = -a;
		break;
		case _ALU_OP_NOT:
			ret = ~a;
		break;
		case _ALU_OP_OR:
			ret = a | b;
		break;
		case _ALU_OP_SHL:
			ret = a << (b & 0x3f);
		break;
		case _ALU_OP_SHR:
			ret = a >> (b & 0x3f);
		break;
		case _ALU_OP_SUB:
			ret = a + (-b) + carry_in;
			if(ret < 0) {
				flags->carry = 1;
			}
		break;
		case _ALU_OP_XOR:
			ret = a ^ b;
		break;
		default: break;
	}

	return(ret);
}

static int _alu_signaled(void* param, unsigned value)
{
	alu_p alu = param;
	
	const unsigned command = buss_data(alu->command, 0);
	const signed op[2] = {
		(signed)buss_data(alu->op[0], 0),
		(signed)buss_data(alu->op[1], 0),
	};

	const unsigned opc = command & 0b1111;

	alu->stats.op[opc]++;
	alu->stats.ops++;

	const size_t sizeof_op1 = pbBFEXT(command, 6, 2);
	const signed op1 = (op[0] < 0) ? (op[0] + _BV(carry_bit[sizeof_op1])) : op[0];
	const signed a = op1 & mask[sizeof_op1];
	const unsigned sign_a = BEXT(a, sign_bit[sizeof_op1]);
	
	const size_t sizeof_op2 = pbBFEXT(command, 4, 2);
	const signed op2 = (op[1] < 0) ? (op[1] + _BV(carry_bit[sizeof_op2])) : op[1];
	const signed b = op2 & mask[sizeof_op2];
	const unsigned sign_b = BEXT(b, sign_bit[sizeof_op2]);

	unsigned flags_in = buss_data(alu->flags, 0);

	unsigned carry_in = ALU_FLAG_BEXT(flags_in, CARRY);

	alu_flags_t flags;

	const size_t sizeof_ret = pbBFEXT(command, 8, 2);
	long long int ret = __alubox(&flags, opc, a, b, carry_in);
	
	if(ret < 0)
		ret += carry_bit[sizeof_ret];

	flags.negative = BEXT(ret, sign_bit[sizeof_ret]);
	flags.carry |= !!(ret & ~mask[sizeof_ret]);

	switch(opc) {
		case _ALU_OP_ADD:
		case _ALU_OP_MUL:
			if((sign_a == sign_b) && (sign_a != flags.negative))
				flags.overflow = 1;
		break;
		case _ALU_OP_SUB:
			if((sign_a != sign_b) && (sign_a != flags.negative))
				flags.overflow = 1;
				
	}

	unsigned ret_out = ret & mask[sizeof_ret];

	flags.zero = (0 == ret_out);

	unsigned flags_out = 0;
	ALU_FLAG_BSET_AS(flags_out, CARRY, flags.carry);
	ALU_FLAG_BSET_AS(flags_out, NEGATIVE, flags.negative);
	ALU_FLAG_BSET_AS(flags_out, OVERFLOW, flags.overflow);
	ALU_FLAG_BSET_AS(flags_out, ZERO, flags.zero);

	buss_data(alu->flags, &flags_out);
	buss_data(alu->ret, &ret_out);

	/* **** */

	return(0);
}

/* **** */

alu_p alu_alloc(main_p xxx, alu_h h2alu)
{
	TRACE_ALLOC();

	alu_p alu = handle_calloc((void*)h2alu, 1, sizeof(alu_t));
	ERR_NULL(alu);

	/* **** */

	buss_alloc(xxx, &alu->command, 4, 0);
	buss_alloc(xxx, &alu->execute, 1, 1);
	buss_alloc(xxx, &alu->flags, 1, 7);
	buss_alloc(xxx, &alu->op[0], 4, 0);
	buss_alloc(xxx, &alu->op[1], 4, 0);
	buss_alloc(xxx, &alu->ret, 4, 0);

	/* **** */
	return(alu);
}

void alu_init(alu_p alu)
{
	TRACE_INIT();

	buss_init(alu->command);
	buss_init(alu->execute);
	buss_init(alu->flags);
	buss_init(alu->op[0]);
	buss_init(alu->op[1]);
	buss_init(alu->ret);

	buss_add_receiver(alu->execute, &alu->signaled, _alu_signaled, alu);
}
