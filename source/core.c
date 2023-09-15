#include "config.h"
#include "core.h"

/* **** */

#include "core_trace.h"
#include "memory.h"
#include "io_buss.h"
#include "register_file.h"

/* **** */

#include "main.h"

/* **** */

#include "bitfield.h"
#include "callback_qlist.h"
#include "err.h"
#include "handle.h"
#include "log.h"
#include "unused.h"

/* **** */

#include "core_t.h"
#include "main_t.h"

#define PC rGPR(PC)
#define SP rGPR(SP)

#include "core_alubox.h"
#include "core_flags.h"
#include "core_inst_decode.h"
#include "core_utility.h"

/* **** */

static int _core_atexit(void* param)
{
	TRACE_ATEXIT();

	handle_free(param);
	return(0);
}

static int _core_atreset(void* param)
{
	TRACE_ATRESET();

	core_p core = param;

	unsigned map = (((31 << 5) | 2) << 5) | 1;
	memory_map(core->memory, &map);

	PC = 0xff00;
	SP = (rGPR(BP) = 0x2000);
//	rGPR(MM) = map;

	/* **** */
	return(0);
}

/* **** */

static uint16_t __core_inst__aluop_db(core_p core, alubox_p alu, unsigned cmd)
{
	const unsigned d = mlBFMOV(IR, 10, 9, 1) | 1;

	alu->wb_r = d;
	core_alubox_op_r_src(core, &alu->op1, d, 1);
	core_alubox_op(&alu->op2, 1, mlBFEXT(IR, 7, 0));

	return(core_alubox(core, alu, cmd));
}

static uint16_t __core_inst__aluop_db_wb(core_p core, alubox_p alu, unsigned cmd)
{
	const uint16_t result = __core_inst__aluop_db(core, alu, cmd);
	_gpr(core, alu->wb_r, &result);

	return(result);
}

static uint16_t __core_inst__aluop_ds(core_p core, alubox_p alu, unsigned cmd)
{
	const unsigned d = mlBFEXT(IR, 7, 4);
	const unsigned d_is_byte = d & 1;

	const unsigned s = mlBFEXT(IR, 3, 0);
	const unsigned s_is_byte = s & 1;

	alu->wb_r = d;
	core_alubox_op_r_src(core, &alu->op1, d, 2 - d_is_byte);
	core_alubox_op_r_src(core, &alu->op2, s, 2 - s_is_byte);

	return(core_alubox(core, alu, cmd));
}

static uint16_t __core_inst__aluop_ds_wb(core_p core, alubox_p alu, unsigned cmd)
{
	const uint16_t result = __core_inst__aluop_ds(core, alu, cmd);
	_gpr(core, alu->wb_r, &result);

	return(result);
}

static uint16_t __core_inst__aluop_dw(core_p core, alubox_p alu, unsigned cmd)
{
	const unsigned d = mlBFMOV(IR, 18, 17, 1);

	alu->wb_r = d;
	core_alubox_op_r_src(core, &alu->op1, d, 2);
	core_alubox_op(&alu->op2, 2, mlBFEXT(IR, 15, 0));

	return(core_alubox(core, alu, cmd));
}

static uint16_t __core_inst__aluop_dw_wb(core_p core, alubox_p alu, unsigned cmd)
{
	const uint16_t result = __core_inst__aluop_dw(core, alu, cmd);
	_gpr(core, alu->wb_r, &result);

	return(result);
}

static uint16_t __core_inst__aluop_rn(core_p core, alubox_p alu, unsigned cmd)
{
	const uint8_t n = mlBFEXT(IR, 3, 0);
	const unsigned r = mlBFEXT(IR, 7, 4);
	const unsigned is_byte = r & 1;

	alu->wb_r = r;
	core_alubox_op_r_src(core, &alu->op1, r, 2 - is_byte);
	core_alubox_op(&alu->op2, 1, n);

	return(core_alubox(core, alu, cmd));
}

static uint16_t __core_inst__aluop_rn_wb(core_p core, alubox_p alu, unsigned cmd)
{
	const uint16_t result = __core_inst__aluop_rn(core, alu, cmd);
	_gpr(core, alu->wb_r, &result);

	return(result);
}

/* **** */

