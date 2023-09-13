#pragma once

typedef union memory_bank_flags_t {
	unsigned raw_flags;
	struct {
		unsigned rom:1;
		unsigned shared:1;
	};
}memory_bank_flags_t;

typedef struct memory_bank_t* memory_bank_p;
typedef struct memory_bank_t {
	char* data;
//
	memory_bank_flags_t flags;
}memory_bank_t;
