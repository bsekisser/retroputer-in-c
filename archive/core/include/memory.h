#pragma once

/* **** */

typedef struct memory_t** memory_h;
typedef struct memory_t* memory_p;

#include <stddef.h> // size_t

typedef union memory_flags_t* memory_flags_p;
typedef union memory_flags_t {
	unsigned raw_flags;
	struct {
		unsigned rom:1;
		unsigned shared:1;
	};
}memory_flags_t;

typedef struct memory_params_t* memory_params_p;
typedef struct memory_params_t {
	char* buffer;
	memory_flags_t flags;
	unsigned page_count;
	size_t page_size;
}memory_params_t;

/* **** */

#include "system_buss.h"

#include "main.h"

/* **** */

memory_p memory_alloc(main_p xxx, memory_h h2m, memory_params_p params);
void memory_init(memory_p m, system_buss_p sb);
