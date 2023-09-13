#include "config.h"
#include "core_trace.h"

/* **** */

#include "core.h"
#include "core_flags.h"
#include "memory.h"
#include "register_file.h"

/* **** */

#include "core_t.h"

/* **** */

#include "bitfield.h"

/* **** */

#include "stdarg.h"

/* **** */

static void __core_trace(const char* format, va_list args)
{
	vprintf(format, args);
}

static void __core_trace_end(core_p core)
{
	printf("\n");
}

static void __core_trace_start(core_p core)
{
	char flags[5], *dst = flags;
	
	const unsigned fflags = core_gpr(core, rFLAGS, 0);

	*dst++ = BEXT(fflags, _ALU_FLAG_C) ? 'C' : 'c';
	*dst++ = BEXT(fflags, _ALU_FLAG_N) ? 'N' : 'n';
	*dst++ = BEXT(fflags, _ALU_FLAG_V) ? 'V' : 'v';
	*dst++ = BEXT(fflags, _ALU_FLAG_Z) ? 'Z' : 'z';
	*dst = 0;

	uint16_t ip = core_ip(core, 0);

	const unsigned ppa = memory_va_to_pa(core->memory, ip, 0);

	printf("0x%08x[0x%08x](0x%08x):%s: ",
		core_ip(core, 0), ppa, core_ir(core), flags);
}

/* **** */

void _core_trace_(core_p core, const char* format, ...)
{
	if(!core_trace(core, 0, 0))
		return;

	va_list args;
	va_start(args, format);
	__core_trace(format, args);
	va_end(args);
}

void core_trace_end(core_p core, const char* format, ...)
{
	if(!core_trace(core, 0, 0))
		return;

	va_list args;
	va_start(args, format);
	__core_trace(format, args);
	va_end(args);

	__core_trace_end(core);
}

void core_trace_single(core_p core, const char* format, ...)
{
	if(!core_trace(core, 0, 0))
		return;

	__core_trace_start(core);

	va_list args;
	va_start(args, format);
	__core_trace(format, args);
	va_end(args);

	__core_trace_end(core);
}

void core_trace_start(core_p core, const char* format, ...)
{
	if(!core_trace(core, 0, 0))
		return;

	__core_trace_start(core);

	va_list args;
	va_start(args, format);
	__core_trace(format, args);
	va_end(args);
}
