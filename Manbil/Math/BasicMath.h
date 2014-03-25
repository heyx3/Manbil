#pragma once

#include <math.h>

//Basic math functions.
class BasicMath
{
public:
	
	//Quickly computes 1.0f/sqrt(f), with lower precision.
	static inline float FastInvSqrt1(float f)
	{
		long i;
		float x2, y;
		const float threeHalfs = 1.5f;

		x2 = f * 0.5f;
		y = f;
		i = *(long*) &y;
		i = 0x5f3759df - (i >> 1);
		y = *(float*) &i;

		y = y * (threeHalfs - (x2 * y * y));

		return y;
	}
	//Quickly computes 1.0f/sqrt(f), with higher precision.
	static inline float FastInvSqrt2(float f)
	{
		long i;
		float x2, y;
		const float threeHalfs = 1.5f;

		x2 = f * 0.5f;
		y = f;
		i = *(long*) &y;
		i = 0x5f3759df - (i >> 1);
		y = *(float*) &i;

		y = y * (threeHalfs - (x2 * y * y));
		y = y * (threeHalfs - (x2 * y * y));

		return y;
	}
	//Quickly computes sqrt(f) with lower precision.
	static inline float FastSqrt(float f) { return 1.0f / FastInvSqrt1(f); }

	//Raises an integer to a integer power.
	static long IntPow(int base, int exponent)
	{
		//Use "exponentiation by squaring".
		//http://stackoverflow.com/questions/101439/the-most-efficient-way-to-implement-an-integer-based-power-function-powint-int
		
		long result = 1;

		while (exponent > 0)
		{
			if (exponent & 1) result *= base;
			exponent >>= 1;
			base *= base;
		}

		return result;
    }

    template <typename T>
    static int Sign(T val)
    {
        return (T(0) < val) - (val < T(0));
    }

	static int Abs(int i) { return (i < 0) ? -i : i; }
	static float Abs(float f) { int f2 = (int&)f & 0x7FFFFFFF; return (float&)f2; }

	static const float NaN;
	static bool IsNaN(float testF) { return testF == NaN; }

	static float Round(float v, int decimals)
	{
		float multiplier = pow(10.0f, decimals),
			  f1 = (v * multiplier),
			  f2 = (v > 0) ? (f1 + 0.5f) : (f1 - 0.5f),
			  f3 = (float)((int)f2 / multiplier);
		return f3;
	}
	static int RoundToInt(float v) { return (int)Round(v, 0); }

	template<class MultipliableClass>
	static MultipliableClass Square(MultipliableClass f) { return f * f; }
	template<class MultipliableClass>
	static MultipliableClass Cube(MultipliableClass f) { return f * f * f; }

	static float Log(float value, float base) { return log(value) / log(base); }

	static float Root(float value, float root) { return powf(value, 1.0f / root); }

	static float Clamp(float val, float min = 0.0f, float max = 1.0f) { return (val < min ? min : (val > max ? max : val)); }

    template<class ComparableClass>
    static ComparableClass Max(ComparableClass one, ComparableClass two) { return (one < two) ? two : one; }
    template<class ComparableClass>
    static ComparableClass Max(ComparableClass one, ComparableClass two, ComparableClass three) { return Max(one, Max(two, three)); }
    template<class ComparableClass>
    static ComparableClass Min(ComparableClass one, ComparableClass two) { return (one > two) ? two : one; }
    template<class ComparableClass>
    static ComparableClass Min(ComparableClass one, ComparableClass two, ComparableClass three) { return Min(one, Min(two, three)); }

	//Gets the given value clamped to the closest value of (intervalStart + (n * intervalSize)) for some integer n.
	static inline float ClampInterval(float intervalStart, float intervalSize, float value)
	{
		int rounded = BasicMath::RoundToInt((value - intervalStart) / intervalSize);
		return intervalStart + (intervalSize * rounded);
	}
	struct ClampIntBoundsResult { int zero, one; ClampIntBoundsResult(int _zero, int _one) : zero(_zero), one(_one) { } };
	//Gets the given value clamped to the closest value of (intervalStart + (n * intervalSize)) for some integer n.
	//This function gets the two values of n that the given value lies between.
	static inline ClampIntBoundsResult ClampIntervalBounds(float intervalStart, float intervalSize, float value)
	{
		float sub = value - intervalStart;
		//bool nearStart = (Abs(sub) < Abs(sub - intervalSize));
		float divided = Abs(sub / intervalSize);
		int rounded = RoundToInt(divided);
		bool nearStart = (rounded == (int)divided);
		return (nearStart ? ClampIntBoundsResult(rounded, rounded + 1) : ClampIntBoundsResult(rounded - 1, rounded));
	}

	//Gets the value that, if plugged into "lerp", would output "value".
	static float LerpComponent(float start, float end, float value) { return (value - start) / (end - start); }

	//Pushes a value (assumed to be between 0 and 1) towards the endpoints of 0 and 1.
	static float Smooth(float value) { return value * value * ((value * -2.0f) + 3.0f); }
	//Pushes a value (assumed to be between 0 and 1) far towards the endpoints of 0 and 1.
	static float Supersmooth(float value) { return value * value * value * (10.0f + (value * (-15.0f + (value * 6.0f)))); }

	static float Lerp(float start, float end, float lerp) { return (lerp * (end - start)) + start; }
	static float Remap(float startSource, float endSource, float startDest, float endDest, float sourceValue) { return Lerp(startDest, endDest, (sourceValue - startSource) / (endSource - startSource)); }
};