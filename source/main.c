#include "config.h"
#include "main.h"

/* **** */

#include "core.h"
#include "memory.h"

/* **** */

#include "callback_qlist.h"
#include "err.h"

/* **** */

#include <assert.h>

/* **** */

#include "main_t.h"

/* **** */

void _main_atexit(main_p xxx)
{
	TRACE_ATEXIT();

	callback_qlist_process(&xxx->atexit_list);
}

void _main_atreset(main_p xxx)
{
	TRACE_ATRESET();

	callback_qlist_process(&xxx->atreset_list);
}

void _main_preflight_checks(void)
{
	assert(2 == sizeof(short));
	assert(4 == sizeof(unsigned));
	assert(sizeof(int) == sizeof(unsigned));
}

/* **** */

main_p main_alloc(void)
{
	TRACE_ALLOC();

	main_p xxx = calloc(1, sizeof(main_t));
	ERR_NULL(xxx);

	/* **** */

	callback_qlist_init(&xxx->atexit_list, LIST_LIFO);
	callback_qlist_init(&xxx->atreset_list, LIST_FIFO);

	/* **** */

	core_alloc(xxx, &xxx->core);
	io_buss_alloc(xxx, &xxx->io_buss);
	memory_alloc(xxx, &xxx->memory);

	/* **** */
	return(xxx);
}

void main_callback_atexit(main_p xxx, callback_qlist_elem_p cble, callback_fn cbf, void* cbp)
{
	callback_qlist_setup_and_register_callback(&xxx->atexit_list, cble, cbf, cbp);
}

void main_callback_atreset(main_p xxx, callback_qlist_elem_p cble, callback_fn cbf, void* cbp)
{
	callback_qlist_setup_and_register_callback(&xxx->atreset_list, cble, cbf, cbp);
}

void main_init(main_p xxx)
{
	TRACE_INIT();

	core_init(xxx->core);
	io_buss_init(xxx->io_buss);
	memory_init(xxx->memory);
}

int main(void)
{
	_main_preflight_checks();

	main_p xxx = main_alloc();
	ERR_NULL(xxx);

	main_init(xxx);

	_main_atreset(xxx);

	while(core_step(xxx->core))
		;

//	if(xxx->step.fn)
//		xxx->step.fn(xxx->step.param);

	_main_atexit(xxx);
	return(0);
}