static int _core_inst_aluop_db(core_p core, unsigned cmd)
{
	alubox_t alu;
	const uint16_t result = __core_inst__aluop_db_wb(core, &alu, cmd);

	CORE_TRACE_START("%s %s, #0x02x", aluop_name[cmd], reg_name(alu.wb_r), alu.op2.v);
	CORE_TRACE_END(" /* 0x%04x %s 0x%02x = 0x%04x */", alu.op1.v, aluop_action[cmd], alu.op2.v, result);

	return(1);
}

static int _core_inst_aluop_ds(core_p core, unsigned cmd)
{
	alubox_t alu;
	const uint16_t result = __core_inst__aluop_ds_wb(core, &alu, cmd);

	CORE_TRACE_START("%s %s, %s", aluop_name[cmd], reg_name(alu.op1.r), reg_name(alu.op2.r));
	CORE_TRACE_END(" /* 0x%04x %s 0x%04x = 0x%04x */", alu.op1.v, aluop_action[cmd], alu.op2.v, result);

	return(1);
}

static int _core_inst_aluop_dw(core_p core, unsigned cmd)
{
	alubox_t alu;
	const uint16_t result = __core_inst__aluop_dw_wb(core, &alu, cmd);

	CORE_TRACE_START("%s %s, #0x04x", aluop_name[cmd], reg_name(alu.op1.r), alu.op2.v);
	CORE_TRACE_END(" /* 0x%04x %s 0x%04x = 0x%04x */", alu.op1.v, aluop_action[cmd], alu.op2.v, result);

	return(1);
}

static int _core_inst_aluop_rn(core_p core, unsigned cmd)
{
	alubox_t alu;
	const uint16_t result = __core_inst__aluop_rn_wb(core, &alu, cmd);
	
	CORE_TRACE_START("%s %s, #0x02x", aluop_name[cmd], reg_name(alu.op1.r), alu.op2.v);
	CORE_TRACE_END(" /* 0x%04x %s 0x%02x = 0x%04x */", alu.op1.v, aluop_action[cmd], alu.op2.v, result);

	return(1);
}

static int _core_inst_add_db(core_p core)
{ return(_core_inst_aluop_db(core, _alu_add)); }

static int _core_inst_add_ds(core_p core)
{ return(_core_inst_aluop_ds(core, _alu_add)); }

static int _core_inst_add_dw(core_p core)
{ return(_core_inst_aluop_dw(core, _alu_add)); }

static int _core_inst_and_db(core_p core)
{ return(_core_inst_aluop_db(core, _alu_and)); }

static int _core_inst_and_ds(core_p core)
{ return(_core_inst_aluop_ds(core, _alu_and)); }

static int _core_inst_and_dw(core_p core)
{ return(_core_inst_aluop_dw(core, _alu_and)); }

static int _core_inst_br_call_f(core_p core)
{
	inst_args_t _args;

	_core_inst_decode_br_call(core, &_args, 0);
	_calc_brf(core, &_args);

	return(1);
}

static int _core_inst_brk(core_p core)
{
	BSET(rGPR(FLAGS), _STATUS_FLAG_INTERRUPT_DISABLE);
	BSET(rGPR(FLAGS), _STATUS_FLAG_SINGLE_STEP);

	CORE_TRACE("brk");

	return(1);
}

static int _core_inst_brs_calls_f(core_p core)
{
	inst_args_t _args;

	_core_inst_decode_br_call(core, &_args, 8);
	_calc_brf(core, &_args);

	return(1);
}

static int _core_inst_clr_f(core_p core)
{
	const unsigned bit = mlBFEXT(IR, 2, 0);

	const unsigned flags_saved = rGPR(FLAGS);
	BCLR(rGPR(FLAGS), bit);

	CORE_TRACE_START("clr", flag_name(bit));
	CORE_TRACE_END(" /* 0x%08x --> 0x%08x */", flags_saved, rGPR(FLAGS));

	return(1);
}

static int _core_inst_cmp_db(core_p core)
{
	alubox_t alu;

	const uint8_t result = __core_inst__aluop_db(core, &alu, _alu_sub);

	CORE_TRACE_START("cmp %s, #0x%02x", reg_name(alu.wb_r), alu.op2.v);
	CORE_TRACE_END(" /* 0x%04x - 0x%02x => 0x%04x */", alu.op1.v, alu.op2.v, result);

	return(1);
}

static int _core_inst_cmp_ds(core_p core)
{
	alubox_t alu;
	const uint16_t result = __core_inst__aluop_ds(core, &alu, _alu_sub);

	CORE_TRACE_START("cmp %s, %s", reg_name(alu.op1.r), reg_name(alu.op2.r));
	CORE_TRACE_END(" /* 0x%04x - 0x%04x = 0x%04x */", alu.op1.v, alu.op2.v, result);

	return(1);
}

