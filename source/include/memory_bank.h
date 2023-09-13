#pragma once

/* **** */

#include "memory_bank_t.h"

/* **** */

#include "mem_access_be.h"

/* **** */

#ifndef _MEMORY_BANK_INLINE_
	#define _MEMORY_BANK_INLINE_ inline
#endif

#ifndef _MEMORY_BANK_STATIC_
	#define _MEMORY_BANK_STATIC_ static
#endif

_MEMORY_BANK_STATIC_ _MEMORY_BANK_INLINE_
void* _memory_bank_data_address(memory_bank_p m, unsigned address)
{
	unsigned offset = address & 0x3fff;
	return(m->data + offset);
}

_MEMORY_BANK_STATIC_ _MEMORY_BANK_INLINE_
char memory_bank_read_byte(memory_bank_p m, unsigned address)
{
	return(*(char*)_memory_bank_data_address(m, address));
}

_MEMORY_BANK_STATIC_ _MEMORY_BANK_INLINE_
short memory_bank_read_word(memory_bank_p m, unsigned address)
{
	return(mem_16be_access(_memory_bank_data_address(m, address), 0));
}

_MEMORY_BANK_STATIC_ _MEMORY_BANK_INLINE_
unsigned memory_bank_read_long(memory_bank_p m, unsigned address)
{
	return(mem_32be_access(_memory_bank_data_address(m, address), 0));
}

_MEMORY_BANK_STATIC_ _MEMORY_BANK_INLINE_
void memory_bank_write_byte(memory_bank_p m, unsigned address, char byte, unsigned override)
{
	if(!m->flags.rom || override)
		*(char*)_memory_bank_data_address(m, address) = (byte & 0xff);
}

_MEMORY_BANK_STATIC_ _MEMORY_BANK_INLINE_
void memory_bank_write_word(memory_bank_p m, unsigned address, short word, unsigned override)
{
	unsigned data = word;
	
	if(!m->flags.rom || override)
		mem_16be_access(_memory_bank_data_address(m, address), &data);
}
