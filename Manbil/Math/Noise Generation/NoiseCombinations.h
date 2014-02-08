#pragma once

#include "BasicGenerators.h"

//TODO: Move NoiseFilterer into here; it's basically a "Combine1Noise" Generator.

class Combine2Noises : public Generator
{
public:

	typedef float (*CombinationFunc)(float noise1, float noise2);

	static float Add2(float f1, float f2) { return f1 + f2; }
	static float Subtract2(float f1, float f2) { return f1 - f2; }
	static float Max2(float f1, float f2) { return BasicMath::Max(f1, f2); }
	static float Min2(float f1, float f2) { return BasicMath::Min(f1, f2); }
	static float Multiply2(float f1, float f2) { return f1 * f2; }
	static float Divide2(float f1, float f2) { return f1 / f2; }
	static float Pow2(float f1, float f2) { return powf(f1, f2); }
	static float Root2(float f1, float f2) { return BasicMath::Root(f1, f2); }

	CombinationFunc CombineOp;
	Noise2D * First, * Second;

	Combine2Noises(CombinationFunc combOp, Noise2D * first, Noise2D * second)
		: CombineOp(combOp), First(first), Second(second) { }

	virtual void Generate(Noise2D & noise) const override;
};

class Combine3Noises : public Generator
{
public:

	typedef float (*CombinationFunc)(float noise1, float noise2, float noise3);

	static float Max3(float f1, float f2, float f3) { return BasicMath::Max(f1, BasicMath::Max(f2, f3)); }
	static float Min3(float f1, float f2, float f3) { return BasicMath::Min(f1, BasicMath::Min(f2, f3)); }

	CombinationFunc CombineOp;
	Noise2D * First, * Second, * Third;

	Combine3Noises(CombinationFunc combOp, Noise2D * first, Noise2D * second, Noise2D * third) : CombineOp(combOp), First(first), Second(second), Third(third) { }

	virtual void Generate(Noise2D & noise) const override;
};