static int _core_inst_cmp_dw(core_p core)
{
	alubox_t alu;
	
	const int16_t result = __core_inst__aluop_dw(core, &alu, _alu_sub);

	CORE_TRACE_START("cmp %s, #0x%04x", reg_name(alu.wb_r), alu.op2.v);
	CORE_TRACE_END(" /* 0x%04x - 0x%04x => 0x%04x */", alu.op1.v, alu.op2.v, result);

	return(1);
}

static int _core_inst_dec_r(core_p core)
{
	alubox_t alu;

	alu.wb_r = mlBFEXT(IR, 3, 0);
	const unsigned is_byte = alu.wb_r & 1;

	const uint16_t saved_r = _gpr(core, alu.wb_r, 0);
	
	core_alubox_op(&alu.op1, 2 - is_byte, saved_r);
	core_alubox_op(&alu.op2, 2 - is_byte, 1);

	const uint16_t result = core_alubox_wb(core, &alu, _alu_sub);

	CORE_TRACE_START("dec %s", reg_name(alu.wb_r));
	CORE_TRACE_END(" /* 0x%04x --> 0x%04x */", saved_r, result);

	return(1);
}

static int _core_inst_div_ds(core_p core)
{ return(_core_inst_aluop_ds(core, _alu_div)); }

/*const unsigned _core_inst_enter_task_list[] = {
	TASK_rPUSH(BP), TASK_rMOV(BP, SP);
	TASK_rSUB_mlIP(SP, 7, 0); 
};*/

static int _core_inst_enter_n(core_p core)
{
	const unsigned n = mlBFEXT(IR, 7, 0);
	
	__push(core, 2, rGPR(BP));
	rGPR(BP) = SP;
	SP -= n;
	
	CORE_TRACE("enter #0x%02x", n);

	return(1);
}

static int _core_inst_exc(core_p core)
{
	const unsigned r = mlBFEXT(IR, 3, 0);
	const unsigned is_byte = r & 1;

	const unsigned mask = is_byte ? 0xff : 0xffff;
	const unsigned sign = is_byte ? 0x80 : 0x8000;

	const uint16_t saved_v = _gpr(core, r, 0) & mask;
	uint16_t v = saved_v;

	if(r & 1)
		v = (v >> 4) | (v << 4);
	else
		v = (v >> 8) | (v << 8);

	v &= mask;

	_gpr(core, r, &v);
	
	BMAS(rGPR(FLAGS), _ALU_FLAG_N, !!(v & sign));
	BMAS(rGPR(FLAGS), _ALU_FLAG_Z, (0 == v));
	
	CORE_TRACE_START("exc %s", reg_name(r));
	CORE_TRACE_END(" /* 0x%08x --> 0x%08x */", saved_v, v);

	return(1);
}

static int _core_inst_exit_n(core_p core) // this seems incorrect should it not be SP = BP?
{
	const unsigned n = mlBFEXT(IR, 7, 0);

	SP += n;
	rGPR(BP) = __pop(core, 2);

	CORE_TRACE("exit #0x%02x", n);

	return(1);
}

static int _core_inst_halt(core_p core)
{
	BSET(rGPR(FLAGS), _STATUS_FLAG_SINGLE_STEP);

	CORE_TRACE("FIXME: halt");

	return(1);
}

static int _core_inst_in_rp(core_p core)
{
	const unsigned p = mlBFEXT(IR, 7, 0);
	const unsigned r = mlBFEXT(IR, 15, 12);

	const uint16_t in = io_buss(core->xxx->io_buss, p, 0);
	_gpr(core, r, &in);

	CORE_TRACE_START("in %s, 0x%02x", reg_name(r), p);
	CORE_TRACE_END(" /* [0x%02x] --> 0x%04x */", p, in);

	return(1);
}

static int _core_inst_inc_r(core_p core)
{
	alubox_t alu;
	
	alu.wb_r = mlBFEXT(IR, 3, 0);
	const unsigned is_byte = alu.wb_r & 1;

	const uint16_t saved_r = _gpr(core, alu.wb_r, 0);

	core_alubox_op(&alu.op1, 2 - is_byte, saved_r);
	core_alubox_op(&alu.op2, 2 - is_byte, 1);

	const uint16_t result = core_alubox_wb(core, &alu, _alu_add);

	CORE_TRACE_START("inc %s", reg_name(alu.wb_r));
	CORE_TRACE_END(" /* 0x%04x --> 0x%04x */", saved_r, result);

	return(1);
}

