#include "Boxes.h"

#include "../Higher Math/Geometryf.h"


//TODO: Optimize by replacing use of x/y/z intervals with direct code.

//Basic initialized value.
float Box2D::GeometricError = 0.001f;

bool Box2D::FixDimensions()
{
	bool neededFixing = false;

	if (width < 0.0f)
	{
		x += width;
		width *= -1.0f;
		neededFixing = true;
	}

	if (height < 0.0f)
	{
		y += height;
		height *= -1.0f;
		neededFixing = true;
	}

	return neededFixing;
}

bool Box2D::Touches(const Box2D & other) const
{
    float xMinO = other.GetXMin(),
          xMaxO = other.GetXMax(),
          yMinO = other.GetYMin(),
          yMaxO = other.GetYMax(),
          xMinT = GetXMin(),
          xMaxT = GetXMax(),
          yMinT = GetYMin(),
          yMaxT = GetYMax();
    return (((xMinO >= xMinT && xMinO <= xMaxT) || (xMaxO >= xMinT && xMaxO <= xMaxT)) &&
            ((yMinO >= yMinT && yMinO <= yMaxT) || (yMaxO >= yMinT && yMaxO <= yMaxT))) ||
           (((xMinT >= xMinO && xMinT <= xMaxO) || (xMaxT >= xMinO && xMaxT <= xMaxO)) &&
            ((yMinT >= yMinO && yMinT <= yMaxO) || (yMaxT >= yMinO && yMaxT <= yMaxO)));
}
bool Box2D::IsInside(const Box2D & other) const
{
    Vector2f min = GetMinCorner(),
             max = GetMaxCorner();
	return other.IsPointInside(min) &&
		   other.IsPointInside(Vector2f(min.x, max.y)) &&
		   other.IsPointInside(Vector2f(max.x, min.y)) &&
		   other.IsPointInside(max);
}

bool Box2D::IsPointOnEdge(Vector2f point) const
{
	return ((WithinError(GetXMin(), point.x) || WithinError(GetXMax(), point.x)) &&
			(point.y >= GetYMin() && point.y <= GetYMax())) ||
		   ((WithinError(GetYMin(), point.y) || WithinError(GetYMax(), point.y)) &&
			(point.x >= GetXMin() && point.x <= GetXMax()));
}

bool Box2D::IsEqual(const Box2D & other) const
{
	return WithinError(x, other.x) && WithinError(y, other.y) &&
		   WithinError(width, other.width) && WithinError(height, other.height);
}

bool Box2D::CastRay(Vector2f start, Vector2f dir, Vector2f& outHitPos, float& outHitT) const
{
    Vector2f min = GetMinCorner(),
             max = GetMaxCorner();

    //Exit early if the ray has no chance of hitting.
    if ((dir.x < 0.0f && start.x < min.x) || (dir.x > 0.0f && start.x > max.x) ||
        (dir.y < 0.0f && start.y < min.y) || (dir.y > 0.0f && start.y > max.y))
    {
        return false;
    }

    //Find up to 4 different intersections -- one for each face.
    float smallestT = std::numeric_limits<float>::infinity();
    Vector2f smallestPos;

    Interval xInt = GetXInterval(),
             yInt = GetYInterval();
    
    //For reference:
    // X' = Vt + X
    // t = (X' - X) / V

    //X faces.
    if (dir.x != 0)
    {
        //Min X face.
        float tempT = (min.x - start.x) / dir.x;
        float y = (dir.y * tempT) + start.y;
        if (tempT >= 0.0f && yInt.IsInside(y))
        {
            smallestT = tempT;
            smallestPos = Vector2f(min.x, y);
        }

        //Max X face.
        tempT = (max.x - start.x) / dir.x;
        y = (dir.y * tempT) + start.y;
        if (tempT >= 0.0f && yInt.IsInside(y) && smallestT > tempT)
        {
            smallestT = tempT;
            smallestPos = Vector2f(max.x, y);
        }
    }
    if (dir.y != 0)
    {
        //Min Y face.
        float tempT = (min.y - start.y) / dir.y;
        float x = (dir.x * tempT) + start.x;
        if (tempT >= 0.0f && xInt.IsInside(x) && smallestT > tempT)
        {
            smallestT = tempT;
            smallestPos = Vector2f(x, min.y);
        }

        //Max Y face.
        tempT = (max.y - start.y) / dir.y;
        x = (dir.x * tempT) + start.x;
        if (tempT >= 0.0f && xInt.IsInside(x) && smallestT > tempT)
        {
            smallestT = tempT;
            smallestPos = Vector2f(x, max.y);
        }
    }

    //Return the result.
    if (smallestT == std::numeric_limits<float>::infinity())
    {
        return false;
    }
    else
    {
        outHitPos = smallestPos;
        outHitT = smallestT;
        return true;
    }
}

