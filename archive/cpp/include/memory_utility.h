#pragma once

/* **** */

#include <stdlib.h>

/* **** */

static inline void* array_buffer(size_t size)
{
	return(calloc(1, size));
}

static inline uint8_t* uint_8_array(void* buffer, unsigned buffer_offset, size_t size)
{
	if(!size)
		return(0);
	if(size < buffer_offset)
		return(0);
	
	return(((uint8_t*)buffer) + buffer_offset);
}
