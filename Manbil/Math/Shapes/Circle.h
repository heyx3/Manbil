#pragma once

#include "../Lower Math/Vectors.h"


//Describes a 2D circle.
class Circle
{
public:

    Vector2f Pos;
    float Radius;

    Circle(Vector2f pos = Vector2f(), float radius = 0.0f) : Pos(pos), Radius(radius) { }


    bool IsPointInside(Vector2f point) const { return Pos.DistanceSquared(point) <= Radius * Radius; }
    inline bool IsTouchingCircle(const Circle & other) const
    {
        float maxDist = Radius + other.Radius;
        return Pos.DistanceSquared(other.Pos) <= (maxDist * maxDist);
    }

    //Gets all intersections between this circle and the given line segment.
    //Stores the intersections in "outIntersections" and returns the number of them (0-2).
    unsigned int GetIntersections(Vector2f segment1, Vector2f segment2, Vector2f outIntersections[2]) const;

    //Gets whether the given line segment touches this circle at all.
    bool DoesIntersect(Vector2f segment1, Vector2f segment2) const;
};