#pragma once

/* **** */

#include "buss_receiver.h"

/* **** */

#include <stdint.h>

/* **** */

typedef struct buss_t** buss_h;
typedef struct buss_t* buss_p;
typedef struct buss_t {
//	void* buffer;
	buss_receiver_p receivers;
	size_t size; /* 2 */
	int32_t value;
	uint32_t mask; /* 0xffffffff */
}buss_t;

/* **** */

buss_p buss(buss_h b, size_t* p2size, uint32_t* p2mask);
void buss_add_receiver(buss_p b, buss_receiver_p br);

static inline buss_value(buss_p b, int32_t* write)
{
	int32_t data = write ? *write : b->value;
	
	if(write) {
		data = data < 0 ? (~(data) + 1) : data;
		b->value = data & b->mask;
	}

	return(b->value);
}