unsigned int Box2D::CastRay(Vector2f start, Vector2f dir,
                            Vector2f& outPos1, float& outT1, Vector2f& outPos2, float& outT2) const
{
    Vector2f min = GetMinCorner(),
             max = GetMaxCorner();

    //Exit early if the ray has no chance of hitting.
    if ((dir.x < 0.0f && start.x < min.x) || (dir.x > 0.0f && start.x > max.x) ||
        (dir.y < 0.0f && start.y < min.y) || (dir.y > 0.0f && start.y > max.y))
    {
        return false;
    }

    //Find up to 4 different intersections.
    const float infinity = std::numeric_limits<float>::infinity();
    float smallestT[4] = { infinity, infinity, infinity, infinity };
    Vector2f smallestPos[4] = { Vector2f(), Vector2f(), Vector2f(), Vector2f() };

    Interval xInt = GetXInterval(),
             yInt = GetYInterval();
    
    //For reference:
    // X' = Vt + X
    // t = (X' - X) / V

    //X faces.
    if (dir.x != 0)
    {
        //Min X face.
        float tempT = (min.x - start.x) / dir.x;
        float y = (dir.y * tempT) + start.y;
        if (tempT >= 0.0f && yInt.IsInside(y))
        {
            smallestT[0] = tempT;
            smallestPos[0] = Vector2f(min.x, y);
        }

        //Max X face.
        tempT = (max.x - start.x) / dir.x;
        y = (dir.y * tempT) + start.y;
        if (tempT >= 0.0f && yInt.IsInside(y))
        {
            smallestT[1] = tempT;
            smallestPos[1] = Vector2f(max.x, y);
        }
    }
    if (dir.y != 0)
    {
        //Min Y face.
        float tempT = (min.y - start.y) / dir.y;
        float x = (dir.x * tempT) + start.x;
        if (tempT >= 0.0f && xInt.IsInside(x))
        {
            smallestT[2] = tempT;
            smallestPos[2] = Vector2f(x, min.y);
        }

        //Max Y face.
        tempT = (max.y - start.y) / dir.y;
        x = (dir.x * tempT) + start.x;
        if (tempT >= 0.0f && xInt.IsInside(x))
        {
            smallestT[3] = tempT;
            smallestPos[3] = Vector2f(x, max.y);
        }
    }


    //Assume that there's either 0, 1, or 2 intersections.
    //Find the intersections.
    unsigned int first = 4, second = 4;
    for (unsigned int i = 0; i < 4; ++i)
    {
        if (smallestT[i] != infinity)
        {
            if (first == 4)
            {
                first = i;
            }
            else
            {
                assert(second == 4);

                if (smallestT[i] < smallestT[first])
                {
                    second = first;
                    first = i;
                }
                else
                {
                    second = i;
                }
            }
        }
    }

    //Figure out what to return.
    if (first == 4)
    {
        return 0;
    }
    else if (second == 4)
    {
        outT1 = smallestT[first];
        outPos1 = smallestPos[first];
        return 1;
    }
    else
    {
        outT1 = smallestT[first];
        outPos1 = smallestPos[first];
        outT2 = smallestT[second];
        outPos2 = smallestPos[second];
        return 2;
    }
}




//Basic initialized value.
float Box3D::GeometricError = 0.001f;

bool Box3D::FixDimensions()
{
	bool neededFixing = false;

	if (dimensions.x < 0.0f)
	{
		minCorner.x += dimensions.x;
		dimensions.x = -dimensions.x;
		neededFixing = true;
	}

	if (dimensions.y < 0.0f)
	{
		minCorner.y += dimensions.y;
		dimensions.y = -dimensions.y;
		neededFixing = true;
	}

	if (dimensions.z < 0.0f)
	{
		minCorner.z += dimensions.z;
		dimensions.z = -dimensions.z;
		neededFixing = true;
	}

	return neededFixing;
}

