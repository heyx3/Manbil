#include "NoiseFilterRegion.h"


void MaxFilterRegion::DoToEveryPoint(void* pData, ActionFunc toDo, const Array2D<float> & noise, Vector2u noiseSize, bool calcStrength)
{
    for (Vector2u loc; loc.y < noiseSize.y; ++loc.y)
        for (loc.x = 0; loc.x < noiseSize.x; ++loc.x)
            if (ActiveIn.IsInside(noise[loc]))
                toDo(pData, loc, StrengthLerp);
}

void CircularFilterRegion::DoToEveryPoint(void* pData, ActionFunc toDo, const Array2D<float> & noise, Vector2u noiseSize, bool calcStrength)
{
    Vector2i minCorner = ToV2i(Center - Vector2f(Radius, Radius)),
             maxCorner = (Center + Vector2f(Radius, Radius)).Ceil();
    if (!Wrap)
    {
        minCorner.x = Mathf::Clamp<int>(minCorner.x, 0, noiseSize.x - 1);
        minCorner.y = Mathf::Clamp<int>(minCorner.x, 0, noiseSize.x - 1);
        maxCorner.x = Mathf::Clamp<int>(minCorner.x, 0, noiseSize.x - 1);
        maxCorner.y = Mathf::Clamp<int>(minCorner.x, 0, noiseSize.x - 1);
    }

    Vector2f locF;
    float(*strengthCalculator)(Vector2f pos, CircularFilterRegion * thisR);
    if (calcStrength) strengthCalculator = [](Vector2f p, CircularFilterRegion * thisR) { return thisR->GetStrengthDropoffScale(p); };
    else strengthCalculator = [](Vector2f p, CircularFilterRegion * thisR) { return 1.0f; };

    if (Wrap)
    {
        Vector2i wrapped;
        for (Vector2i loc(0, minCorner.y); loc.y <= maxCorner.y; ++loc.y)
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

                Vector2u wrappedU(wrapped.x, wrapped.y);
                if (Center.DistanceSquared(locF) < (Radius * Radius) && ActiveIn.IsInside(noise[wrappedU]))
                    toDo(pData, wrappedU, StrengthLerp * strengthCalculator(locF, this));
            }
        }
    }
    else
    {
        Vector2u minCornerU(Mathf::Max<unsigned int>(0, minCorner.x),
                            Mathf::Max<unsigned int>(0, minCorner.y));
        for (Vector2u loc(0, minCornerU.y); loc.y <= (unsigned int)maxCorner.y; ++loc.y)
        {
            locF.y = (float)loc.y;

            for (loc.x = minCornerU.x; loc.x <= (unsigned int)maxCorner.x; ++loc.x)
            {
                locF.x = (float)loc.x;

                if (Center.DistanceSquared(locF) < (Radius * Radius) && ActiveIn.IsInside(noise[loc]))
                    toDo(pData, loc, StrengthLerp * strengthCalculator(locF, this));
            }
        }
    }
}
float CircularFilterRegion::GetStrengthDropoffScale(Vector2f pos) const
{
    if (DropoffRadiusPercent == 0.0f)
    {
        return 1.0f;
    }

    float maxDist = Mathf::Abs(DropoffRadiusPercent) * Radius,
          dist = pos.Distance(Center);
    if (DropoffRadiusPercent > 0.0f)
    {
        return Mathf::Clamp(Mathf::Remap(0, maxDist, 1.0f, 0.0f, dist), 0.0f, 1.0f);
    }
    else
    {
        return Mathf::Clamp(Mathf::Remap(0, maxDist, 0.0f, 1.0f, dist), 0.0f, 1.0f);
    }
}

void RectangularFilterRegion::DoToEveryPoint(void* pData, ActionFunc toDo, const Array2D<float> & noise, Vector2u noiseSize, bool calcStrength)
{
    if (Wrap)
    {
        Vector2i wrapped;
        for (Vector2i loc(0, TopLeft.y); loc.y <= BottomRight.y; ++loc.y)
        {
            wrapped.y = loc.y;
            while (wrapped.y < 0) wrapped.y += noiseSize.y;
            while (wrapped.y >= noiseSize.y) wrapped.y -= noiseSize.y;

            for (loc.x = TopLeft.x; loc.x <= BottomRight.x; ++loc.x)
            {
                wrapped.x = loc.x;
                while (wrapped.x < 0) wrapped.x += noiseSize.x;
                while (wrapped.x >= noiseSize.x) wrapped.x -= noiseSize.x;

                Vector2u wrappedU((unsigned int)wrapped.x, (unsigned int)wrapped.y);

                if (ActiveIn.IsInside(noise[wrappedU]))
                    toDo(pData, wrappedU, StrengthLerp);
            }
        }
    }
    else
    {
        Vector2u topLeftU(Mathf::Max<unsigned int>(0, TopLeft.x),
                          Mathf::Max<unsigned int>(0, TopLeft.y));
        Vector2u maxCornerU(Mathf::Min<unsigned int>(BottomRight.x, noiseSize.x),
                            Mathf::Min<unsigned int>(BottomRight.y, noiseSize.y));
        for (Vector2u loc(0, topLeftU.y); loc.y <= maxCornerU.y; ++loc.y)
            for (loc.x = topLeftU.x; loc.x <= maxCornerU.x; ++loc.x)
                if (ActiveIn.IsInside(noise[loc]))
                    toDo(pData, loc, StrengthLerp);
    }
    
}