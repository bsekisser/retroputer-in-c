#include "config.h"
#include "memory.h"

/* **** */

#include "segment.h"
#include "kernel_data.h"

#include "memory_bank.h"

#include "main.h"

/* **** */

#include "bitfield.h"
#include "callback_qlist.h"
#include "err.h"
#include "handle.h"
#include "log.h"
#include "mem_access.h"
#include "mem_access_be.h"

/* **** */

#include <stdint.h>

/* **** */

#ifndef Kb
	#define Kb(_x) ((_x) << 10)
#endif

#ifndef Mb
	#define Mb(_x) Kb(Kb(_x))
#endif

/* **** */

#include "memory_t.h"

/* **** */

int __memory_atexit(void* param)
{
	TRACE_ATEXIT();
	
	handle_free(param);
	return(0);
}

int __memory_atreset(void* param)
{
	memory_p m = param;

	return(0);
}

/* **** */

unsigned memory_address_page(unsigned address)
{ return(pbBFEXT(address, 14, 5)); }

unsigned memory_address_page_mapped(memory_p m, unsigned page)
{
	switch(page) {
		case 1:
			return(pbBFEXT(m->map, 0, 5));
			break;
		case 2:
			return(pbBFEXT(m->map, 5, 5));
			break;
		case 3:
			return(pbBFEXT(m->map, 10, 5));
			break;
	}

	return(page);
}

memory_p memory_alloc(main_p xxx, memory_h h2m)
{
	TRACE_ALLOC();

	ERR_NULL(xxx);
	ERR_NULL(h2m);

	/* **** */
	
	memory_p m = handle_calloc((void*)h2m, 1, sizeof(memory_t));
	ERR_NULL(m);
	
	m->xxx = xxx;

	/* **** */
	
	main_callback_atexit(xxx, &m->atexit, __memory_atexit, h2m);
	main_callback_atreset(xxx, &m->atexit, __memory_atreset, m);
	
	/* **** setup memory banks and map */
	
	for(unsigned i = 0; i < 32; i++) {
		memory_bank_p  mb = &m->pages[i];
		mb->data = &m->buffer[i << 14];
		mb->flags.rom = ((i >= 28) && (i <= 31));

		m->page_map[i] = mb;
	}

	/* **** load rom */

	memory_load_rom(m, kernel_segment_list);

	/* **** */

	return(m);
}

memory_bank_p memory_bank_for_address(memory_p m, unsigned address)
{
	unsigned page = memory_address_page(address);

	if(0) {
		LOG_START("address: 0x%08x", address);
		LOG_END(", page: 0x%08x", page);
	}

	if(0) {
		switch(page) {
		case 1:
			page = pbBFEXT(m->map, 0, 5);
			break;
		case 2:
			page = pbBFEXT(m->map, 5, 5);
			break;
		case 3:
			page = pbBFEXT(m->map, 10, 5);
			break;
		default:
			break;
		}

		return(&m->pages[page]);
	}

	return(m->page_map[page & 0b11111]);
}

void memory_init(memory_p m)
{
	TRACE_INIT();
}

void memory_load_rom(memory_p m, segment_p segment)
{
	if(0) { LOG("segment: 0x%08" PRIxPTR, (uintptr_t)segment); }

	while(segment && segment->len && segment->data) {
		if(0) {
			LOG_START("segment: 0x%08" PRIxPTR, (uintptr_t)segment);
			_LOG_(", address: 0x%08x", segment->address);
			_LOG_(", data: 0x%08" PRIxPTR, (uintptr_t)segment->data);
			LOG_END(", len: 0x%08zx", segment->len);
		}

		char* dst = &m->buffer[segment->address];
		char* src = (char*)segment->data;
		char* end = &dst[segment->len];

		assert(dst >= m->buffer);
		assert(end <= &m->buffer[sizeof(m->buffer)]);

		while(dst <= end)
			*dst++ = *src++;

		segment++;
	}
}

unsigned memory_map(memory_p m, unsigned const* write)
{
	if(write) {
		const unsigned map = *write;

		const unsigned page_1 = pbBFEXT(map, 0, 5);
		m->page_map[1] = &m->pages[page_1];

		const unsigned page_2 = pbBFEXT(map, 5, 5);
		m->page_map[2] = &m->pages[page_2];

		const unsigned page_3 = pbBFEXT(map, 10, 5);
		m->page_map[3] = &m->pages[page_3];

		if(0) {
			LOG("map: 0x%08x", map);
			LOG("-- 1: 0x%08x", page_1);
			LOG("-- 2: 0x%08x", page_2);
			LOG("-- 3: 0x%08x", page_3);
		}
	}

	return(mem_32_access(&m->map, write));
}

uint8_t memory_read_byte(memory_p m, unsigned address)
{
	memory_bank_p mb = memory_bank_for_address(m, address);

	if(0) {
		LOG_START("memory_bank: 0x%08" PRIxPTR, (uintptr_t)mb);
		LOG_END(", address: 0x%08x", address);
	}

	return(memory_bank_read_byte(mb, address));
}

uint16_t memory_read_word(memory_p m, unsigned address)
{
	memory_bank_p mb = memory_bank_for_address(m, address);
	return(memory_bank_read_word(mb, address));
}

uint32_t memory_read_long(memory_p m, unsigned address)
{
	memory_bank_p mb = memory_bank_for_address(m, address);
	return(memory_bank_read_long(mb, address));
}

unsigned memory_va_to_pa(memory_p m, unsigned address, unsigned* p2page)
{
	const unsigned address_page = memory_address_page(address);
	const unsigned page = memory_address_page_mapped(m, address_page);
	
	if(p2page)
		*p2page = page;

	return((page << 14) | (address & 0x3fff));
}

void memory_write_byte(memory_p m, unsigned address, uint8_t value)
{
	memory_bank_p mb = memory_bank_for_address(m, address);
	memory_bank_write_byte(mb, address, value, m->flags.override);
}

void memory_write_word(memory_p m, unsigned address, uint16_t value)
{
	memory_bank_p mb = memory_bank_for_address(m, address);
	memory_bank_write_word(mb, address, value, m->flags.override);
}
