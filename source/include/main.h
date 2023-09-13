#pragma once

/* **** */

typedef struct main_t** main_h;
typedef struct main_t* main_p;

/* **** */

//#include "core.h"
//#include "io_buss.h"
//#include "memory.h"

/* **** */

#include "callback_qlist.h"

/* **** */

void main_callback_atexit(main_p xxx, callback_qlist_elem_p cble, callback_fn cbf, void* cbp);
void main_callback_atreset(main_p xxx, callback_qlist_elem_p cble, callback_fn cbf, void* cbp);
