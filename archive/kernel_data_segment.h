#pragma once

/* **** */

#include "kernel_data_segment.t"

#define _KERNEL_SEGMENT_NAME(_x) kernel_ ## _x ## _data
#define KERNEL_SEGMENT(_address) \
	_address, sizeof(_KERNEL_SEGMENT_NAME(_address)), _KERNEL_SEGMENT_NAME(_address),