static int _core_inst_loop_r(core_p core)
{ LOG_ACTION(exit(-1)); } // TODO: not implimented

static int _core_inst_loops_r(core_p core)
{ LOG_ACTION(exit(-1)); } // TODO: not implimented

static int _core_inst_mod_ds(core_p core)
{ return(_core_inst_aluop_ds(core, _alu_mod)); }

static int _core_inst_mov_ds(core_p core)
{
	const unsigned d = mlBFEXT(IR, 7, 4);
	const unsigned s = mlBFEXT(IR, 3, 0);

	const uint16_t saved_s = _gpr(core, s, 0);
	_gpr(core, d, &saved_s);

	CORE_TRACE_START("mov %s, %s", reg_name(d), reg_name(s));
	CORE_TRACE_END(" /* 0x%04x */", saved_s);

	return(1);
}

static int _core_inst_mul_ds(core_p core)
{ return(_core_inst_aluop_ds(core, _alu_mul)); }

static int _core_inst_ld(core_p core)
{
	inst_args_t _args;
	inst_args_p args = _core_inst_decode_ldst_ld(core, &_args);
	
	const unsigned is_byte = arg(d) & 1;
	
	CORE_TRACE_START("ld %s, [", reg_name(arg(d)));
	const uint32_t address = __calc_address(core, args, arg(a), 0);
	_CORE_TRACE_("]");

	uint16_t read;

	if(is_byte) {
		read = memory_read_byte(core->memory, address);
		_CORE_TRACE_(" /* [0x%08x] --> 0x%02x */", address, read);
	} else {
		read = memory_read_word(core->memory, address);
		_CORE_TRACE_(" /* [0x%08x] --> 0x%04x */", address, read);
	}

	CORE_TRACE_END("");

	_gpr(core, arg(d), &read);

	if(0) {
		CORE_TRACE_START("ld 0x%08x", arg(a));
		_CORE_TRACE_(", %s", reg_name(arg(d)));
		_CORE_TRACE_(", x:%01u", arg_flag(x));
		_CORE_TRACE_(", y:%01u", arg_flag(y));
		_CORE_TRACE_(", m:%02u", arg(m));
		CORE_TRACE_END(", i:%01u", arg_flag(i));
	}

	return(1);
}

static int _core_inst_ld_db(core_p core)
{
	const unsigned d = mlBFMOV(IR, 19, 17, 1) | 1;
	const uint8_t b = mlBFEXT(IR, 7, 0);
	
	GPRx(d) = b;

	CORE_TRACE("ld %s, #0x%02x", reg_name(d), b);

	return(1);
}

static int _core_inst_ld_dw(core_p core)
{
	const unsigned d = mlBFMOV(IR, 27, 25, 1);
	const uint16_t w = mlBFEXT(IR, 15, 0);
	
	GPRx(d) = w;

	CORE_TRACE("ld %s, #0x%04x", reg_name(d), w);

	return(1);
}

static int _core_inst_neg(core_p core)
{
	const unsigned r = mlBFEXT(IR, 3, 0);
	const unsigned is_byte = r & 1;

	const unsigned mask = is_byte ? 0xff : 0xffff;
	const unsigned sign = is_byte ? 0x80 : 0x8000;

	const uint16_t saved_v = _gpr(core, r, 0);
	const uint16_t v = -saved_v & mask;
	_gpr(core, r, &v);
	
	BMAS(rGPR(FLAGS), _ALU_FLAG_N, !!(v & sign));
	BMAS(rGPR(FLAGS), _ALU_FLAG_Z, (0 == v));
	
	CORE_TRACE_START("exc %s", reg_name(r));
	CORE_TRACE_END(" /* 0x%04x --> 0x%04x */", saved_v & mask, v);

	return(1);
}

static int _core_inst_nop(core_p core) { return(1); }

static int _core_inst_not(core_p core)
{
	const unsigned r = mlBFEXT(IR, 3, 0);
	const unsigned is_byte = r & 1;

	const unsigned mask = is_byte ? 0xff : 0xffff;
	const unsigned sign = is_byte ? 0x80 : 0x8000;

	const uint16_t saved_v = _gpr(core, r, 0);
	const uint16_t v = ~saved_v & mask;
	_gpr(core, r, &v);
	
	BMAS(rGPR(FLAGS), _ALU_FLAG_N, !!(v & sign));
	BMAS(rGPR(FLAGS), _ALU_FLAG_Z, (0 == v));
	
	CORE_TRACE_START("exc %s", reg_name(r));
	CORE_TRACE_END(" /* 0x%04x --> 0x%04x */", saved_v & mask, v);

	return(1);
}

