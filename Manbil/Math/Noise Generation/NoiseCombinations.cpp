#include "NoiseCombinations.h"

void Combine2Noises2D::Generate(Noise2D & nse) const
{
    Noise2D first(nse.GetWidth(), nse.GetHeight());
    Noise2D second(nse.GetWidth(), nse.GetHeight());
    First->Generate(first);
    Second->Generate(second);

	Vector2i loc;
    for (loc.y = 0; loc.y < nse.GetHeight(); ++loc.y)
        for (loc.x = 0; loc.x < nse.GetWidth(); ++loc.x)
            nse[loc] = CombineOp(first[loc], second[loc]);
}

void Combine3Noises2D::Generate(Noise2D & nse) const
{
    Noise2D first(nse.GetWidth(), nse.GetHeight());
    Noise2D second(nse.GetWidth(), nse.GetHeight());
    Noise2D third(nse.GetWidth(), nse.GetHeight());
    First->Generate(first);
    Second->Generate(second);
    Third->Generate(third);

    Vector2i loc;
    for (loc.y = 0; loc.y < nse.GetHeight(); ++loc.y)
        for (loc.x = 0; loc.x < nse.GetWidth(); ++loc.x)
			nse[loc] = CombineOp(first[loc], second[loc], third[loc]);
}

void Combine2Noises3D::Generate(Noise3D & nse) const
{
    Noise3D first(nse.GetWidth(), nse.GetHeight(), nse.GetDepth());
    Noise3D second(nse.GetWidth(), nse.GetHeight(), nse.GetDepth());
    First->Generate(first);
    Second->Generate(second);

    Vector3i loc;
    for (loc.z = 0; loc.z < nse.GetDepth(); ++loc.z)
        for (loc.y = 0; loc.y < nse.GetHeight(); ++loc.y)
            for (loc.x = 0; loc.x < nse.GetWidth(); ++loc.x)
                nse[loc] = CombineOp(first[loc], second[loc]);
}

void Combine3Noises3D::Generate(Noise3D & nse) const
{
    Noise3D first(nse.GetWidth(), nse.GetHeight(), nse.GetDepth());
    Noise3D second(nse.GetWidth(), nse.GetHeight(), nse.GetDepth());
    Noise3D third(nse.GetWidth(), nse.GetHeight(), nse.GetDepth());
    First->Generate(first);
    Second->Generate(second);
    Third->Generate(third);

    Vector3i loc;
    for (loc.z = 0; loc.z < nse.GetDepth(); ++loc.z)
        for (loc.y = 0; loc.y < nse.GetHeight(); ++loc.y)
            for (loc.x = 0; loc.x < nse.GetWidth(); ++loc.x)
                nse[loc] = CombineOp(first[loc], second[loc], third[loc]);
}