#pragma once

#include <math.h>
#include <cwchar>
#include <assert.h>


//Basic math functions.
class Mathf
{
public:

    static const float NaN;
    static bool IsNaN(float testF) { return testF != testF; }

    //Quickly computes 1.0f/sqrt(f), with lower precision.
    static float FastInvSqrt1(float f);
    //Quickly computes 1.0f/sqrt(f), with higher precision.
    static float FastInvSqrt2(float f);

    //Raises an integer to a integer power.
    static long IntPow(int base, int exponent);

    //Any type that satisfies the following constraints:
    //   1) Can be constructed with a given initial value of 0.
    //   2) Defines operators "<" and "-".
    template <typename T>
    //Computes the sign (-1, 0, or 1) of a value.
    static int Sign(T val) { return (T(0) < val) - (val < T(0)); }

    //Computes the absolute value of the given integer.
    static unsigned int Abs(int i) { return (unsigned int)((i < 0) ? -i : i); }
    //Computes the absolute value of the given float.
    static float Abs(float f) { return fabsf(f); }

    //Rounds to the given number of decimals.
    static float Round(float v, int decimals);
    //Rounds the given value to the nearest integer.
    static int RoundToInt(float v) { return (int)(v + (0.4999f * Sign(v))); }
    //Rounds the given value to the nearest integer.
    static int RoundToInt(double v) { return (int)(v + (0.499999 * Sign(v))); }
    //Rounds the given positive value to the nearest unsigned integer.
    static inline unsigned int RoundToUInt(float v)
    {
        assert(v >= 0.0f);
        return (unsigned int)(v + (0.4999f * Sign(v)));
    }
    //Rounds the given positive value to the nearest unsigned integer.
    static inline unsigned int RoundToUInt(double v)
    {
        assert(v >= 0.0f);
        return (unsigned int)(v + (0.499999 * Sign(v)));
    }

    //Converts radians to degrees.
    static float RadToDeg(float radians) { return radians * 57.2957795f; }
    //Converts degrees to radians.
    static float DegToRad(float degrees) { return degrees * 0.0174532f; }

    //Computes a logarithm of base e.
    static float Log(float value) { return log(value); }
    //Computes a logarithm of any base.
    static float Log(float value, float base) { return log(value) / log(base); }
    //Calculates the given root of the given value.
    static float Root(float value, float root) { return powf(value, 1.0f / root); }

    //Any type with < and > operators defined.
    template<typename ComparableType>
    //Clamps the given value between the given min and max.
    static ComparableType Clamp(ComparableType val, ComparableType min, ComparableType max)
    {
        return (val < min ? min : (val > max ? max : val));
    }

    //Any type with the < operator defined.
    template<class ComparableClass>
    //Gets the max of two given values.
    static ComparableClass Max(ComparableClass one, ComparableClass two) { return (one < two) ? two : one; }
    //Any type with the < operator defined.
    template<class ComparableClass>
    //Gets the max of two given values.
    static ComparableClass Max(ComparableClass one, ComparableClass two, ComparableClass three)
    {
        return Max(one, Max(two, three));
    }
    //Any type with the > operator defined.
    template<class ComparableClass>
    //Gets the min of two given values.
    static ComparableClass Min(ComparableClass one, ComparableClass two) { return (one > two) ? two : one; }
    //Any type with the > operator defined.
    template<class ComparableClass>
    //Gets the min of two given values.
    static ComparableClass Min(ComparableClass one, ComparableClass two, ComparableClass three)
    {
        return Min(one, Min(two, three));
    }

    //Calculates the given value clamped to the closest value of
    //    (intervalStart + (n * intervalSize)) for any integer n.
    static inline float ClampToInterval(float intervalStart, float intervalSize, float value)
    {
        int rounded = RoundToInt((value - intervalStart) / intervalSize);
        return intervalStart + (intervalSize * rounded);
    }

    struct ClampIntervalResult
    {
        int zero, one;
        ClampIntervalResult(int _zero, int _one) : zero(_zero), one(_one) { }
    };
    //Same as "ClampToInterval", but instead of returning the clamped value, this function returns
    //    the two values of n that the given value lies between.
    static ClampIntervalResult ClampToIntervalBounds(float intervalStart, float intervalSize, float value);

    //Gets whether "factor" is a factor of "n".
    static bool IsFactor(int n, int factor) { return (n % factor) == 0; }


    //Linear interpolation.
    static float Lerp(float start, float end, float lerp) { return (lerp * (end - start)) + start; }
    //Gets the value that, if plugged into "lerp", would output "value".
    static inline float LerpComponent(float start, float end, float value)
    {
        return (value - start) / (end - start);
    }

    //Remaps the given value from the given source range to the given destination range.
    static inline float Remap(float startSource, float endSource,
                              float startDest, float endDest, float sourceValue)
    {
        return Lerp(startDest, endDest, (sourceValue - startSource) / (endSource - startSource));
    }

    //Pushes a value (assumed to be between 0 and 1) towards the endpoints of 0 and 1.
    static float Smooth(float value) { return value * value * ((value * -2.0f) + 3.0f); }
    //Pushes a value (assumed to be between 0 and 1) far towards the endpoints of 0 and 1.
    static inline float Supersmooth(float value)
    {
        return value * value * value * (10.0f + (value * (-15.0f + (value * 6.0f))));
    }
};