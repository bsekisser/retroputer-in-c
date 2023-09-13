#pragma once

typedef union memory_flags_t* memory_flags_p;
typedef union memory_flags_t {
	unsigned raw_flags;
	struct {
		unsigned override:1;
	};
}memory_flags_t;
