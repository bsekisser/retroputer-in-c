#include "access.h"
#include "buss_receiver.hpp"
#include "buss.hpp"

/* **** */

#include "bitfield.h"

/* **** */

#include <cstddef>
#include <cstdint>

/* **** */

unsigned __mask(size_t size, unsigned* p2mask)
{
	const unsigned mask = p2mask ? *p2mask : 0xffffffff;

	return(mask & _BM(size << 3));
}

void buss::add_receiver(buss_receiver* br)
{
	if(receivers)
		receivers->add_receiver(br);

	receivers = br;
}

/*buss::buss()
{
	_size = 2;
	_mask = __mask(_size, 0);
}*/

buss::buss(size_t size, unsigned mask)
{
	_size = size;
	_mask = __mask(_size, &mask);
}

/*const buss& buss::operator =(const buss* cb)
{
	_size = cb->_size;
	_mask = cb->_mask;
	return(*this);
}*/

void buss::signal(uint32_t* write, unsigned hold)
{
	const uint32_t signal_value = value(write);

	if(receivers)
		receivers->signal(signal_value);

	if(!hold)
		_value = 0;
}
