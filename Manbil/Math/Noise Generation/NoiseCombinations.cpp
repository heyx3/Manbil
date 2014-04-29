#include "NoiseCombinations.h"

void Combine2Noises2D::Generate(Noise2D & nse) const
{
	Vector2i loc;
    for (loc.y = 0; loc.y < nse.GetHeight(); ++loc.y)
        for (loc.x = 0; loc.x < nse.GetWidth(); ++loc.x)
            nse[loc] = CombineOp((*First)[loc], (*Second)[loc]);
}

void Combine3Noises2D::Generate(Noise2D & nse) const
{
    Vector2i loc;
    for (loc.y = 0; loc.y < nse.GetHeight(); ++loc.y)
        for (loc.x = 0; loc.x < nse.GetWidth(); ++loc.x)
			nse[loc] = CombineOp((*First)[loc], (*Second)[loc], (*Third)[loc]);
}

void Combine2Noises3D::Generate(Noise3D & nse) const
{
    Vector3i loc;
    for (loc.z = 0; loc.z < nse.GetDepth(); ++loc.z)
        for (loc.y = 0; loc.y < nse.GetHeight(); ++loc.y)
            for (loc.x = 0; loc.x < nse.GetWidth(); ++loc.x)
                nse[loc] = CombineOp((*First)[loc], (*Second)[loc]);
}

void Combine3Noises3D::Generate(Noise3D & nse) const
{
    Vector3i loc;
    for (loc.z = 0; loc.z < nse.GetDepth(); ++loc.z)
        for (loc.y = 0; loc.y < nse.GetHeight(); ++loc.y)
            for (loc.x = 0; loc.x < nse.GetWidth(); ++loc.x)
                nse[loc] = CombineOp((*First)[loc], (*Second)[loc], (*Third)[loc]);
}