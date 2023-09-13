#pragma once

enum {
	_ALU_FLAG_Z,
	_ALU_FLAG_V,
	_ALU_FLAG_C,
	_ALU_FLAG_N,
	_STATUS_FLAG_SINGLE_STEP,
	_STATUS_FLAG_INTERRUPT_SERVICE,
	_STATUS_FLAG_INTERRUPT_DISABLE,
	_STATUS_FLAG_EXCEPTION,
};

#define __ALU_FLAG_BV(_x) _BV(_ALU_FLAG_##_x)

#define _ALU_FLAGS_BV_CV (__ALU_FLAG_BV(C) | __ALU_FLAG_BV(V))
#define _ALU_FLAGS_BV_NZ (__ALU_FLAG_BV(N) | __ALU_FLAG_BV(Z))

#define _ALU_FLAGS_BV_CNVZ (_ALU_FLAGS_BV_CV | _ALU_FLAGS_BV_NZ)
