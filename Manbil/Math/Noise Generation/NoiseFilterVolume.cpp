#include "NoiseFilterVolume.h"

#include "../Shapes/ThreeDShapes.h"


void MaxFilterVolume::DoToEveryPoint(void* pData, ActionFunc toDo, const Fake3DArray<float> & noise, Vector3i noiseSize, bool calcStrength)
{
    for (Vector3i loc; loc.z < noiseSize.z; ++loc.z)
        for (loc.y = 0; loc.y < noiseSize.y; ++loc.y)
            for (loc.x = 0; loc.x < noiseSize.x; ++loc.x)
                if (ActiveIn.Touches(noise[loc]))
                    toDo(pData, loc, StrengthLerp);
}

void SphereFilterVolume::DoToEveryPoint(void* pData, ActionFunc toDo, const Fake3DArray<float> & noise, Vector3i noiseSize, bool calcStrength)
{
    Vector3i minCorner = (Center - Vector3f(Radius, Radius, Radius)).CastToInt(),
             maxCorner = (Center + Vector3f(Radius, Radius, Radius)).Ceil();
    if (!Wrap)
    {
        minCorner.x = BasicMath::Clamp(minCorner.x, 0, noiseSize.x - 1);
        minCorner.y = BasicMath::Clamp(minCorner.y, 0, noiseSize.y - 1);
        minCorner.z = BasicMath::Clamp(minCorner.z, 0, noiseSize.z - 1);
        maxCorner.x = BasicMath::Clamp(maxCorner.x, 0, noiseSize.x - 1);
        maxCorner.y = BasicMath::Clamp(maxCorner.y, 0, noiseSize.y - 1);
        maxCorner.z = BasicMath::Clamp(maxCorner.z, 0, noiseSize.z - 1);
    }

    Sphere sphere(Center, Radius);
    Vector3f locF;
    float(*strengthCalculator)(Vector3f pos, SphereFilterVolume * thisV);
    if (calcStrength) strengthCalculator = [](Vector3f p, SphereFilterVolume * thisV) { return thisV->GetStrengthDropoffScale(p); };
    else strengthCalculator = [](Vector3f p, SphereFilterVolume * thisV) { return 1.0f; };

    if (Wrap)
    {
        Vector3i wrapped;
        for (Vector3i loc(0, 0, minCorner.z); loc.z <= maxCorner.z; ++loc.z)
        {
            locF.z = (float)loc.z;

            wrapped.z = loc.z;
            while (wrapped.z < 0) wrapped.z += noiseSize.z;
            while (wrapped.z >= noiseSize.z) wrapped.z -= noiseSize.z;

            for (loc.y = minCorner.y; loc.y <= maxCorner.y; ++loc.y)
            {
                locF.y = (float)loc.y;

                wrapped.y = loc.y;
                while (wrapped.y < 0) wrapped.y += noiseSize.y;
                while (wrapped.y >= noiseSize.y) wrapped.y -= noiseSize.y;

                for (loc.x = minCorner.x; loc.x <= maxCorner.x; ++loc.x)
                {
                    locF.x = (float)loc.x;

                    wrapped.x = loc.x;
                    while (wrapped.x < 0) wrapped.x += noiseSize.x;
                    while (wrapped.x >= noiseSize.x) wrapped.x -= noiseSize.x;

                    if (sphere.IsPointInside(Vector3f(loc.x, loc.y, loc.z)) &&
                        ActiveIn.Touches(noise[wrapped]))
                        toDo(pData, wrapped, StrengthLerp * GetStrengthDropoffScale(locF));

                }
            }
        }
    }
    else
    {
        for (Vector3i loc(0, 0, minCorner.z); loc.z <= maxCorner.z; ++loc.z)
        {
            locF.z = (float)loc.z;

            for (loc.y = minCorner.y; loc.y <= maxCorner.y; ++loc.y)
            {
                locF.y = (float)loc.y;

                for (loc.x = minCorner.x; loc.x <= maxCorner.x; ++loc.x)
                {
                    locF.x = (float)loc.x;

                    if (sphere.IsPointInside(Vector3f(loc.x, loc.y, loc.z)) &&
                        ActiveIn.Touches(noise[loc]))
                        toDo(pData, loc, StrengthLerp * GetStrengthDropoffScale(locF));

                }
            }
        }
    }
}
float SphereFilterVolume::GetStrengthDropoffScale(Vector3f pos) const
{
    if (DropoffRadiusPercent == 0.0f)
    {
        return 1.0f;
    }

    float maxDist = BasicMath::Abs(DropoffRadiusPercent) * Radius,
          dist = pos.Distance(Center);
    if (DropoffRadiusPercent > 0.0f)
    {
        return BasicMath::Clamp(BasicMath::Remap(0, maxDist, 1.0f, 0.0f, dist), 0.0f, 1.0f);
    }
    else
    {
        return BasicMath::Clamp(BasicMath::Remap(0, maxDist, 0.0f, 1.0f, dist), 0.0f, 1.0f);
    }
}

void CubeFilterVolume::DoToEveryPoint(void* pData, ActionFunc toDo, const Fake3DArray<float> & noise, Vector3i noiseSize, bool calcStrength)
{
    if (Wrap)
    {
        Vector3i wrapped;
        for (Vector3i loc(0, 0, MinCorner.z); loc.z <= MaxCorner.z; ++loc.z)
        {
            wrapped.z = loc.z;
            while (wrapped.z < 0) wrapped.z += noiseSize.z;
            while (wrapped.z >= noiseSize.z) wrapped.z -= noiseSize.z;

            for (loc.y = MinCorner.y; loc.y <= MaxCorner.y; ++loc.y)
            {
                wrapped.y = loc.y;
                while (wrapped.y < 0) wrapped.y += noiseSize.y;
                while (wrapped.y >= noiseSize.y) wrapped.y -= noiseSize.y;

                for (loc.x = MinCorner.x; loc.x <= MaxCorner.x; ++loc.x)
                {
                    wrapped.x = loc.x;
                    while (wrapped.x < 0) wrapped.x += noiseSize.x;
                    while (wrapped.x >= noiseSize.x) wrapped.x -= noiseSize.x;

                    if (ActiveIn.Touches(noise[wrapped]))
                        toDo(pData, wrapped, StrengthLerp);
                }
            }
        }
    }
    else
    {
        for (Vector3i loc(0, 0, MinCorner.z); loc.z <= MaxCorner.z; ++loc.z)
        {
            for (loc.y = MinCorner.y; loc.y <= MaxCorner.y; ++loc.y)
            {
                for (loc.x = MinCorner.x; loc.x <= MaxCorner.x; ++loc.x)
                {
                    if (ActiveIn.Touches(noise[loc]))
                        toDo(pData, loc, StrengthLerp);
                }
            }
        }
    }
}