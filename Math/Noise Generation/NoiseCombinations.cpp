#include "NoiseCombinations.h"

void Combine2Noises::Generate(Noise2D & nse) const
{
	int x, y;
	Vector2i loc;
	for (x = 0; x < nse.GetWidth(); ++x)
	{
		loc.x = x;

		for (y = 0; y < nse.GetHeight(); ++y)
		{
			loc.y = y;

			nse[loc] = CombineOp((*First)[loc], (*Second)[loc]);
		}
	}
}

void Combine3Noises::Generate(Noise2D & nse) const
{
	int x, y;
	Vector2i loc;
	for (x = 0; x < nse.GetWidth(); ++x)
	{
		loc.x = x;

		for (y = 0; y < nse.GetHeight(); ++y)
		{
			loc.y = y;

			nse[loc] = CombineOp((*First)[loc], (*Second)[loc], (*Third)[loc]);
		}
	}
}