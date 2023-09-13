#pragma once

/* **** */

typedef struct memory_bank_t** memory_bank_h;
typedef struct memory_bank_t* memory_bank_p;

typedef union memory_bank_flags_t* memory_bank_flags_p;
typedef union memory_bank_flags_t {
	unsigned raw_flags;
	struct {
		unsigned rom:1;
		unsigned shared:1;
	};
}memory_bank_flags_t;

#include <stddef.h> // size_t

typedef struct memory_bank_params_t* memory_bank_params_p;
typedef struct memory_bank_params_t {
	unsigned address;
	char* buffer;
	memory_bank_flags_t flags;
	size_t size;
}memory_bank_params_t;

/* **** */

#include "system_buss.h"

#include "main.h"

/* **** */

memory_bank_p memory_bank_alloc(main_p xxx, memory_bank_h h2m, memory_bank_params_p mbp);
void memory_bank_init(memory_bank_p m, system_buss_p sb);
unsigned memory_bank_read_byte(memory_bank_p m, unsigned address);
unsigned memory_bank_read_word(memory_bank_p m, unsigned address);
void memory_bank_write_byte(memory_bank_p m, unsigned address, unsigned value, unsigned override);
void memory_bank_write_word(memory_bank_p m, unsigned address, unsigned value, unsigned override);
