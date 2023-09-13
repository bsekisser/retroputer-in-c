#pragma once

/* **** */

typedef struct io_buss_t** io_buss_h;
typedef struct io_buss_t* io_buss_p;

/* **** */

#include "main.h"

/* **** */

#include <stdint.h>

/* **** */

typedef uint16_t (*io_port_fn)(void* param, uint8_t p, uint16_t const* write);

typedef struct io_port_t** io_port_h;
typedef struct io_port_t* io_port_p;
typedef struct io_port_t {
	io_port_fn fn;
	void* param;
}io_port_t;

/* **** */

io_buss_p io_buss_alloc(main_p xxx, io_buss_h io);
void io_buss_init(io_buss_p io);
uint16_t io_buss(io_buss_p io, uint8_t p, uint16_t const* write);
