#include "memory_bank.hpp"

/* **** */

#include "memory_flags_t.h"

/* **** */

#include "memory_utility.h"
#include "system_buss.hpp"

/* **** */

#include <cstdint>

/* **** */

static int _memory_bank_signaled(void* param, uint32_t value)
{
	class memory_bank& self = (class memory_bank&)param;

	return(self.signaled(value));
}

memory_bank::memory_bank(class system_buss& system_buss_in,
	uint32_t address, size_t size,
	memory_flags_p flags,
	void* buffer):
		_size(size),
		_starting_address(address),
		_system_buss(system_buss_in)
{
	_buffer = buffer ?: array_buffer(size);
	_data = uint_8_array(_buffer, buffer ? address : 0, size);

	_ending_address = address + size - 1;

	_signaled_receiver = new buss_receiver(_memory_bank_signaled, this);
	system_buss_in.execute_buss().add_receiver(_signaled_receiver);
}

int memory_bank::signaled(uint32_t value)
{
	uint32_t address = _system_buss.address();
	const uint32_t command = _system_buss.command();
	
	if(address < _starting_address)
		return(0);
	if(address > _ending_address)
		return(0);
	
	uint32_t data = 0;

	switch(command) {
		case COMMANDS_MEMORY_READ_WORD:
			data = (data << 8) | read(address++);
			__attribute__((fallthrough));
		case COMMANDS_MEMORY_READ_BYTE:
			data = (data << 8) | read(address++);
		break;
		case COMMANDS_MEMORY_WRITE_BYTE:
		case COMMANDS_MEMORY_WRITE_WORD:
			data = _system_buss.data();
		break;
	}

	switch(command) {
		case COMMANDS_MEMORY_READ_BYTE:
		case COMMANDS_MEMORY_READ_WORD:
			_system_buss.data(&data);
		break;
		case COMMANDS_MEMORY_WRITE_WORD:
			write(address++, (data >> 8) & 0xff);
			__attribute__((fallthrough));
		case COMMANDS_MEMORY_WRITE_BYTE:
			write(address++, data & 0xff);
		break;
	}

	return(1);
}

void memory_bank::write(uint32_t address, uint8_t value, unsigned override)
{
	if(!_flags.rom || override)
		*_data_offset_for_address(_data, address) = value;
}
