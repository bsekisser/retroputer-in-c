#include "core.h"

/* **** */

#include "main.h"

/* **** */

#include "callback_qlist.h"

/* **** */

typedef struct core_t {
}core_t;

/* **** */

static int _core_atreset(void* core)
{
	
}

/* **** */

void core_alloc(main_p xxx, core_h h2core)
{
	core_p core = handle_calloc((void*)h2core, 1, sizeof(core_t));
	ERR_NULL(core);
	
	main_atexit_callback(xxx, core_atexit, core);
	main_atreset_callback(xxx, core_atreset, core);
}
