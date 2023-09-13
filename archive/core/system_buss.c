#include "config.h"
#include "system_buss.h"

/* **** */

#include "buss.h"
#include "main.h"

/* **** */

#include "err.h"
#include "handle.h"
#include "log.h"

/* **** */

static const unsigned MAP_SIZE = 2;
static const unsigned MAP_MASK = 0b1111111111111111; // 16-bit memory mask
static const unsigned ADDRESS_SIZE = 4;
static const unsigned ADDRESS_MASK = 0b1111111111111111111; // 19-bit address bus
static const unsigned DATA_SIZE = 2;
static const unsigned DATA_MASK = 0b1111111111111111;
static const unsigned COMMAND_SIZE = 1;
static const unsigned COMMAND_MASK = 0b11;
static const unsigned EXECUTE_SIZE = 1;
static const unsigned EXECUTE_MASK = 0b1;

typedef struct system_buss_t {
	buss_p address_buss;
	buss_p command_buss;
	buss_p data_buss;
	buss_p execute_buss;
	buss_p map_buss;
//
	main_p xxx;
}system_buss_t;

/* **** */

static unsigned _system_buss_access(system_buss_p sb,
	unsigned address, unsigned command, unsigned* write)
{
	unsigned data = write ? *write : 0;
	unsigned signal = 1;
	
	buss_value(sb->address_buss, &address);
	buss_value(sb->data_buss, &data);
	buss_value(sb->command_buss, &command);
	buss_signal(sb->execute_buss, &signal, 0);

	if(write)
		return(data);

	return(buss_value(sb->data_buss, 0));
}


/* **** */

unsigned system_buss_address(system_buss_p sb)
{
	return(buss_value(sb->address_buss, 0));
}

buss_p system_buss_address_buss(system_buss_p sb)
{
	return(sb->address_buss);
}

system_buss_p system_buss_alloc(main_p xxx, system_buss_h h2sb)
{
	TRACE_ALLOC();

	ERR_NULL(xxx);

	system_buss_p sb = handle_calloc((void*)h2sb, 1, sizeof(system_buss_t));
	ERR_NULL(h2sb);

	/* **** */
	
	buss_alloc(xxx, &sb->address_buss, ADDRESS_SIZE, ADDRESS_MASK);
	buss_alloc(xxx, &sb->command_buss, COMMAND_SIZE, COMMAND_MASK);
	buss_alloc(xxx, &sb->data_buss, DATA_SIZE, DATA_MASK);
	buss_alloc(xxx, &sb->execute_buss, EXECUTE_SIZE, EXECUTE_MASK);
	buss_alloc(xxx, &sb->map_buss, MAP_SIZE, MAP_MASK);

	/* **** */

	return(sb);
}

unsigned system_buss_command(system_buss_p sb)
{
	return(buss_value(sb->command_buss, 0));
}

buss_p system_buss_command_buss(system_buss_p sb)
{
	return(sb->command_buss);
}

unsigned system_buss_data(system_buss_p sb, unsigned* value)
{
	return(buss_value(sb->data_buss, value));
}

buss_p system_buss_data_buss(system_buss_p sb)
{
	return(sb->data_buss);
}

buss_p system_buss_execute_buss(system_buss_p sb)
{
	return(sb->execute_buss);
}

void system_buss_init(system_buss_p sb)
{
	TRACE_INIT();
	
	buss_init(sb->address_buss);
	buss_init(sb->command_buss);
	buss_init(sb->data_buss);
	buss_init(sb->execute_buss);
	buss_init(sb->map_buss);
}

unsigned system_buss_map(system_buss_p sb, unsigned* value)
{
	unsigned masked_value = (value ? *value : 0) & MAP_MASK;

	if(value) {
		buss_value(sb->map_buss, &masked_value);
		return(0);
	}

	return(buss_value(sb->map_buss, 0));
}

buss_p system_buss_map_buss(system_buss_p sb)
{
	return(sb->map_buss);
}

char system_buss_read_byte(system_buss_p sb, unsigned address)
{
	return((char)_system_buss_access(sb, address,
		COMMANDS_MEMORY_READ_BYTE, 0));
}

short system_buss_read_word(system_buss_p sb, unsigned address)
{
	return((short)_system_buss_access(sb, address,
		COMMANDS_MEMORY_READ_WORD, 0));
}

void system_buss_write_byte(system_buss_p sb, unsigned address, char value)
{
	unsigned vvalue = value & 0xff;

	_system_buss_access(sb, address, COMMANDS_MEMORY_WRITE_BYTE, &vvalue);
}

void system_buss_write_word(system_buss_p sb, unsigned address, short value)
{
	unsigned vvalue = value & 0xffff;

	_system_buss_access(sb, address, COMMANDS_MEMORY_WRITE_WORD, &vvalue);
}
