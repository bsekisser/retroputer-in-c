#include "system_buss.hpp"
#include "buss.hpp"

/* **** */

#include <cstdint>

/* **** */

#define ADDRESS_MASK 0x7ffffU
#define ADDRESS_SIZE 4

#define COMMAND_MASK 3
#define COMMAND_SIZE 1

#define DATA_MASK 0xffffU
#define DATA_SIZE 2

#define EXECUTE_MASK 1
#define EXECUTE_SIZE 1

#define MAP_MASK 0xffffU
#define MAP_SIZE 2

/* **** */

uint32_t system_buss::map(uint32_t* write)
{
	if(write) {
		uint32_t data = *write & MAP_MASK;
		return(_map_buss.value(&data));
	}

	return(_map_buss.value(0));
}

system_buss::system_buss():
	_address_buss(*(new buss(ADDRESS_SIZE, ADDRESS_MASK))),
	_command_buss(*(new buss(COMMAND_SIZE, COMMAND_MASK))),
	_data_buss(*(new buss(DATA_SIZE, DATA_MASK))),
	_execute_buss(*(new buss(EXECUTE_SIZE, EXECUTE_MASK))),
	_map_buss(*(new buss(MAP_SIZE, MAP_MASK)))
{}
