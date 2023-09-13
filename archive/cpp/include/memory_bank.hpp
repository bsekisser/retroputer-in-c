#pragma once

/* **** */

#include "memory_flags_t.h"

/* *** */

#include "buss_receiver.hpp"
#include "system_buss.hpp"

/* **** */ 

#include <cstddef>

/* **** */

static inline uint8_t* _data_offset_for_address(uint8_t* data, uint32_t address)
{
	return(data + (address & 0x3fff));
}

typedef class memory_bank* memory_bank_p2c;
class memory_bank {
	void* _buffer = 0;
	uint8_t* _data = 0;
	uint32_t _ending_address;
	size_t _size = 0;
	uint32_t _starting_address = 0;
//
	memory_flags_t _flags;
	class buss_receiver* _signaled_receiver;
	class system_buss& _system_buss;
//
public:
	memory_bank(class system_buss& system_buss_in,
		unsigned address, size_t size,
		memory_flags_p flags,
		void* buffer);
	uint8_t read(uint32_t address) { return(*_data_offset_for_address(_data, address)); }
	void write(uint32_t address, uint8_t value, unsigned override = 0);
	int signaled(unsigned value);
};
