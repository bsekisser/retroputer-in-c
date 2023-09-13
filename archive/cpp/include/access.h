#pragma once

/* **** */

#include <stdint.h>

/* **** */

template <class T>
T generic_access(T* data, T* write)
{
	if(write)
		*data = *write;

	return(*data);
}