static int _core_inst_orr_db(core_p core)
{ return(_core_inst_aluop_db(core, _alu_orr)); }

static int _core_inst_orr_ds(core_p core)
{ return(_core_inst_aluop_ds(core, _alu_orr)); }

static int _core_inst_orr_dw(core_p core)
{ return(_core_inst_aluop_dw(core, _alu_orr)); }

static int _core_inst_out_rp(core_p core)
{
	const unsigned p = mlBFEXT(IR, 7, 0);
	const unsigned r = mlBFEXT(IR, 15, 12);

	const uint16_t out = _gpr(core, r, 0);
	io_buss(core->xxx->io_buss, p, &out);

	CORE_TRACE_START("out 0x%04x, %s", p, reg_name(r));
	CORE_TRACE_END(" /* [0x%02x] <-- 0x%04x */", p, out);

	return(1);
}

static unsigned _core_inst_pop__r(core_p core, unsigned r, unsigned is_byte)
{
	const uint16_t saved_sp = SP;

	uint16_t data = __pop(core, 2 - is_byte);
	_gpr(core, r, &data);

	CORE_TRACE_START("pop %s", reg_name(r));
	CORE_TRACE_END(" /* [SP:(0x%04x)] --> 0x%04x */", saved_sp, data);

	return(data);
}

static int _core_inst_pop_all(core_p core)
{
	const unsigned __popall_list[] = {
		rBP, rY, rX, rD, rC, rB, rA, rSP
	};

	const unsigned* list = __popall_list;

	while(list && (~0U != *list))
		GPRx(*list++) = __pop(core, 2);

	CORE_TRACE("popall");

	return(1);
}

static int _core_inst_pop_mm(core_p core)
{
	unsigned mmap = _core_inst_pop__r(core, rMM, 0);
	memory_map(core->memory, &mmap);

	return(1);
}

static int _core_inst_pop_r(core_p core)
{
	const unsigned r = mlBFEXT(IR, 3, 0);
	const unsigned is_byte = r & 1;

	_core_inst_pop__r(core, r, is_byte);

	return(1);
}

static int _core_inst_popf(core_p core)
{
	uint16_t flags = _core_inst_pop__r(core, rFLAGS, 1);
	_gpr(core, rFLAGS, &flags);

	return(1);
}

static int _core_inst_push__r(core_p core, unsigned r, unsigned is_byte)
{
	const uint16_t data = _gpr(core, r, 0);
	__push(core, 2 - is_byte, data);

	CORE_TRACE_START("push %s", reg_name(r));
	CORE_TRACE_END(" /* [SP:(0x%04x)] <-- 0x%04x */", SP, data);

	return(1);
}

static int _core_inst_push_all(core_p core)
{
	const unsigned __pushall_list[] = {
		rSP, rA, rB, rC, rD, rX, rY, rBP, ~0U,
	};

	const unsigned* list = __pushall_list;

	while(list && (~0U != *list))
		__push(core, 2, *list++);

	CORE_TRACE("pushall");

	return(1);
}

static int _core_inst_push_mm(core_p core)
{
	return(_core_inst_push__r(core, rMM, 0));
}

static int _core_inst_push_r(core_p core)
{
	const unsigned r = mlBFEXT(IR, 3, 0);
	const unsigned is_byte = r & 1;

	return(_core_inst_push__r(core, r, is_byte));
}

static int _core_inst_pushf(core_p core)
{
	return(_core_inst_push__r(core, rFLAGS, 1));
}

static int _core_inst_ret(core_p core)
{
	const uint16_t saved_sp = SP;
	const uint16_t new_pc = __pop(core, 2);

	CORE_TRACE_START("ret");
	CORE_TRACE_END(" /* [SP:(0x%04x)] --> 0x%04x */", saved_sp, new_pc);

	PC = new_pc;

	return(1);
}

static int _core_inst_sdiv_ds(core_p core)
{ return(_core_inst_aluop_ds(core, _alu_sdiv)); }

