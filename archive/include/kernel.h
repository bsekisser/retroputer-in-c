#pragma once

/* **** */

typedef struct kernel_t** kernel_h;
typedef struct kernel_t* kernel_p;

/* **** */

kernel_p kernel_alloc(main_p xxx, kernel_h k);
void kernel_init(kernel_p k);
