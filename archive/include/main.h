#pragma once

/* **** */

typedef struct main_t** main_h;
typedef struct main_t* main_p;

#include "alu.h"
#include "core.h"
#include "kernel.h"
#include "rom.h"
#include "system_buss.h"

/* **** */

#include "callback_qlist.h"

/* **** */

typedef struct main_t {
	alu_p alu;
	core_p core;
	kernel_p kernel;
	rom_p rom;
	system_buss_p system_buss;
//
	struct {
		callback_fn fn;
		void* param;
	}step;
//
	callback_qlist_t atexit_list;
	callback_qlist_t atreset_list;
}main_t;