static int _core_inst_set_f(core_p core)
{
	const unsigned bit = mlBFEXT(IR, 2, 0);

	const unsigned flags_saved = rGPR(FLAGS);
	BSET(rGPR(FLAGS), bit);

	CORE_TRACE_START("set", flag_name(bit));
	CORE_TRACE_END(" /* 0x%04x --> 0x%04x */", flags_saved, rGPR(FLAGS));

	return(1);
}

static int _core_inst_shl_ds(core_p core)
{ return(_core_inst_aluop_ds(core, _alu_shl)); }

static int _core_inst_shl_rn(core_p core)
{ return(_core_inst_aluop_rn(core, _alu_shl)); }

static int _core_inst_shr_ds(core_p core)
{ return(_core_inst_aluop_ds(core, _alu_shr)); }

static int _core_inst_shr_rn(core_p core)
{ return(_core_inst_aluop_rn(core, _alu_shr)); }

static int _core_inst_smod_ds(core_p core)
{ return(_core_inst_aluop_ds(core, _alu_smod)); }

static int _core_inst_smul_ds(core_p core)
{ return(_core_inst_aluop_ds(core, _alu_smul)); }

static int _core_inst_st(core_p core)
{
	inst_args_t _args;
	inst_args_p args = _core_inst_decode_ldst_st(core, &_args);
	
	const unsigned is_byte = arg(s) & 1;
	
	CORE_TRACE_START("st [");
	const unsigned address = __calc_address(core, args, arg(a), 0);
	_CORE_TRACE_("], %s", reg_name(arg(s)));

	const uint16_t write = _gpr(core, arg(s), 0);

	if(is_byte) {
		_CORE_TRACE_(" /* [0x%08x] --> 0x%02x */", address, write & 0xff);
		memory_write_byte(core->memory, address, write);
	} else {
		_CORE_TRACE_(" /* [0x%08x] --> 0x%04x */", address, write);
		memory_write_word(core->memory, address, write);
	}

	CORE_TRACE_END("");

	if(0) {
		CORE_TRACE_START("st 0x%08x", arg(a));
		_CORE_TRACE_(", %s", reg_name(arg(s)));
		_CORE_TRACE_(", x:%01u", arg_flag(x));
		_CORE_TRACE_(", y:%01u", arg_flag(y));
		_CORE_TRACE_(", m:%02u", arg(m));
		CORE_TRACE_END(", i:%01u", arg_flag(i));
	}

	return(1);
}

static int _core_inst_sub_db(core_p core)
{ return(_core_inst_aluop_db(core, _alu_sub)); }

static int _core_inst_sub_ds(core_p core)
{ return(_core_inst_aluop_ds(core, _alu_sub)); }

static int _core_inst_sub_dw(core_p core)
{ return(_core_inst_aluop_dw(core, _alu_sub)); }

static int _core_inst_swap_ds(core_p core)
{
	const unsigned d = mlBFEXT(IR, 7, 4);
	const unsigned s = mlBFEXT(IR, 3, 0);

	const uint16_t saved_d = _gpr(core, d, 0);
	const uint16_t saved_s = _gpr(core, s, 0);
	_gpr(core, d, &saved_s);
	_gpr(core, s, &saved_d);

	CORE_TRACE_START("swap %s, %s", reg_name(d), reg_name(s));
	CORE_TRACE_END(" /* 0x%04x <--> 0x%04x */", saved_d, saved_s);

	return(1);
}

static int _core_inst_test_db(core_p core)
{ LOG_ACTION(exit(-1)); } // TODO: not implimented

static int _core_inst_test_ds(core_p core)
{ LOG_ACTION(exit(-1)); } // TODO: not implimented

static int _core_inst_test_dw(core_p core)
{ LOG_ACTION(exit(-1)); } // TODO: not implimented

static int _core_inst_trap_b(core_p core)
{ LOG_ACTION(exit(-1)); } // TODO: not implimented

static int _core_inst_trap_r(core_p core)
{ LOG_ACTION(exit(-1)); } // TODO: not implimented

static int _core_inst_wait(core_p core)
{
	const unsigned b = mlBFEXT(IR, 7, 0);
	
	BSET(rGPR(FLAGS), _STATUS_FLAG_SINGLE_STEP);

	CORE_TRACE("FIXME: wait #0x%02x", b);

	return(1);
}

static int _core_inst_xor_db(core_p core)
{ return(_core_inst_aluop_db(core, _alu_xor)); }

static int _core_inst_xor_ds(core_p core)
{ return(_core_inst_aluop_ds(core, _alu_xor)); }

