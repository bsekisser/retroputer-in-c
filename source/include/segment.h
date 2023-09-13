#pragma once

/* **** */

#include <stddef.h>

/* **** */

typedef struct segment_t* segment_p;
typedef struct segment_t {
	unsigned address;
	size_t len;
	char const* data;
}segment_t;
