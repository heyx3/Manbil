#include "Boxes.h"


//TODO: Optimize by replacing use of x/y/z intervals with direct code.

//Basic initialized value.
float Box2D::GeometricError = 0.01f;

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





//Basic initialized value.
float Box3D::GeometricError = 0.01f;

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