static int _core_inst_xor_dw(core_p core)
{ return(_core_inst_aluop_dw(core, _alu_xor)); }

/* **** */

core_p core_alloc(main_p xxx, core_h h2core)
{
	TRACE_ALLOC();

	ERR_NULL(xxx);
	ERR_NULL(h2core);

	/* **** */

	core_p core = handle_calloc((void*)h2core, 1, sizeof(core_t));
	ERR_NULL(core);

	core->xxx = xxx;
	core->trace = 1;

	/* **** */

	main_callback_atexit(xxx, &core->atexit, _core_atexit, h2core);
	main_callback_atreset(xxx, &core->atreset, _core_atreset, core);

	/* **** */
	return(core);
}

uint16_t core_gpr(core_p core, unsigned r, unsigned* write)
{
	const uint16_t data = write ? *write : 0;
	const uint16_t* p2data = write ? &data : 0;

	return(_gpr(core, r, p2data));
}

void core_init(core_p core)
{
	TRACE_INIT();

	ERR_NULL(core);

	ERR_NULL(core->xxx);
	main_p xxx = core->xxx;

	ERR_NULL(xxx->memory);
	core->memory = xxx->memory;
}

uint16_t core_ip(core_p core, unsigned* write)
{
	return(mem_16_access(&IP, write));
}

unsigned core_ir(core_p core)
{
	return(mem_access(&IR, sizeof(IR), 0));
}

typedef int (*core_inst_fn)(core_p core);

static int core_step_return(core_p core, core_inst_fn fn)
{
	PC = rGPR(MP);
	return(fn(core));
}

static int core_step4(core_p core, uint8_t op, uint8_t p1)
{
	_ifetch(core);

	switch(op & 0xf0) {
		case 0x10:
			if((0 == (op & 1)) && (0 == p1))
				return(core_step_return(core, _core_inst_ld_dw));
			else if(0 != p1)
				return(core_step_return(core, _core_inst_ld));
		break;
		case 0x20:
			return(core_step_return(core, _core_inst_st));
		case 0x80:
			if(0 == (p1 & 1))
				return(core_step_return(core, _core_inst_loop_r));
		break;
		case 0x90:
			if(0 == (p1 & 1))
				return(core_step_return(core, _core_inst_br_call_f));
		break;
	}

	CORE_TRACE("op: 0x%08x, op[1]: %01u, p1: 0x%08x, p1[1]: %01u", op, op & 1, p1, p1 & 1);

	return(0);
}

static int core_step3(core_p core, uint8_t op, uint8_t p1)
{
	_ifetch(core);

	switch(op) {
		case 0x30:
			return(core_step_return(core, _core_inst_in_rp));
		case 0x31:
			return(core_step_return(core, _core_inst_out_rp));
	}

	switch(op & 0xf9) {
		case 0x48:
			return(core_step_return(core, _core_inst_add_dw));
		case 0x50:
			return(core_step_return(core, _core_inst_sub_dw));
		case 0x58:
			return(core_step_return(core, _core_inst_cmp_dw));
		case 0x60:
			return(core_step_return(core, _core_inst_and_dw));
		case 0x68:
			return(core_step_return(core, _core_inst_orr_dw));
		case 0x70:
			return(core_step_return(core, _core_inst_test_dw));
		case 0x78:
			return(core_step_return(core, _core_inst_xor_dw));
	}

	switch(op & 0xf0) {
		case 0x80:
			if(1 == (p1 & 1))
				return(core_step_return(core, _core_inst_loops_r));
		break;
		case 0x90:
			if(1 == (p1 & 1))
				return(core_step_return(core, _core_inst_brs_calls_f));
		break;
	}

	switch(op & 0xf1) {
		case 0x11:
			if(0 == p1)
				return(core_step_return(core, _core_inst_ld_db));
		break;
	}

	return(core_step4(core, op, p1));
}

