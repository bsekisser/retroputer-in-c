#pragma once

enum REGISTER_INDEX {
	rA = 0, rAL = 1,
	rB = 2, rBL = 3,
	rC = 4, rCL = 5,
	rD = 6, rDL = 7,
	rX = 8, rXL = 9,
	rY = 10, rYL = 11,
	rBP = 12,
	rSP = 14,
	rSTATUS = 16,
	rFLAGS = 17,
	rPC = 18,
	rMM = 20,
	rMP = 22,
	_rXXX,
	_GPR_COUNT,
};
