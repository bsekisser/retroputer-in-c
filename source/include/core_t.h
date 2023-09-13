#pragma once

/* **** */

#include "memory.h"

#include "main.h"

/* **** */

#include "callback_qlist.h"

/* **** */

#include <stdint.h>

/* **** */

typedef struct core_inst_t {
	uint16_t ip;
#define IP xCtCIt->ip

	unsigned ir;
#define IR xCtCIt->ir
}core_inst_t;

typedef struct core_t* core_p;
typedef struct core_t {
	uint16_t gpr[_GPR_COUNT];
#define GPRx(_x) core->gpr[_x >> 1]
#define rGPR(_x) GPRx(r##_x)
//
	core_inst_t ctcit;
#define xCtCIt (&core->ctcit)
//
	callback_qlist_elem_t atexit;
	callback_qlist_elem_t atreset;
	memory_p memory;
	main_p xxx;

	union {
		unsigned raw_flags;
		struct {
			unsigned jump:1;
			unsigned trace:1;
		};
	};
}core_t;
