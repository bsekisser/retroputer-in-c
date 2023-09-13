#include <stdint.h>

typedef struct buss_t* buss_p;
typedef struct buss_t {
	int32_t data;
	uint32_t mask;
	size_t size;
	int32_t value;
}buss_t;

buss_p buss(size_t* size, unsigned* mask);
void buss_add_receiver(buss_p b, buss_receiver_p br);

static inline size_t buss_size(buss_p b)
{
	return(b->size);
}

static inline int32_t buss_value(buss_p b, int32_t* write)
{
	int32_t data = write ? *write : b->value;

	if(write) {
		data = ((data < 0) ? ((~data) + 1) : data) & b->mask;
		b->value = data;
	}
		
	return(data);
}
