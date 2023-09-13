#pragma once

typedef int (*buss_receiver_fn)(void* param, int32_t value);

typedef struct buss_receiver_t** buss_receiver_h;
typedef struct buss_receiver_t* buss_receiver_p;
typedef struct buss_receiver_t {
	buss_receiver_p next;
//
	buss_receiver_fn fn;
	void* param;
}buss_receiver_t;

buss_receiver_p buss_receiver(buss_receiver_fn fn, void* param);
void buss_receiver_add_receiver(buss_receiver_h h2br, buss_receiver_p br);
