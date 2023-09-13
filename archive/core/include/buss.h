#pragma once

/* **** */

#include "main.h"

/* **** */

#include "queue.h"

/* **** */

typedef struct buss_t** buss_h;
typedef struct buss_t* buss_p;

typedef int (*buss_receiver_fn)(void* param, unsigned value);

typedef struct buss_receiver_t** buss_receiver_h;
typedef struct buss_receiver_t* buss_receiver_p;
typedef struct buss_receiver_t {
	buss_receiver_fn fn;
	void* param;
//
	qelem_p receiver_qelem;
}buss_receiver_t;

/* **** */

void buss_add_receiver(buss_p buss, buss_receiver_p br, buss_receiver_fn fn, void* param);
buss_p buss_alloc(main_p xxx, buss_h h2buss, size_t size_in, unsigned mask_in);
unsigned buss_data(buss_p buss, unsigned* write);
void buss_init(buss_p buss);
void buss_signal(buss_p buss, unsigned* write, unsigned hold);
unsigned buss_value(buss_p buss, unsigned* write);
