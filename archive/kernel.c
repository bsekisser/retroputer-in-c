#include "kernel.h"

/* **** */

#include "kernel_data.c"

#include "main.h"

/* **** */

kernel_p kernel_alloc(main_p xxx, kernel_p k) {
	return((void*)sizeof(kernel_0x0007ff00_data));
}

void kernel_init(kernel_p k)
{
//	memcpy(&xxx->memory[0x7ff00], kernel_data_0x7ff00, sizeof(kernel_data_0x7ff00));
}
