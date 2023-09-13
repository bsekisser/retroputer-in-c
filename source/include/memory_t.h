#pragma once

#include "memory_bank.h"

#include "main.h"

/* **** */

#include "callback_qlist.h"

/* **** */

#include "memory_flags_t.h"

/* **** */

typedef struct memory_t* memory_p;
typedef struct memory_t {
	memory_bank_p page_map[32];
//
	char buffer[Kb(512)];
	unsigned map;
	memory_bank_t pages[32];
//
	memory_flags_t flags;

	callback_qlist_elem_t atexit;
	callback_qlist_elem_t atreset;
	main_p xxx;
}memory_t;
