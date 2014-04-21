#include "NoiseFilterRegion.h"


void MaxFilterRegion::DoToEveryPoint(void* pData, ActionFunc toDo, const Fake2DArray<float> & noise, Vector2i noiseSize, bool calcStrength)
{
    int x, y;
    Vector2i loc;

    for (x = 0; x < noiseSize.x; ++x)
    {
        loc.x = x;

        for (y = 0; y < noiseSize.y; ++y)
        {
            loc.y = y;

            if (ActiveIn.Touches(noise[loc]))
                toDo(pData, loc, StrengthLerp);
        }
    }
}

void CircularFilterRegion::DoToEveryPoint(void* pData, ActionFunc toDo, const Fake2DArray<float> & noise, Vector2i noiseSize, bool calcStrength)
{
    int x, y;
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
        for (x = topLeft.x; x < bottomRight.x; ++x)
        {
            loc.x = x;
            if (Wrap) loc.x = noise.Wrap(Vector2i(x, 0)).x;
            else if (x < 0 || x >= noise.GetWidth())
                continue;

            locF.x = (float)x;


            for (y = topLeft.y; y < bottomRight.y; ++y)
            {
                loc.y = y;
                if (Wrap) loc.y = noise.Wrap(Vector2i(0, y)).y;
                else if (y < 0 || y >= noise.GetHeight())
                    continue;

                locF.y = (float)y;

                if (Center.DistanceSquared(locF) <= radSqr)
                {
                    if (ActiveIn.Touches(noise[Vector2i(x, y)]))
                        toDo(pData, loc, StrengthLerp * GetStrengthDropoffScale(locF));
                }
            }
        }
    }
    else
    {
        for (x = topLeft.x; x <= bottomRight.x; ++x)
        {
            loc.x = x;
            if (Wrap) loc.x = noise.Wrap(Vector2i(x, 0)).x;
            else if (x < 0 || x >= noise.GetWidth())
                continue;

            locF.x = (float)x;

            for (y = topLeft.y; y <= bottomRight.y; ++y)
            {
                loc.y = y;
                if (Wrap) loc.y = noise.Wrap(Vector2i(0, y)).y;
                else if (y < 0 || y >= noise.GetHeight())
                    continue;

                locF.y = (float)y;

                if (Center.DistanceSquared(locF) <= radSqr)
                {
                    if (ActiveIn.Touches(noise[loc]))
                        toDo(pData, loc, StrengthLerp);
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

void RectangularFilterRegion::DoToEveryPoint(void* pData, ActionFunc toDo, const Fake2DArray<float> & noise, Vector2i noiseSize, bool calcStrength)
{
    Vector2i topLeft = TopLeft,
             bottomRight = BottomRight;

    int x, y;
    Vector2i loc;
    for (x = topLeft.x; x <= bottomRight.x; ++x)
    {
        loc.x = x;

        if (Wrap)
            loc.x = noise.Wrap(loc).x;
        else if (x < 0 || x >= noise.GetWidth())
            continue;

        for (y = topLeft.y; y <= bottomRight.y; ++y)
        {
            loc.y = y;

            if (Wrap)
                loc.y = noise.Wrap(loc).y;
            else if (y < 0 || y >= noise.GetHeight())
                continue;

            if (ActiveIn.Touches(noise[loc]))
                toDo(pData, loc, StrengthLerp);
        }
    }
}