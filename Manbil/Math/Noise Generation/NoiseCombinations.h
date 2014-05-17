#pragma once

#include "BasicGenerators.h"


class Combine2Noises2D : public Generator2D
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
    Generator2D * First, *Second;

	Combine2Noises2D(CombinationFunc combOp, Generator2D * first, Generator2D * second)
		: CombineOp(combOp), First(first), Second(second) { }

	virtual void Generate(Noise2D & noise) const override;
};

class Combine3Noises2D : public Generator2D
{
public:

	typedef float (*CombinationFunc)(float noise1, float noise2, float noise3);

	static float Max3(float f1, float f2, float f3) { return BasicMath::Max(f1, BasicMath::Max(f2, f3)); }
	static float Min3(float f1, float f2, float f3) { return BasicMath::Min(f1, BasicMath::Min(f2, f3)); }
    static float Clamp(float value, float min, float max) { return BasicMath::Clamp(value, min, max); }

	CombinationFunc CombineOp;
    Generator2D * First, *Second, *Third;

    Combine3Noises2D(CombinationFunc combOp, Generator2D * first, Generator2D * second, Generator2D * third) : CombineOp(combOp), First(first), Second(second), Third(third) { }

	virtual void Generate(Noise2D & noise) const override;
};


class Combine2Noises3D : public Generator3D
{
public:

    typedef float(*CombinationFunc)(float noise1, float noise2);

    static float Add2(float f1, float f2) { return f1 + f2; }
    static float Subtract2(float f1, float f2) { return f1 - f2; }
    static float Max2(float f1, float f2) { return BasicMath::Max(f1, f2); }
    static float Min2(float f1, float f2) { return BasicMath::Min(f1, f2); }
    static float Multiply2(float f1, float f2) { return f1 * f2; }
    static float Divide2(float f1, float f2) { return f1 / f2; }
    static float Pow2(float f1, float f2) { return powf(f1, f2); }
    static float Root2(float f1, float f2) { return BasicMath::Root(f1, f2); }

    CombinationFunc CombineOp;
    Generator3D * First, *Second;

    Combine2Noises3D(CombinationFunc combOp, Generator3D * first, Generator3D * second)
        : CombineOp(combOp), First(first), Second(second)
    { }

    virtual void Generate(Noise3D & noise) const override;
};

class Combine3Noises3D : public Generator3D
{
public:

    typedef float(*CombinationFunc)(float noise1, float noise2, float noise3);

    static float Max3(float f1, float f2, float f3) { return BasicMath::Max(f1, BasicMath::Max(f2, f3)); }
    static float Min3(float f1, float f2, float f3) { return BasicMath::Min(f1, BasicMath::Min(f2, f3)); }
    static float Clamp(float value, float min, float max) { return BasicMath::Clamp(value, min, max); }

    CombinationFunc CombineOp;
    Generator3D * First, *Second, *Third;

    Combine3Noises3D(CombinationFunc combOp, Generator3D * first, Generator3D * second, Generator3D * third) : CombineOp(combOp), First(first), Second(second), Third(third) { }

    virtual void Generate(Noise3D & noise) const override;
};