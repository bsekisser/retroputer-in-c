#pragma once

#define REGISTER_COUNT  24

enum {
	rA, rAL,
	rB, rBL,
	rC, rCL,
	rD, rDL,
	rX, rXL,
	rY, rYL,
	rBP = 12,
	rSP = 14,
//
	rSTATUS = 16, rFLAGS,
	rPC = 18,
	rMM = 20,
	rMP = 22,
//
	_GPR_COUNT,
	_rSPR = rBP,
};

static inline char const* reg_name(unsigned r) {
	const char* reg_name_list[2][16] = {
		{ "A", "B", "C", "D",
			"X", "Y", "BP", "SP",
			"STATUS", "PC", "MM", "MP",
			"CCC", "DDD", "EEE", "FFF" },
		{ "AL", "BL", "CL", "DL",
			"XL", "YL", "666", "777",
			"FLAGS", "999", "aaa", "bbb",
			"ccc", "ddd", "eee", "fff" },
	};

	return(reg_name_list[r & 1][r >> 1]);
}
