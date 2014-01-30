#include "BasicGenerators.h"

namespace NoiseAnalysis2D
{
	MinMax GetMinAndMax(Noise2D & nse)
	{
		MinMax ret(999999.0f, -999999.0f);

	    int x, y;
		Vector2i loc;
		float tempVal;
		for (x = 0; x < nse.GetWidth(); ++x)
		{
			loc.x = x;

			for (y = 0; y < nse.GetHeight(); ++y)
			{
				loc.y = y;

				tempVal = nse[loc];

				if (tempVal > ret.Max)
				{
					ret.Max = tempVal;
				}
				if (tempVal < ret.Min)
				{
					ret.Min = tempVal;
				}
			}
		}

		return ret;
	}
	float GetAverage(Noise2D & nse)
	{
		float av = 0.0f;

		int x, y;
		Vector2i loc;
		for (x = 0; x < nse.GetWidth(); ++x)
		{
			loc.x = x;

			for (y = 0; y < nse.GetHeight(); ++y)
			{
				loc.y = y;

				av += nse[loc];
			}
		}

		return av / (float)(nse.GetWidth() * nse.GetHeight());
	}
}