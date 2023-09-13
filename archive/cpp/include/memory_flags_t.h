#pragma once

typedef union memory_flags_t* memory_flags_p;
typedef union memory_flags_t {
	unsigned raw_flags;
	struct {
		unsigned rom:1;
		unsigned shared:1;
	};
}memory_flags_t;
