#include "NoiseFilterVolume.h"

#include "../Shapes/ThreeDShapes.h"


void MaxFilterVolume::DoToEveryPoint(void* pData, ActionFunc toDo, const Array3D<float> & noise, Vector3u noiseSize, bool calcStrength)
{
    for (Vector3u loc; loc.z < noiseSize.z; ++loc.z)
        for (loc.y = 0; loc.y < noiseSize.y; ++loc.y)
            for (loc.x = 0; loc.x < noiseSize.x; ++loc.x)
                if (ActiveIn.IsInside(noise[loc]))
                    toDo(pData, loc, StrengthLerp);
}

void SphereFilterVolume::DoToEveryPoint(void* pData, ActionFunc toDo, const Array3D<float> & noise, Vector3u noiseSize, bool calcStrength)
{
    Vector3i minCorner = ToV3i(Center - Vector3f(Radius, Radius, Radius)),
             maxCorner = (Center + Vector3f(Radius, Radius, Radius)).Ceil();
    if (!Wrap)
    {
        minCorner.x = BasicMath::Clamp<int>(minCorner.x, 0, noiseSize.x - 1);
        minCorner.y = BasicMath::Clamp<int>(minCorner.y, 0, noiseSize.y - 1);
        minCorner.z = BasicMath::Clamp<int>(minCorner.z, 0, noiseSize.z - 1);
        maxCorner.x = BasicMath::Clamp<int>(maxCorner.x, 0, noiseSize.x - 1);
        maxCorner.y = BasicMath::Clamp<int>(maxCorner.y, 0, noiseSize.y - 1);
        maxCorner.z = BasicMath::Clamp<int>(maxCorner.z, 0, noiseSize.z - 1);
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

                    Vector3u wrappedU(wrapped.x, wrapped.y, wrapped.z);

                    if (sphere.IsPointInside(locF) && ActiveIn.IsInside(noise[wrappedU]))
                        toDo(pData, wrappedU, StrengthLerp * strengthCalculator(locF, this));

                }
            }
        }
    }
    else
    {
        Vector3u minCornerU(BasicMath::Max<unsigned int>(0, minCorner.x),
                            BasicMath::Max<unsigned int>(0, minCorner.y),
                            BasicMath::Max<unsigned int>(0, minCorner.z));
        for (Vector3u loc(0, 0, minCornerU.z); loc.z <= (unsigned int)maxCorner.z; ++loc.z)
        {
            locF.z = (float)loc.z;

            for (loc.y = minCornerU.y; loc.y <= (unsigned int)maxCorner.y; ++loc.y)
            {
                locF.y = (float)loc.y;

                for (loc.x = minCornerU.x; loc.x <= (unsigned int)maxCorner.x; ++loc.x)
                {
                    locF.x = (float)loc.x;

                    if (sphere.IsPointInside(locF) && ActiveIn.IsInside(noise[loc]))
                        toDo(pData, loc, StrengthLerp * strengthCalculator(locF, this));

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

void CubeFilterVolume::DoToEveryPoint(void* pData, ActionFunc toDo, const Array3D<float> & noise, Vector3u noiseSize, bool calcStrength)
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

                    Vector3u wrappedU((unsigned int)wrapped.x, (unsigned int)wrapped.y, (unsigned int)wrapped.z);
                    if (ActiveIn.IsInside(noise[wrappedU]))
                        toDo(pData, wrappedU, StrengthLerp);
                }
            }
        }
    }
    else
    {
        Vector3u minCornerU(BasicMath::Max<unsigned int>(0, MinCorner.x),
                            BasicMath::Max<unsigned int>(0, MinCorner.y),
                            BasicMath::Max<unsigned int>(0, MinCorner.z));
        Vector3u maxCornerU(BasicMath::Min<unsigned int>(MaxCorner.x, noiseSize.x),
                            BasicMath::Min<unsigned int>(MaxCorner.y, noiseSize.y),
                            BasicMath::Min<unsigned int>(MaxCorner.z, noiseSize.z));
        for (Vector3u loc(0, 0, minCornerU.z); loc.z <= maxCornerU.z; ++loc.z)
            for (loc.y = minCornerU.y; loc.y <= maxCornerU.y; ++loc.y)
                for (loc.x = minCornerU.x; loc.x <= maxCornerU.x; ++loc.x)
                    if (ActiveIn.IsInside(noise[loc]))
                        toDo(pData, loc, StrengthLerp);
    }
}