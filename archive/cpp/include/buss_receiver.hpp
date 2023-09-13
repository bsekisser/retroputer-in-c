#pragma once

/* **** */

#include <cstdint>

/* **** */

typedef int (*buss_receiver_fn)(void* param, unsigned value);

typedef class buss_receiver* buss_receiver_p2c;
class buss_receiver {
	buss_receiver_p2c next;
	buss_receiver_fn fn;
	void* param;
public:
	buss_receiver(buss_receiver_fn fn, void* param);
	void add_receiver(buss_receiver_p2c br);
	void add_receiver(buss_receiver_fn fn, void* param);
	void signal(int32_t value);
};

void buss_receiver::add_receiver(buss_receiver_fn fn, void* param)
{
	add_receiver(new buss_receiver(fn, param));
}