static int core_step2(core_p core, uint8_t op)
{
	uint8_t p1 = _ifetch(core);

	switch(op) {
		case 0x01:
			return(core_step_return(core, _core_inst_add_ds));
		case 0x02:
			return(core_step_return(core, _core_inst_sub_ds));
		case 0x03:
			return(core_step_return(core, _core_inst_cmp_ds));
		case 0x04:
			return(core_step_return(core, _core_inst_and_ds));
		case 0x05:
			return(core_step_return(core, _core_inst_orr_ds));
		case 0x06:
			return(core_step_return(core, _core_inst_test_ds));
		case 0x07:
			return(core_step_return(core, _core_inst_xor_ds));
		case 0x08:
			return(core_step_return(core, _core_inst_trap_b));
		case 0x09:
			switch(p1 & 0xf0) {
				case 0x00:
					return(core_step_return(core, _core_inst_not));
				case 0x01:
					return(core_step_return(core, _core_inst_neg));
				case 0x20:
					return(core_step_return(core, _core_inst_exc));
			}
		break;
		case 0x0a:
			return(core_step_return(core, _core_inst_shl_rn));
		case 0x0b:
			return(core_step_return(core, _core_inst_shl_ds));
		case 0x0c:
			return(core_step_return(core, _core_inst_shr_rn));
		case 0x0d:
			return(core_step_return(core, _core_inst_shr_ds));
		case 0x0e:
			return(core_step_return(core, _core_inst_swap_ds));
		case 0x0f:
			return(core_step_return(core, _core_inst_mov_ds));
		case 0xa8:
			return(core_step_return(core, _core_inst_mul_ds));
		case 0xa9:
			return(core_step_return(core, _core_inst_div_ds));
		case 0xaa:
			return(core_step_return(core, _core_inst_mod_ds));
		case 0xab:
			return(core_step_return(core, _core_inst_smul_ds));
		case 0xac:
			return(core_step_return(core, _core_inst_sdiv_ds));
		case 0xad:
			return(core_step_return(core, _core_inst_smod_ds));
		case 0xaf:
			return(core_step_return(core, _core_inst_wait));
	}

	switch(op & 0xf9) {
		case 0x38:
			return(core_step_return(core, _core_inst_enter_n));
		case 0x39:
			return(core_step_return(core, _core_inst_exit_n));
		case 0x49:
			return(core_step_return(core, _core_inst_add_db));
		case 0x51:
			return(core_step_return(core, _core_inst_sub_db));
		case 0x59:
			return(core_step_return(core, _core_inst_cmp_db));
		case 0x61:
			return(core_step_return(core, _core_inst_and_db));
		case 0x69:
			return(core_step_return(core, _core_inst_orr_db));
		case 0x71:
			return(core_step_return(core, _core_inst_test_db));
		case 0x79:
			return(core_step_return(core, _core_inst_xor_db));
	}

	return(core_step3(core, op, p1));
}

static int core_step1(core_p core)
{
	uint8_t op = _ifetch(core);
	
	switch(op) {
		case 0x00:
			return(core_step_return(core, _core_inst_nop));
		case 0x3e:
			return(core_step_return(core, _core_inst_halt));
		case 0x3f:
			return(core_step_return(core, _core_inst_brk));
		case 0xa0:
			return(core_step_return(core, _core_inst_push_all));
		case 0xa1:
			return(core_step_return(core, _core_inst_pop_all));
		case 0xa2:
			return(core_step_return(core, _core_inst_pushf));
		case 0xa3:
			return(core_step_return(core, _core_inst_popf));
		case 0xa4:
			return(core_step_return(core, _core_inst_push_mm));
		case 0xa5:
			return(core_step_return(core, _core_inst_pop_mm));
		case 0xa7:
			return(core_step_return(core, _core_inst_ret));
	}

	switch(op & 0xf0) {
		case 0xc0:
			return(core_step_return(core, _core_inst_inc_r));
		case 0xd0:
			return(core_step_return(core, _core_inst_dec_r));
		case 0xe0:
			return(core_step_return(core, _core_inst_push_r));
		case 0xf0:
			return(core_step_return(core, _core_inst_pop_r));
	}

	switch(op & 0xf8) {
		case 0xb0:
			return(core_step_return(core, _core_inst_set_f));
		case 0xb8:
			return(core_step_return(core, _core_inst_clr_f));
		case 0x40:
			return(core_step_return(core, _core_inst_trap_r));
	}

	return(core_step2(core, op));
}

int core_step(core_p core)
{
	IP = rGPR(MP) = PC;
	IR = 0;

	switch(core_step1(core)) {
		case 1:
//			PC = rGPR(MP);
		break;
		case 0:
			CORE_TRACE("");
			return(0);
	}

	return(1);
}

unsigned core_trace(core_p core, unsigned trace, unsigned* restore)
{
	const unsigned savedTrace = core->trace;

	if(restore) {
		core->trace = !!(*restore);
	} else if(trace) {
		core->trace = 1;
	}
	
	return(savedTrace);
}
