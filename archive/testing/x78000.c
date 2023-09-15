#include <stdio.h>

void test0(void) {
	const unsigned d0 = 0xe000;
	const unsigned d1 = d0 << 3;

	printf("(d = 0x%08x) << 3 = 0x%08x\n", d0, d1);

	const unsigned x0 = 0x8000;
	const unsigned x1 = d1 + x0;

	printf("d(0x%08x) + x(0x%08x) = 0x%08x\n", d1, x0, x1);
};

void test1(void) {
	uint32_t address = 0;
	
	const int16_t d0 = 0xe000;
}

int main(void) {
	return(0);
}
