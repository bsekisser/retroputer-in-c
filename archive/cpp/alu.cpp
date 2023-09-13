#include "alu.hpp"

/* **** */

#include "buss.hpp"
#include "buss_receiver.hpp"

/* **** */

#include "bitfield.h"

/* **** */

static unsigned __shift(size_t size)
{
	const static unsigned bits[4] = { 8, 16, 19, 32 };
	return(bits[size & 3]);
}

static unsigned _carry_bit(size_t size)
{
	return(__shift(size));
}

static unsigned _carry_bit_value(size_t size)
{
	return(_BV(_carry_bit(size)));
}

static unsigned _mask(size_t size)
{
	return(_BM(__shift(size)));
}

static unsigned _sign_bit(size_t size)
{
	return(__shift(size) - 1);
}

static unsigned _sign_bit_value(size_t size)
{
	return(_BV(_sign_bit(size)));
}

/* **** */

static int signaled(void* this_param, unsigned value) {
	class alu* self = static_cast<class alu*>(this_param);

	/* **** */

	const unsigned command = self->command_buss().data();
	const int32_t op1 = (int32_t)self->op1_buss().data();
	const int32_t op2 = (int32_t)self->op2_buss().data();
	const size_t sizeof_ret = pbBFEXT(command, 8, 2);
	const size_t sizeof_op1 = pbBFEXT(command, 6, 2);
	const size_t sizeof_op2 = pbBFEXT(command, 4, 2);
	const unsigned op = command & 15;
	self->stats()->op[op]++;
	self->stats()->ops++;

	// get the operands and mask them based on the command size
	int32_t a = op1;
	if (a < 0) { a += _carry_bit_value(sizeof_op1); }
	a &= _mask(sizeof_op1);

	int32_t b = op2;
	if (b < 0) { b += _carry_bit_value(sizeof_op2); }
	a &= _mask(sizeof_op2);

	const unsigned flags_in = self->flags_buss().data();

	const unsigned sign_a = BEXT(a, _sign_bit_value(sizeof_op1));
	const unsigned sign_b = BEXT(b, _sign_bit_value(sizeof_op1));
	const unsigned carry_in = BEXT(flags_in, _ALU_FLAG_C);

	uint32_t flags = 0;
	int64_t result = 0;
	
	switch(op) {
		case ADD:
			result = a + b + carry_in;
		break;
		case AND:
			result = a & b;
		break;
		case DIV:
		case MOD:
			if (0 == b) {
				BSET(flags, _ALU_FLAG_C);
				BSET(flags, _ALU_FLAG_N);
				BSET(flags, _ALU_FLAG_V);
				BSET(flags, _ALU_FLAG_Z);
				result = 0;
			} else {
				switch(op) {
					case DIV:
						result = a / b;
					break;
					case MOD:
						result = a % b;
				}
			}
		break;
		case MUL:
			result = a * b;
		break;
		case NEG:
			result = -a;
		break;
		case NOT:
			result = ~a;
		break;
		case OR:
			result = a | b;
		break;
		case SHL:
			result = a << (b & 0x3f);
		break;
		case SHR:
			result = a >> (b & 0x3f);
		break;
		case SUB:
			result = a + (-b) + carry_in;
			if(result < 0)
				BSET(flags, _ALU_FLAG_C);
		break;
		case XOR:
			result = a ^ b;
		break;
	}

	/* **** */

	if(result < 0)
		result += _carry_bit_value(sizeof_ret);

	const unsigned negative = BEXT(result, _sign_bit(sizeof_ret));
	BSET_AS(flags, _ALU_FLAG_N, negative);

	BSET_AS(flags, _ALU_FLAG_C, !!(result & ~_mask(sizeof_ret)));

	result &= _mask(sizeof_ret);

	if(sign_a != negative) switch(op) {
		case ADD:
		case MUL:
			BSET_AS(flags, _ALU_FLAG_V, !!(sign_a == sign_b));
		break;
		case SUB:
			BSET_AS(flags, _ALU_FLAG_V, !!(sign_a != sign_b));
		break;
	}

	BSET_AS(flags, _ALU_FLAG_Z, !!(0 == result));

	/* **** */

	self->flags_buss().data(&flags);
	self->ret_buss().data((uint32_t*)&result);

	return(1);
}

/* **** */

alu::alu():
	_command(*(new buss(2, 0x3ff))),
	_execute(*(new buss(1, 1))),
	_flags(*(new buss(1, 7))),
	_op1(*(new buss(4))),
	_op2(*(new buss(4))),
	_ret(*(new buss(4)))
{
	signaled_receiver = new buss_receiver(signaled, this);
};
