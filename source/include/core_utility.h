#pragma once

#include "core_inst_decode.h"
#include "mem_access.h"

/* **** */

typedef struct calc_address_t {
}calc_address_t;

static uint32_t __calc_address(core_p core, inst_args_p args, uint32_t address, unsigned trace)
{
	unsigned saved_trace = core_trace(core, trace, 0);

	if(arg_flag(i)) {
		_CORE_TRACE_("[");
	}

	switch(arg(m)) {
		case 2:
			_CORE_TRACE_("((0x%08x + BP) & 0xffff)", address);
			address += (int16_t)rGPR(BP);
			address &= 0xffff;
		break;
		case 3:
			_CORE_TRACE_("(0x%08x + (D << 3))", address);
			address += (((int32_t)(int16_t)rGPR(D)) << 3);
		break;
		default:
			_CORE_TRACE_("0x%08x", address);
		break;
	}

	if(arg_flag(x)) {
		_CORE_TRACE_(", X");
		address += (int16_t)rGPR(X);
	}

	if(arg_flag(i)) {
		_CORE_TRACE_("]");
		address = memory_read_long(core->memory, address);
	}

	if(arg_flag(y)) {
		_CORE_TRACE_(", Y");
		address += (int16_t)rGPR(Y);
	}

	if(trace) core_trace(core, 0, &saved_trace);
	return(address & 0x7ffff);
}

static unsigned __pop(core_p core, uint8_t count)
{
	unsigned data = 0;
	
	switch(count) {
		case 2:
			data = memory_read_word(core->memory, SP);
		break;
		case 1:
			data = memory_read_byte(core->memory, SP);
		break;
	}
	
	SP += count;
	SP &= 0xffff;

	return(data);
}

static void __push(core_p core, uint8_t count, unsigned data)
{
	SP -= count;
	SP &= 0xffff;

	switch(count) {
		case 2:
			memory_write_word(core->memory, SP, (uint16_t)data);
		break;
		case 1:
			memory_write_byte(core->memory, SP, (uint8_t)data);
		break;
	}
}

/* **** */

typedef struct br_call_t* br_call_p;
typedef struct br_call_t {
	int32_t a;
	union {
		unsigned raw_flags;
		struct {
			unsigned f:3;
			unsigned i:1;
			unsigned m:2;
			unsigned n:1;
			unsigned s:1;
			unsigned u:1;
			unsigned w:1;
			unsigned x:1;
			unsigned y:1;
		};
	};
}br_call_t;

static void _calc_brf(core_p core, inst_args_p args) {
	CORE_TRACE_START("%s%s ", arg_flag(w) ? "call" : "br" , arg_flag(u) ? "" : "f");

	if(1) {
		_CORE_TRACE_("/* n: %01u", arg_flag(n));
		_CORE_TRACE_(", f: %01u", arg(f));
		_CORE_TRACE_(", m: %01u", arg(m));
		_CORE_TRACE_(", i: %01u", arg_flag(i));
		_CORE_TRACE_(", x: %01u", arg_flag(x));
		_CORE_TRACE_(", y: %01u", arg_flag(y));
		_CORE_TRACE_(", u: %01u", arg_flag(u));
		_CORE_TRACE_(", w: %01u", arg_flag(w));
		_CORE_TRACE_(", a: %08x */ ", arg(a));
	}

	if(arg_flag(w))
		__push(core, 2, PC);

	uint32_t addr = arg_flag(s) ? (int8_t)arg(a) : arg(a);

	if(0 == arg(m)) {
		addr += PC;
		addr &= 0xffff;
	}

	addr = __calc_address(core, args, addr, 0);

	if(0 != arg(m))
		addr = memory_read_word(core->memory, addr);

	unsigned flag_value = 0;

	if(0 == arg_flag(u)) {
		const unsigned saved_flags = rGPR(FLAGS) ;
		const unsigned test_flag = BEXT(saved_flags, arg(f));

		flag_value = (arg_flag(n) != test_flag);

		if(0) {
			LOG_START("f: %01u", arg(f));
			_LOG_(", n: %01u", arg_flag(n));
			_LOG_(", saved_flags: 0x%08x", saved_flags);
			_LOG_(", test_flag: 0x%08x", test_flag);
			LOG_END(", flag_value: 0x%08x", flag_value);
		}
	}

//	CORE_TRACE_END(" 0x%08x", addr);
	CORE_TRACE_END("");

	if(arg_flag(u) || flag_value)
		PC = addr;
}

static uint16_t _gpr(core_p core, unsigned r, const uint16_t* write)
{
	const unsigned check_is_byte = (r < _rSPR) || (rSTATUS == r);
	const unsigned is_byte = check_is_byte ? (r & 1) : 0;

	uint16_t* gpr = &core->gpr[r >> 1];
	
	uint16_t data = (write ? *write : *gpr);
	if(is_byte) data = (int8_t)data;

	if(0) {
		LOG_START("r: %02u", r);
		_LOG_(", gpr: 0x%08x", *gpr);
		_LOG_(", data: 0x%08x", data);

		if(write) {
			_LOG_(", write 0x%08x", *write);
		}

		LOG_END();
	}

	if(write)
		*gpr = data;

	return(data);
}

static uint8_t _ifetch(core_p core)
{
	const uint16_t mp = rGPR(MP)++;
	const uint8_t byte = memory_read_byte(core->memory, mp) & 0xff;

	IR <<= 8;
	IR |= byte;

	if(0) {
		LOG_START("MP: 0x%08x", mp);
		_LOG_(", byte: 0x%02x", byte);
		LOG_END(", IR: 0x%08x", IR);
	}

	return(byte);
}
