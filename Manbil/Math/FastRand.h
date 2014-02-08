#pragma once

#include "BasicMath.h"

//A fast (but not cryptographically-strong) PRNG.
class FastRand
{
public:

	int Seed;

	FastRand(int seed = 12345) : Seed(seed) { }

	inline int GetRandInt(void)
	{
		Seed = (Seed ^ 61) ^ (Seed >> 16);
		Seed += (Seed << 3);
		Seed ^= (Seed >> 4);
		Seed *= 0x27d4eb2d;
		Seed ^= (Seed >> 15);
		return Seed;
	}
	inline float GetZeroToOne(void)
	{
		const int b = 999999;
		return (float)(BasicMath::Abs(GetRandInt()) % b) / (float)b;
	}
};