#include "config.h"

#include "memory.h"

/* **** */

#include "memory_bank.h"
#include "system_buss.h"

#include "main.h"

/* **** */

#include "err.h"
#include "handle.h"
#include "log.h"

/* **** */

#include <stddef.h> // size_t

/* **** */

#define _PAGE_COUNT 32
#define _PAGE_SIZE 0x4000

typedef struct memory_t {
	memory_bank_p pages[_PAGE_COUNT];
	memory_params_t params;
//
	char* buffer;
}memory_t;

/* **** */

memory_p memory_alloc(main_p xxx, memory_h h2m, memory_params_p params_in)
{
	TRACE_ALLOC();

	const char* buffer = params_in ? params_in->buffer : 0;
	const unsigned page_count = params_in ? params_in->page_count : _PAGE_COUNT;
	const size_t page_size = params_in ? params_in->page_size : _PAGE_SIZE;
	const size_t buffer_size = page_count * page_size;
	const size_t size = buffer ? buffer_size : 0;

	memory_p m = handle_calloc((void*)h2m, 1, sizeof(memory_t) + size);
	ERR_NULL(m);
	
	memcpy(&m->params, params_in, sizeof(memory_params_t));

	for(unsigned i = 0; i < page_count; i++)
	{
		const unsigned address = i * page_size;

		memory_bank_params_t mbp;
		mbp.address = address;
		mbp.buffer = &m->buffer[address];
		mbp.flags.rom = ((i >= 28) && (i <= 31));
		mbp.flags.shared = params_in ? params_in->flags.shared : 0;
		mbp.size = page_size;

		memory_bank_alloc(xxx, &m->pages[i], &mbp);
	}

	/* **** */
	return(m);
}
