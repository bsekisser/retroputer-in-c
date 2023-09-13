#include "system_buss.hpp"

typedef union memory_bank_flags_t {
	unsigned raw_flags;
	struct {
		unsigned rom:1;
		unsigned shared:1;
	};
}memory_bank_flags_t;

class memory_bank {
	unsigned char* data;
	unsigned ending_address;
	unsigned starting_address;
//
	memory_bank_flags_t flags;
//
//	main_p xxx;
//	system_buss_p system_buss;
public:
	memory_bank::memory_bank(system_buss sb, unsigned address = 0,
		unsigned size = 0, unsigned rom = 0, unsigned shared = 0,
			unsigned buffer = 0);
	unsigned memory_bank::read(unsigned address);
};

static void* _memory_bank_data_address(void* data, unsigned address)
{
	return(((char*)data) + (address & 0x3fff));
}

memory_bank::memory_bank(system_buss sb,
	unsigned address = 0,
	unsigned size = 0,
	unsigned rom = 0,
	unsigned shared = 0,
	unsigned buffer = 0)
{
}

unsigned memory_bank::read(unsigned address)
{
	return(*(char*)_memory_bank_data_address(data, address));
}
