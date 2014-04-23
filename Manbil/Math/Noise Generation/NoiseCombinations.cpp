#include "NoiseCombinations.h"

void Combine2Noises::Generate(Noise2D & nse) const
{
	Vector2i loc;
    for (loc.y = 0; loc.y < nse.GetHeight(); ++loc.y)
        for (loc.x = 0; loc.x < nse.GetWidth(); ++loc.x)
            nse[loc] = CombineOp((*First)[loc], (*Second)[loc]);
}

void Combine3Noises::Generate(Noise2D & nse) const
{
    Vector2i loc;
    for (loc.y = 0; loc.y < nse.GetHeight(); ++loc.y)
        for (loc.x = 0; loc.x < nse.GetWidth(); ++loc.x)
			nse[loc] = CombineOp((*First)[loc], (*Second)[loc], (*Third)[loc]);
}