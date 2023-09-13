#include "config.h"
#include "buss.h"

/* **** */

#include "main.h"

/* **** */

#include "bitfield.h"
#include "handle.h"
#include "mem_access.h"
#include "queue.h"

/* **** */

typedef struct buss_t {
	queue_t receivers;
//
	unsigned data;
	unsigned mask;
	size_t size;
	unsigned value;
}buss_t;

/* **** */

void buss_add_receiver(buss_p buss, buss_receiver_p brp, buss_receiver_fn brf, void* param)
{
	enqueue(&buss->receivers, brp->receiver_qelem);

	brp->fn = brf;
	brp->param = param;
	brp->receiver_qelem->data = brp;
}

buss_p buss_alloc(main_p xxx, buss_h h2buss, size_t size_in, unsigned mask_in)
{
	TRACE_ALLOC();

	buss_p buss = handle_calloc((void*)h2buss, 1, sizeof(buss_t));
	ERR_NULL(buss);
	
	queue_init(&buss->receivers);
	
	const unsigned size = size_in ? size_in : 2;
	const unsigned size_mask = mlBF((size << 3) - 1, 0);

	const unsigned mask = mask_in ? mask_in : size_mask;

	buss->mask = mask;
	buss->size = size;

	return(buss);
}

unsigned buss_data(buss_p buss, unsigned* write)
{
	return(mem_32_access(&buss->data, write));
}

void buss_init(buss_p buss)
{
	TRACE_INIT();
}

void buss_signal(buss_p buss, unsigned* write, unsigned hold)
{
	const unsigned data = buss_value(buss, write);

	buss_receiver_p brp = (buss_receiver_p)buss->receivers.head;

	while(brp) {
		brp->fn(brp->param, data);
		brp = brp->receiver_qelem->next->data;
	}

	if(!hold)
		buss_value(buss, 0);
}

unsigned buss_value(buss_p buss, unsigned* write)
{
	return(mem_32_access(&buss->value, write));
}
