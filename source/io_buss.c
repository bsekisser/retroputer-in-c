#include "config.h"
#include "io_buss.h"

/* **** */

#include "core_trace.h"
#include "main.h"

/* **** */

#include "callback_qlist.h"
#include "err.h"
#include "handle.h"
#include "log.h"
#include "unused.h"

/* **** */

typedef struct io_buss_t {
	io_port_p port[256];
//
	callback_qlist_elem_t atexit;
	callback_qlist_elem_t atreset;
	main_p xxx;
	
}io_buss_t;

static int __io_buss_atexit(void* param)
{
	TRACE_ATEXIT();

	handle_free(param);
	return(0);
}

static int __io_buss_atreset(void* param)
{
	io_buss_p iob = param;
	
	return(0);
	UNUSED(iob);
}

io_buss_p io_buss_alloc(main_p xxx, io_buss_h h2iob)
{
	TRACE_ALLOC();

	ERR_NULL(h2iob);
	ERR_NULL(xxx);

	io_buss_p iob = handle_calloc((void*)h2iob, 1, sizeof(io_buss_t));
	ERR_NULL(iob);

	iob->xxx = xxx;

	/* **** */

	main_callback_atexit(xxx, &iob->atexit, __io_buss_atexit, h2iob);
	main_callback_atreset(xxx, &iob->atreset, __io_buss_atreset, iob);

	/* **** */

	/* **** */

	return(iob);
}

void io_buss_init(io_buss_p iob)
{
	TRACE_INIT();
	
	ERR_NULL(iob);
}

uint16_t io_buss(io_buss_p iob, uint8_t p, uint16_t const* write)
{
	io_port_p port = iob->port[p];
	
	if(port && port->fn)
		return(port->fn(port->param, p, write));

	LOG_START("port:[0x%08x]", p);
	if(write)
		_LOG_(" <-- %08x", *write);
	LOG_END();

	return(0);
}
