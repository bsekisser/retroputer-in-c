#pragma once

/* **** */

#include "memory_flags_t.h"

/* **** */

#include "memory_bank.hpp"

/* **** */

#include <cstddef>

/* **** */

const unsigned rom_pages_list[] = { 28, 29, 30, 31, ~0U };

class memory {
	void* _buffer;
	class memory_bank* _pages[32];
	class memory_bank* _pages_map[32];
	int32_t _map;
//
	memory_flags_t _flags;
	class system_buss& _system_buss;
public:
	memory(class system_buss& system_buss_in,
		unsigned page_count = 32, size_t page_size = 0x4000,
		unsigned* rom_pages = (unsigned*)rom_pages_list,
		unsigned shared = false, void* buffer = 0);
};
