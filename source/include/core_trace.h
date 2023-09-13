#pragma once

/* **** */

#include "core.h"

/* **** */

void _core_trace_(core_p core, const char* format, ...);
void core_trace_end(core_p core, const char* format, ...);
void core_trace_single(core_p core, const char* format, ...);
void core_trace_start(core_p core, const char* format, ...);

#define _CORE_TRACE_(_f, ...) \
	_core_trace_(core, _f, ##__VA_ARGS__);

#define CORE_TRACE_START(_f, ...) \
	core_trace_start(core, _f, ##__VA_ARGS__);

#define CORE_TRACE_END(_f, ...) \
	core_trace_end(core, _f, ##__VA_ARGS__);

#define CORE_TRACE(_f, ...) \
	core_trace_single(core, _f, ##__VA_ARGS__);
