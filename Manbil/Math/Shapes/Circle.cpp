#include "Circle.h"


unsigned int Circle::GetIntersections(Vector2f p1, Vector2f p2, Vector2f outPs[2]) const
{
    //Transform so that this circle is at the origin.
    p1 -= Pos;
    p2 -= Pos;

    float determinant = (p1.x * p2.y) - (p2.x * p1.y);
    float segmentLenSqr = p1.DistanceSquared(p2);
    float discriminant = (Radius * Radius * segmentLenSqr) -
                         (determinant * determinant);

    if (discriminant < -0.01f) return 0;
    else
    {
        float distY = p2.y - p1.y,
              distX = p2.x - p1.x;

        float invSegmentLenSqr = 1.0f / segmentLenSqr;
        float xPart1 = determinant * distY,
              yPart1 = -determinant * distX;


        if (discriminant > 0.0f)
        {
            float xPart2 = (distY < 0.0f ? -1.0f : 1.0f) * distX * sqrtf(discriminant),
                  yPart2 = fabsf(distY) * sqrtf(discriminant);

            outPs[0] = Vector2f((xPart1 + xPart2) * invSegmentLenSqr,
                                (yPart1 + yPart2) * invSegmentLenSqr);
            outPs[1] = Vector2f((xPart1 - xPart2) * invSegmentLenSqr,
                                (yPart1 - yPart2) * invSegmentLenSqr);

            outPs[0] += Pos;
            outPs[1] += Pos;

            return 2;
        }
        else
        {
            outPs[0] = Vector2f(xPart1 * invSegmentLenSqr,
                                yPart1 * invSegmentLenSqr);
            outPs[0] += Pos;
            return 1;
        }
    }
}
bool Circle::DoesIntersect(Vector2f p1, Vector2f p2) const
{
	//Transform so that this circle is at the origin.
	p1 -= Pos;
	p2 -= Pos;

	float determinant = (p1.x * p2.y) - (p2.x * p1.y);
	float segmentLenSqr = p1.DistanceSquared(p2);
	float discriminant = (Radius * Radius * segmentLenSqr) -
	                    (determinant * determinant);

	return discriminant >= -0.01f;
}