#pragma once

/* **** */

#include "access.h"
#include "buss_receiver.hpp"

/* **** */

#include <cstddef>
#include <cstdint>

/* **** */

typedef class buss* buss_p2c;
typedef class buss& buss_ref;

class buss {
	uint32_t _data = 0;
	unsigned _mask;
	uint32_t _value = 0;
	size_t _size;
	class buss_receiver* receivers;
public:
	void add_receiver(class buss_receiver* br);
	void add_receiver(buss_receiver_fn fn, void* param);
//	buss();
	buss(size_t size = 2, unsigned mask = 0xffffffff);
	uint32_t data(uint32_t* write = 0);
	unsigned mask(unsigned* write = 0);
//	const buss& operator =(const buss* cb);
	void signal(uint32_t* write, unsigned hold);
	uint32_t value(uint32_t* write = 0);
};

/* **** */

void buss::add_receiver(buss_receiver_fn fn, void* param)
{
	add_receiver(new buss_receiver(fn, param));
}

uint32_t buss::data(uint32_t* write)
{
	return(generic_access(&_data, write));
}

unsigned buss::mask(unsigned* write)
{
	return(generic_access(&_mask, write));
}

uint32_t buss::value(uint32_t* write)
{
	int32_t data = (int32_t)(write ? *write : _value);

	if(write) {
		data = ((data < 0) ? ((~data) + 1) : data);
		_value = (uint32_t)(data & _mask);
	}
		
	return(data);
}

