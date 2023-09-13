#pragma once

/* **** */

typedef struct memory_t** memory_h;
typedef struct memory_t* memory_p;

/* **** */

#include "memory_bank.h"
#include "segment.h"

#include "main.h"

/* **** */

#include <stdint.h>

/* **** */

unsigned memory_address_page(unsigned address);
unsigned memory_address_page_mapped(memory_p memory, unsigned page);
memory_p memory_alloc(main_p xxx, memory_h h2m);
memory_bank_p memory_bank_for_address(memory_p m, unsigned address);
void memory_init(memory_p m);
void memory_load_rom(memory_p m, segment_p segment);
unsigned memory_map(memory_p m, unsigned const* write);
uint8_t memory_read_byte(memory_p m, unsigned address);
uint16_t memory_read_word(memory_p m, unsigned address);
unsigned memory_read_long(memory_p m, unsigned address);
unsigned memory_va_to_pa(memory_p m, unsigned address, unsigned* p2page);
void memory_write_byte(memory_p m, unsigned address, uint8_t value);
void memory_write_word(memory_p m, unsigned address, uint16_t value);
