#pragma once

#include "Mathf.h"


//A fast (but not cryptographically-strong) PRNG.
//Always generates non-negative numbers.
class FastRand
{
public:

	int Seed;

	FastRand(int seed = 1234567) : Seed(seed) { }

	inline int GetRandInt(void)
	{
        //I forget where this particular XORshift generator came from.
		Seed = (Seed ^ 61) ^ (Seed >> 16);
		Seed += (Seed << 3);
		Seed ^= (Seed >> 4);
		Seed *= 0x27d4eb2d;
		Seed ^= (Seed >> 15);
        assert(Seed >= 0);
		return Seed;
	}
	inline float GetZeroToOne(void)
    {
        const int b = 9999999;
		return (float)(GetRandInt() % b) / (float)b;
	}
};