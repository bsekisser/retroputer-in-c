static unsigned _mask(size_t size, uint32_t* p2mask)
{
	uint32_t mask = p2mask ? *p2mask : ~0U;

	return(mask & _BM(size << 3));
}

buss_p buss(buss_h h2buss, size_t* p2size, uint32_t* p2mask)
{
	ERR_NULL(h2buss);
	
	buss_p b = handle_calloc((void*)h2buss, 1, sizeof(buss_t));
	ERR_NULL(b);

	b->size = p2size ? p2size : 2;
	b->mask = _mask(b->size, p2mask);
}

void buss_add_receiver(buss_p b, buss_receiver_p br)
{
	if(b->receivers)
		return(buss_receiver_add_receiver(b->receivers, br);
	
	br->next = 0;
	b->receivers = br;
}
