#include "BasicGenerators.h"

namespace NoiseAnalysis2D
{
	MinMax GetMinAndMax(const Noise2D & nse)
	{
		MinMax ret(999999.0f, -999999.0f);

		float tempVal;
        int size = nse.GetWidth() * nse.GetHeight();
        for (unsigned int i = 0; i < size; ++i)
        {
            tempVal = nse.GetArray()[i];

            if (tempVal > ret.Max)
            {
                ret.Max = tempVal;
            }
            if (tempVal < ret.Min)
            {
                ret.Min = tempVal;
            }
        }

		return ret;
	}
	float GetAverage(const Noise2D & nse)
	{
		float av = 0.0f;

        int size = nse.GetWidth() * nse.GetHeight();
        for (unsigned int i = 0; i < size; ++i)
            av += nse.GetArray()[i];

		return av / (float)(nse.GetWidth() * nse.GetHeight());
	}
}
namespace NoiseAnalysis3D
{
    MinMax GetMinAndMax(Noise3D & nse)
    {
        MinMax ret(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());

        Vector3i loc;
        float tempVal;
        int size = nse.GetWidth() * nse.GetHeight() * nse.GetDepth();
        for (int i = 0; i < size; ++i)
        {
            tempVal = nse.GetArray()[i];
            if (tempVal > ret.Max)
            {
                ret.Max = tempVal;
            }
            if (tempVal < ret.Min)
            {
                ret.Min = tempVal;
            }
        }

        return ret;
    }
    float GetAverage(Noise3D & nse)
    {
        float avg = 0.0f;

        int size = nse.GetWidth() * nse.GetHeight() * nse.GetDepth();
        for (int i = 0; i < size; ++i)
            avg += nse.GetArray()[i];

        return avg / (float)size;
    }
}