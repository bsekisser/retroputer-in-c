#pragma once

/* **** */

typedef struct system_buss_t** system_buss_h;
typedef struct system_buss_t* system_buss_p;

/* **** */

#include "buss.h"

/* **** */

enum {
	COMMANDS_MEMORY_READ_BYTE = 0b00,
	COMMANDS_MEMORY_READ_WORD = 0b01,
	COMMANDS_MEMORY_WRITE_BYTE = 0b10,
	COMMANDS_MEMORY_WRITE_WORD = 0b11,
};

/* **** */

unsigned system_buss_address(system_buss_p sb);
system_buss_p system_buss_alloc(main_p xxx, system_buss_h h2sb);
unsigned system_buss_command(system_buss_p sb);
unsigned system_buss_data(system_buss_p sb, unsigned* value);
unsigned system_buss_execute_add_receiver(system_buss_p sb, void* rf, void* rfp);
void system_buss_init(system_buss_p sb);
