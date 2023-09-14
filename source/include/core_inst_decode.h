#pragma once

/* **** */

typedef union inst_arg_flags_t {
	unsigned raw_flags;
	struct {
		unsigned i:1;
		unsigned n:1;
		unsigned s:1;
		unsigned u:1;
		unsigned w:1;
		unsigned x:1;
		unsigned y:1;
	};
}inst_arg_flags_t;

typedef struct inst_args_t* inst_args_p;
typedef struct inst_args_t {
	int32_t a;
	int32_t d;
	int32_t f;
	int32_t m;
	int32_t s;
	inst_arg_flags_t flags;
}inst_args_t;

/* **** */

#include "bitfield.h"

/* **** */

#define arg(_x) args->_x
#define arg_flag(_x) args->flags._x

static inst_args_p _core_inst_decode_ldst(core_p core, inst_args_p args)
{
	arg(a) = mlBFEXT(IR, 18, 0);
	arg_flag(i) = BEXT(IR, 21);
	arg(m) = mlBFEXT(IR, 23, 22);
	arg_flag(x) = BEXT(IR, 20);
	arg_flag(y) = BEXT(IR, 19);

	return(args);
}

static inst_args_p _core_inst_decode_ldst_ld(core_p core, inst_args_p args)
{
	_core_inst_decode_ldst(core, args);
	arg(d) = mlBFEXT(IR, 27, 24);

	return(args);
}

static inst_args_p _core_inst_decode_ldst_st(core_p core, inst_args_p args)
{
	_core_inst_decode_ldst(core, args);
	arg(s) = mlBFEXT(IR, 27, 24);

	return(args);
}

static inst_args_p _core_inst_decode_br_call(core_p core, inst_args_p args, unsigned offset)
{
	arg(a) = mlBFEXT(IR, 15 - offset, 0);
	arg_flag(i) = BEXT(IR, 21 - offset);
	arg(f) = mlBFEXT(IR, 26 - offset, 24 - offset);
	arg(m) = mlBFEXT(IR, 23 - offset, 22 - offset);
	arg_flag(n) = BEXT(IR, 27 - offset);
	arg_flag(s) = BEXT(IR, 16 - offset);
	arg_flag(u) = BEXT(IR, 18 - offset);
	arg_flag(w) = BEXT(IR, 17 - offset);
	arg_flag(x) = BEXT(IR, 20 - offset);
	arg_flag(y) = BEXT(IR, 19 - offset);

	return(args);
}
