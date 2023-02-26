#pragma once
#include <iostream>
#include <immintrin.h>

namespace seq {
	float InvSqrt(float n) {
		const float threehalfs = 1.5F;
		float y = n;
		long i = *(long*)&y;
		i = 0x5f3759df - (i >> 1);
		y = *(float*)&i;
		y = y * (threehalfs - ((n * 0.5F) * y * y));
		return y;
	}

	void Mod2(int32_t* a) { *a &= 0x1; }
	void Mod2(int64_t* a) { *a &= 0x1; }
	void Mod2(uint32_t* a) { *a &= 0x1; }
	void Mod2(uint64_t* a) { *a &= 0x1; }

	void Mod4(int32_t* a) { *a -= ((*a >> 2) << 2); }
	void Mod4(int64_t* a) { *a -= ((*a >> 2) << 2); }
	void Mod4(uint32_t* a) { *a -= ((*a >> 2) << 2); }
	void Mod4(uint64_t* a) { *a -= ((*a >> 2) << 2); }

	void Mod8(int32_t* a) { *a -= ((*a >> 3) << 3); }
	void Mod8(int64_t* a) { *a -= ((*a >> 3) << 3); }
	void Mod8(uint32_t* a) { *a -= ((*a >> 3) << 3); }
	void Mod8(uint64_t* a) { *a -= ((*a >> 3) << 3); }
}