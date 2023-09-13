#pragma once

/* **** */

#include "core.h"
#include "io_buss.h"
#include "memory.h"


/* **** */

#include "callback_qlist.h"

/* **** */

typedef struct main_t {
	core_p core;
	io_buss_p io_buss;
	memory_p memory;
//
	callback_qlist_t atexit_list;
	callback_qlist_t atreset_list;
}main_t;
