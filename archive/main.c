#include "main.h"

/* **** */

#include "alu.h"
#include "kernel.h"

/* **** */

#include "callback_qlist.h"
#include "err.h"

/* **** */

#include <assert.h>

/* **** */

void _main_atexit(main_p xxx)
{
	callback_qlist_process(&xxx->atexit_list);
}

void _main_atreset(main_p xxx)
{
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
	main_p xxx = calloc(1, sizeof(main_t));
	ERR_NULL(xxx);

	/* **** */

	callback_qlist_init(&xxx->atexit_list, LIST_LIFO);
	callback_qlist_init(&xxx->atreset_list, LIST_FIFO);

	/* **** */

	alu_alloc(xxx, &xxx->alu);
//	kernel_alloc(xxx, &xxx->kernel);

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
	alu_init(xxx->alu);
//	kernel_init(xxx->kernel);
}

int main(void)
{
	_main_preflight_checks();

	main_p xxx = main_alloc();
	ERR_NULL(xxx);

	_main_atreset(xxx);

	if(xxx->step.fn)
		xxx->step.fn(xxx->step.param);

	_main_atexit(xxx);
	return(0);
}
