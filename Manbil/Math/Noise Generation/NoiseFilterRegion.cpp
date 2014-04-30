#include "NoiseFilterRegion.h"


void MaxFilterRegion::DoToEveryPoint(void* pData, ActionFunc toDo, const Array2D<float> & noise, Vector2i noiseSize, bool calcStrength)
{
    Vector2i loc;
    for (loc.y = 0; loc.y < noiseSize.y; ++loc.y)
    {
        for (loc.x = 0; loc.x < noiseSize.x; ++loc.x)
        {
            if (ActiveIn.Touches(noise[loc]))
                toDo(pData, loc, StrengthLerp);
        }
    }
}

void CircularFilterRegion::DoToEveryPoint(void* pData, ActionFunc toDo, const Array2D<float> & noise, Vector2i noiseSize, bool calcStrength)
{
    Vector2i loc;
    Vector2f locF;
    float radSqr = BasicMath::Square(Radius);
    Vector2i topLeft((int)(Center.x - Radius), (int)(Center.y - Radius)),
        bottomRight((int)(0.5f + Center.x + Radius), (int)(0.5f + Center.y + Radius));
    if (topLeft.x < 0) topLeft.x = 0;
    if (topLeft.y < 0) topLeft.y = 0;
    if (topLeft.x >= noiseSize.x) topLeft.x = noiseSize.x - 1;
    if (topLeft.y >= noiseSize.y) topLeft.y = noiseSize.y - 1;
    if (bottomRight.x < 0) bottomRight.x = 0;
    if (bottomRight.y < 0) bottomRight.y = 0;
    if (bottomRight.x >= noiseSize.x) bottomRight.x = noiseSize.x - 1;
    if (bottomRight.y >= noiseSize.y) bottomRight.y = noiseSize.y - 1;

    if (calcStrength)
    {
        if (Wrap)
        {
            Vector2i wrapped;
            for (loc.y = topLeft.y; loc.y < bottomRight.y; ++loc.y)
            {
                wrapped.y = noise.Wrap(Vector2i(0, loc.y)).y;
                locF.y = (float)wrapped.y;

                for (loc.x = topLeft.x; loc.x < bottomRight.x; ++loc.x)
                {
                    wrapped.x = noise.Wrap(Vector2i(loc.x, 0)).x;
                    locF.x = (float)wrapped.x;

                    if (Center.DistanceSquared(locF) <= radSqr)
                    {
                        if (ActiveIn.Touches(noise[wrapped]))
                            toDo(pData, wrapped, StrengthLerp * GetStrengthDropoffScale(locF));
                    }
                }
            }
        }
        else
        {
            Vector2i min(BasicMath::Max(0, topLeft.x), BasicMath::Max(0, topLeft.y)),
                     max(BasicMath::Min((int)noise.GetWidth() - 1, bottomRight.x),
                         BasicMath::Min((int)noise.GetHeight() - 1, bottomRight.y));
            for (loc.y = min.y; loc.y <= max.y; ++loc.y)
            {
                locF.y = (float)loc.y;

                for (loc.x = min.x; loc.x <= max.x; ++loc.x)
                {
                    locF.x = (float)loc.x;

                    if (Center.DistanceSquared(locF) <= radSqr)
                    {
                        if (ActiveIn.Touches(noise[loc]))
                            toDo(pData, loc, StrengthLerp * GetStrengthDropoffScale(locF));
                    }
                }
            }
        }
        
    }
    else
    {
        if (Wrap)
        {
            Vector2i wrapped;
            for (loc.y = topLeft.y; loc.y < bottomRight.y; ++loc.y)
            {
                wrapped.y = noise.Wrap(Vector2i(0, loc.y)).y;
                locF.y = (float)wrapped.y;

                for (loc.x = topLeft.x; loc.x < bottomRight.x; ++loc.x)
                {
                    wrapped.x = noise.Wrap(Vector2i(loc.x, 0)).x;
                    locF.x = (float)wrapped.x;

                    if (Center.DistanceSquared(locF) <= radSqr)
                    {
                        if (ActiveIn.Touches(noise[wrapped]))
                            toDo(pData, wrapped, StrengthLerp);
                    }
                }
            }
        }
        else
        {
            Vector2i min(BasicMath::Max(0, topLeft.x), BasicMath::Max(0, topLeft.y)),
                max(BasicMath::Min((int)noise.GetWidth() - 1, bottomRight.x),
                BasicMath::Min((int)noise.GetHeight() - 1, bottomRight.y));
            for (loc.y = min.y; loc.y <= max.y; ++loc.y)
            {
                locF.y = (float)loc.y;

                for (loc.x = min.x; loc.x <= max.x; ++loc.x)
                {
                    locF.x = (float)loc.x;

                    if (Center.DistanceSquared(locF) <= radSqr)
                    {
                        if (ActiveIn.Touches(noise[loc]))
                            toDo(pData, loc, StrengthLerp);
                    }
                }
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

void RectangularFilterRegion::DoToEveryPoint(void* pData, ActionFunc toDo, const Array2D<float> & noise, Vector2i noiseSize, bool calcStrength)
{
    if (Wrap)
    {
        Vector2i loc, wrapped;
        for (loc.y = TopLeft.y; loc.y <= BottomRight.y; ++loc.y)
        {
            wrapped.y = noise.Wrap(loc).y;

            for (loc.x = TopLeft.x; loc.x <= BottomRight.x; ++loc.x)
            {
                wrapped.x = noise.Wrap(loc).x;

                if (ActiveIn.Touches(noise[wrapped]))
                    toDo(pData, wrapped, StrengthLerp);
            }
        }
    }
    else
    {
        Vector2i min(BasicMath::Max(0, TopLeft.x), BasicMath::Max(0, TopLeft.y)),
                 max(BasicMath::Min((int)noise.GetWidth() - 1, BottomRight.x),
                     BasicMath::Min((int)noise.GetHeight() - 1, BottomRight.y));

        Vector2i loc;
        for (loc.y = min.y; loc.y <= max.y; ++loc.y)
            for (loc.x = min.x; loc.x <= max.x; ++loc.x)
                if (ActiveIn.Touches(noise[loc]))
                    toDo(pData, loc, StrengthLerp);
    }
    
}