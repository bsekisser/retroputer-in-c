#include "memory.hpp"

/* **** */

#include "memory_flags_t.h"

/* **** */

#include "memory_bank.hpp"
#include "memory_utility.h"
#include "system_buss.hpp"

/* **** */

#include <cstdint>

/* **** */

memory::memory(class system_buss& system_buss_in,
	uint32_t page_count, size_t page_size,
	uint32_t* rom_pages, unsigned shared, void* buffer):
		_system_buss(system_buss_in)
{
	_buffer = buffer ?: array_buffer(page_size * page_count);
	
	for(uint32_t i = 0; i < page_count; i++) {
		memory_flags_t flags;

		flags.rom = ((i >= 28) && (i <= 32));
		flags.shared = shared;

		const uint32_t address = i * page_size;

		_pages[i] = new memory_bank(_system_buss,
			address, page_size,
			&flags, _buffer);
	}
}
