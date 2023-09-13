#pragma once

/* **** */

enum {
	COMMANDS_MEMORY_READ_BYTE = 0b00,
	COMMANDS_MEMORY_READ_WORD = 0b01,
	COMMANDS_MEMORY_WRITE_BYTE = 0b10,
	COMMANDS_MEMORY_WRITE_WORD = 0b11,
};

/* **** */

#include "access.h"
#include "buss.hpp"

/* **** */

typedef class system_buss* system_buss_p2c;
typedef class system_buss& system_buss_ref;

class system_buss {
	buss_ref _address_buss;
	buss_ref _command_buss;
	buss_ref _data_buss;
	buss_ref _execute_buss;
	buss_ref _map_buss;
public:
	uint32_t address() { return(_address_buss.value()); }
	class buss& address_buss() { return(_address_buss); }
	uint32_t command() { return(_command_buss.value()); }
	class buss& command_buss() { return(_command_buss); }
	uint32_t data(uint32_t* write = 0) { return(_data_buss.value(write)); }
	class buss& data_buss() { return(_data_buss); }
	class buss& execute_buss() { return(_execute_buss); }
	uint32_t map(uint32_t* write = 0);
	class buss& map_buss() { return(_map_buss); }
	system_buss();
};
