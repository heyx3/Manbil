#include "BasicMath.h"

#include <limits>


const float BasicMath::NaN = std::numeric_limits<float>::quiet_NaN();


float BasicMath::FastInvSqrt1(float f)
{
    long i;
    float x2, y;
    const float threeHalfs = 1.5f;

    x2 = f * 0.5f;
    y = f;
    i = *(long*)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float*)&i;

    y = y * (threeHalfs - (x2 * y * y));

    return y;
}
float BasicMath::FastInvSqrt2(float f)
{
    long i;
    float x2, y;
    const float threeHalfs = 1.5f;

    x2 = f * 0.5f;
    y = f;
    i = *(long*)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float*)&i;

    y = y * (threeHalfs - (x2 * y * y));
    y = y * (threeHalfs - (x2 * y * y));

    return y;
}

long BasicMath::IntPow(int base, int exponent)
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

float BasicMath::Round(float v, int decimals)
{
    float multiplier = pow(10.0f, decimals),
          f1 = (v * multiplier),
          f2 = (f1 + (0.5f * Sign(v))),
          f3 = (float)((int)f2 / multiplier);
    return f3;
}

BasicMath::ClampIntervalResult BasicMath::ClampToIntervalBounds(float intervalStart,
                                                                float intervalSize,
                                                                float value)
{
    float sub = value - intervalStart;
    float divided = Abs(sub / intervalSize);
    int rounded = RoundToInt(divided);

    bool nearStart = (rounded == (int)divided);
    return (nearStart ? ClampIntervalResult(rounded, rounded + 1) :
                        ClampIntervalResult(rounded - 1, rounded));
}