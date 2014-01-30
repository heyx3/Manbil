#include "Boxes.h"

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
	return GetXInterval().Touches(other.GetXInterval()) &&
		   GetYInterval().Touches(other.GetYInterval());
}
bool Box2D::IsInside(const Box2D & other) const
{
	return other.IsPointInside(GetTopLeft()) &&
		   other.IsPointInside(GetTopRight()) &&
		   other.IsPointInside(GetBottomLeft()) &&
		   other.IsPointInside(GetBottomRight());
}

bool Box2D::PointTouches(Vector2f point) const
{
	return (GreaterThanOrEqual(point.x, GetLeftSide()) &&
			GreaterThanOrEqual(point.y, GetTopSide()) &&
			LessThanOrEqual(point.x, GetRightSide()) &&
			LessThanOrEqual(point.y, GetBottomSide()));
}
bool Box2D::IsPointInside(Vector2f point) const
{
	return (GreaterThan(point.x, GetLeftSide()) &&
			GreaterThan(point.y, GetTopSide()) &&
			LessThan(point.x, GetRightSide()) &&
			LessThan(point.y, GetBottomSide()));
}
bool Box2D::IsPointOnEdge(Vector2f point) const
{
	return ((WithinError(GetLeftSide(), point.x) || WithinError(GetRightSide(), point.x)) &&
			(GetYInterval().Touches(point.y))) ||
		   ((WithinError(GetTopSide(), point.y) || WithinError(GetBottomSide(), point.y)) &&
			(GetXInterval().Touches(point.x)));
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
		topLeftFront.x += dimensions.x;
		dimensions.x = -dimensions.x;
		neededFixing = true;
	}

	if (dimensions.y < 0.0f)
	{
		topLeftFront.y += dimensions.y;
		dimensions.y = -dimensions.y;
		neededFixing = true;
	}

	if (dimensions.z < 0.0f)
	{
		topLeftFront.z += dimensions.z;
		dimensions.z = -dimensions.z;
		neededFixing = true;
	}

	return neededFixing;
}

bool Box3D::Touches(const Box3D & other) const
{
	return GetXInterval().Touches(other.GetXInterval()) &&
		   GetYInterval().Touches(other.GetYInterval()) &&
		   GetZInterval().Touches(other.GetZInterval());
}
bool Box3D::IsInside(const Box3D & other) const
{
	return other.IsPointInside(GetTopLeftFront()) &&
		   other.IsPointInside(GetTopRightFront()) &&
		   other.IsPointInside(GetBottomLeftFront()) &&
		   other.IsPointInside(GetBottomRightFront()) &&
		   other.IsPointInside(GetTopLeftBack()) &&
		   other.IsPointInside(GetTopRightBack()) &&
		   other.IsPointInside(GetBottomLeftBack()) &&
		   other.IsPointInside(GetBottomRightBack());
}

bool Box3D::PointTouches(Vector3f point) const
{
	return (GreaterThanOrEqual(point.x, GetLeftSide()) &&
			GreaterThanOrEqual(point.y, GetTopSide()) &&
			GreaterThanOrEqual(point.z, GetFrontSide()) &&
			LessThanOrEqual(point.x, GetRightSide()) &&
			LessThanOrEqual(point.y, GetBottomSide()) &&
			LessThanOrEqual(point.z, GetBackSide()));
}
bool Box3D::IsPointInside(Vector3f point) const
{
	return (GreaterThan(point.x, GetLeftSide()) &&
			GreaterThan(point.y, GetTopSide()) &&
			GreaterThan(point.z, GetFrontSide()) &&
			LessThan(point.x, GetRightSide()) &&
			LessThan(point.y, GetBottomSide()) &&
			LessThan(point.z, GetBackSide()));
}
bool Box3D::IsPointOnFace(Vector3f point) const
{
	return ((WithinError(GetLeftSide(), point.x) || WithinError(GetRightSide(), point.x)) &&
			GetYInterval().Touches(point.y) &&
			GetZInterval().Touches(point.z)) ||
		   ((WithinError(GetTopSide(), point.y) || WithinError(GetBottomSide(), point.y)) &&
			GetXInterval().Touches(point.x) &&
			GetZInterval().Touches(point.z)) ||
		   ((WithinError(GetFrontSide(), point.z) || WithinError(GetBackSide(), point.z)) &&
		    GetXInterval().Touches(point.x) &
			GetYInterval().Touches(point.y));
}

bool Box3D::IsEqual(const Box3D & other) const
{
	return WithinError(topLeftFront.x, other.topLeftFront.x) &&
		   WithinError(topLeftFront.y, other.topLeftFront.y) &&
		   WithinError(topLeftFront.z, other.topLeftFront.z) &&
		   WithinError(dimensions.x, other.dimensions.x) &&
		   WithinError(dimensions.y, other.dimensions.y) &&
		   WithinError(dimensions.z, other.dimensions.z);
}