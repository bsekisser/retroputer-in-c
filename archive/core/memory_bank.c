#include "memory_bank.h"

/* **** */

#include "system_buss.h"

//#include "main.h"

/* **** */

#include "err.h"
#include "handle.h"
#include "mem_access.h"
#include "mem_access_be.h"

/* **** */

typedef struct memory_bank_t {
	unsigned char* data;
	unsigned ending_address;
	unsigned starting_address;
//
	memory_bank_flags_t flags;
//
	system_buss_p system_buss;
	main_p xxx;
}memory_bank_t;

/* **** */

static void* _memory_bank_data_address(memory_bank_p m, unsigned address)
{
	return(&m->data[address & 0x3fff]);
}

void _memory_bank_signaled(memory_bank_p m) {
	const unsigned command = system_buss_command(m->system_buss);
	const unsigned address = system_buss_address(m->system_buss);

	if(address < m->starting_address)
		return;

	if(address > m->ending_address)
		return;

	unsigned data = 0;

	switch(command) {
		default:
		case COMMANDS_MEMORY_READ_BYTE:
			data = memory_bank_read_byte(m, address);
		break;	
		case COMMANDS_MEMORY_READ_WORD:
			data = memory_bank_read_word(m, address);
		break;
		case COMMANDS_MEMORY_WRITE_BYTE:
		case COMMANDS_MEMORY_WRITE_WORD:
			data = system_buss_data(m->system_buss, 0);
		break;
	}

	switch(command) {
		default:
		case COMMANDS_MEMORY_READ_BYTE:
		case COMMANDS_MEMORY_READ_WORD:
			system_buss_data(m->system_buss, &data);
		break;
		case COMMANDS_MEMORY_WRITE_BYTE:
			memory_bank_write_byte(m, address, data, 0);
		break;
		case COMMANDS_MEMORY_WRITE_WORD:
			memory_bank_write_word(m, address, data, 0);
		break;
	}
}

/* **** */

memory_bank_p memory_bank_alloc(main_p xxx, memory_bank_h h2m,
	memory_bank_params_p mbp)
{
	const size_t param_size = mbp->size;
	const size_t size = sizeof(memory_bank_t) + param_size;

	memory_bank_p m = handle_calloc((void*)h2m, 1, size);
	ERR_NULL(m);

	m->xxx = xxx;

	/* **** */
	
	m->starting_address = mbp->address;
	m->ending_address = mbp->address + size;
	m->flags.raw_flags = mbp->flags.raw_flags;

	/* **** */

	return(m);
}

void memory_bank_init(memory_bank_p m, system_buss_p system_buss)
{
	ERR_NULL(system_buss);
	m->system_buss = system_buss;

	/* **** */

	system_buss_execute_add_receiver(system_buss,
		_memory_bank_signaled, m);
}

unsigned memory_bank_read_byte(memory_bank_p m, unsigned address)
{
	return(*(char*)_memory_bank_data_address(m, address));
}

unsigned memory_bank_read_word(memory_bank_p m, unsigned address)
{
	return(mem_16be_access(_memory_bank_data_address(m, address), 0));
}

void memory_bank_write_byte(memory_bank_p m, unsigned address, unsigned value, unsigned override)
{
	if(!m->flags.rom || override)
		*(char*)_memory_bank_data_address(m, address) = (value & 0xff);
}

void memory_bank_write_word(memory_bank_p m, unsigned address, unsigned value, unsigned override)
{
	if(!m->flags.rom || override)
		mem_16be_access(_memory_bank_data_address(m, address), &value);
}