bool Box3D::Touches(const Box3D & other) const
{
    float xMinO = other.GetXMin(),
        xMaxO = other.GetXMax(),
        yMinO = other.GetYMin(),
        yMaxO = other.GetYMax(),
        zMinO = other.GetZMin(),
        zMaxO = other.GetZMax(),
        xMinT = GetXMin(),
        xMaxT = GetXMax(),
        yMinT = GetYMin(),
        yMaxT = GetYMax(),
        zMinT = GetZMin(),
        zMaxT = GetZMax();
    return (((xMinO >= xMinT && xMinO <= xMaxT) || (xMaxO >= xMinT && xMaxO <= xMaxT)) &&
            ((yMinO >= yMinT && yMinO <= yMaxT) || (yMaxO >= yMinT && yMaxO <= yMaxT)) &&
            ((zMinO >= zMinT && zMinO <= zMaxT) || (zMaxO >= zMinT && zMaxO <= zMaxT))) ||
           (((xMinT >= xMinO && xMinT <= xMaxO) || (xMaxT >= xMinO && xMaxT <= xMaxO)) &&
            ((yMinT >= yMinO && yMinT <= yMaxO) || (yMaxT >= yMinO && yMaxT <= yMaxO)) &&
            ((zMinT >= zMinO && zMinT <= zMaxO) || (zMaxT >= zMinO && zMaxT <= zMaxO)));
	return GetXInterval().Touches(other.GetXInterval()) &&
		   GetYInterval().Touches(other.GetYInterval()) &&
		   GetZInterval().Touches(other.GetZInterval());
}
bool Box3D::IsInside(const Box3D & other) const
{
    return other.IsPointInside(GetMinCorner()) &&
           other.IsPointInside(GetMaxCorner());
    Vector3f min = GetMinCorner(),
             max = GetMaxCorner();
	return other.IsPointInside(min) &&
		   other.IsPointInside(Vector3f(min.x, min.y, max.z)) &&
           other.IsPointInside(Vector3f(min.x, max.y, min.z)) &&
           other.IsPointInside(Vector3f(min.x, max.y, max.z)) &&
           other.IsPointInside(Vector3f(max.x, min.y, min.z)) &&
           other.IsPointInside(Vector3f(max.x, min.y, max.z)) &&
           other.IsPointInside(Vector3f(max.x, max.y, min.z)) &&
		   other.IsPointInside(max);
}

bool Box3D::PointTouches(Vector3f point) const
{
    return (point.x >= GetXMin() && point.y >= GetYMin() && point.z >= GetZMin() &&
            point.x <= GetXMax() && point.y <= GetYMax() && point.z <= GetZMax());
	return (GreaterThanOrEqual(point.x, GetXMin()) &&
			GreaterThanOrEqual(point.y, GetYMin()) &&
			GreaterThanOrEqual(point.z, GetZMin()) &&
			LessThanOrEqual(point.x, GetXMax()) &&
			LessThanOrEqual(point.y, GetYMax()) &&
			LessThanOrEqual(point.z, GetZMax()));
}
bool Box3D::IsPointInside(Vector3f point) const
{
    return (point.x > GetXMin() && point.y > GetYMin() && point.z > GetZMin() &&
            point.x < GetXMax() && point.y < GetYMax() && point.z < GetZMax());
	return (GreaterThan(point.x, GetXMin()) &&
			GreaterThan(point.y, GetYMin()) &&
			GreaterThan(point.z, GetZMin()) &&
			LessThan(point.x, GetXMax()) &&
			LessThan(point.y, GetYMax()) &&
			LessThan(point.z, GetZMax()));
}
bool Box3D::IsPointOnFace(Vector3f point) const
{
	return ((WithinError(GetXMin(), point.x) || WithinError(GetXMax(), point.x)) &&
			(point.y >= GetYMin() && point.y <= GetYMax()) &&
			(point.z >= GetZMin() && point.z <= GetZMax())) ||
		   ((WithinError(GetYMin(), point.y) || WithinError(GetYMax(), point.y)) &&
			(point.x >= GetXMin() && point.x <= GetXMax()) &&
            (point.z >= GetZMin() && point.z <= GetZMax())) ||
		   ((WithinError(GetZMin(), point.z) || WithinError(GetZMax(), point.z)) &&
            (point.x >= GetXMin() && point.x <= GetXMax()) &
            (point.y >= GetYMin() && point.y <= GetYMax()));
}

bool Box3D::IsEqual(const Box3D & other) const
{
	return WithinError(minCorner.x, other.minCorner.x) &&
		   WithinError(minCorner.y, other.minCorner.y) &&
		   WithinError(minCorner.z, other.minCorner.z) &&
		   WithinError(dimensions.x, other.dimensions.x) &&
		   WithinError(dimensions.y, other.dimensions.y) &&
		   WithinError(dimensions.z, other.dimensions.z);
}