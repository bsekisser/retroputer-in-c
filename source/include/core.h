#pragma once

/* **** */

typedef struct core_t** core_h;
typedef struct core_t* core_p;

static inline char const* flag_name(unsigned bit) {
	const char* flag_name_list[] = {
		"ZERO", "OVERFLOW", "CARRY", "NEGATIVE",
		"SINGLE_STEP", "INTERRUPT_SERVICE",
		"INTERRUPT_DISABLE", "EXCEPTION"
	};

	return(flag_name_list[bit]);
}

/* **** */

#include "main.h"

/* **** */

#include <stdint.h>

/* **** */

core_p core_alloc(main_p xxx, core_h h2core);
uint16_t core_gpr(core_p core, unsigned r, unsigned* write);
void core_init(core_p core);
uint16_t core_ip(core_p core, unsigned* write);
unsigned core_ir(core_p core);
void core_jump(core_p core, unsigned address);
int core_step(core_p core);
unsigned core_trace(core_p core, unsigned trace, unsigned* restore);
