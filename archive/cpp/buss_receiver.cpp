#include "buss_receiver.hpp"

/* **** */

#include <stdint.h>

/* **** */

buss_receiver::buss_receiver(buss_receiver_fn fn, void* param)
{
	this->fn = fn;
	this->param = param;
}

void buss_receiver::add_receiver(buss_receiver* br)
{
	if(next)
		next->add_receiver(br);

	next = br;
}

void buss_receiver::signal(int32_t value)
{
	fn(param, value);

	if(next)
		next->signal(value);
}
