#pragma once

#include <math.h>
#include "Vectors.h"
#include "Interval.h"

//Represents a rectangle shape useful for 2D collision.
class Box2D
{
public:
	
	//The margin of error for comparing equality between two floats.
	//This is generally bad practice, but the margin of error must be consistent across all rectangles and must be choosable by the user of this code, so I made it a public static variable.
	//It is up to the user whether or not this error margin should be an invariant (e.x. he may want to change it in between levels for some reason).
	static float GeometricError;

	//Makes a rectangle from the given center position and dimensions.
	static void RectByCenter(Box2D & out, float centerX, float centerY, float width, float height) { out.x = centerX - (width * 0.5f); out.y = centerY - (height * 0.5f); out.width = width; out.height = height; }
	//Makes a rectangle from the given top-left-corner position and dimensions.
	Box2D(float x, float y, float width, float height) { this->x = x; this->y = y; this->width = width; this->height = height; }
	//Default constructor makes an empty Box2D.
	Box2D() { x = 0; y = 0; width = 0; height = 0; }
	
	//If the rectangle has a negative width/height, flips it to make it positive.
	//Returns whether it needed to be flipped at all.
	bool FixDimensions();

	//Finds if this Box2D touches the given one.
	bool Touches(const Box2D & other) const;
	//Finds if this Box2D is inside the given one.
	bool IsInside(const Box2D & other) const;

	//Finds if the given point touches this Box2D (on its edge or inside it).
	bool PointTouches(Vector2f point) const;
	//Finds if the given point is inside this Box2D.
	bool IsPointInside(Vector2f point) const;
	//Finds if the given point is on an edge of this Box2D.
	bool IsPointOnEdge(Vector2f point) const;

	//Finds if this Box2D is equal to the given one.
	bool IsEqual(const Box2D & other) const;


	//Getters/setters.

	Vector2f GetPosition(void) const { return Vector2f(x, y); }
	void SetCenterOfBox(Vector2f pos) { x = pos.x - (width * 0.5f); y = pos.y - (height * 0.5f); }
	void Move(float deltaX, float deltaY) { x += deltaX; y += deltaY; }

	float GetWidth(void) const { return width; }
	float GetHeight(void) const { return height; }
	Vector2f GetDimensions(void) const { return Vector2f(width, height); }
	void SetWidth(float newWidth) { width = newWidth; }
	void SetHeight(float newHeight) { height = newHeight; }
	void SetDimensions(Vector2f newWidthAndHeight) { width = newWidthAndHeight.x; height = newWidthAndHeight.y; }

	float GetLeftSide(void) const { return x; }
	float GetRightSide(void) const { return x + width; }
	float GetTopSide(void) const { return y; }
	float GetBottomSide(void) const { return y + height; }

	Vector2f GetTopLeft(void) const { return Vector2f(GetLeftSide(), GetTopSide()); }
	Vector2f GetTopRight(void) const { return Vector2f(GetRightSide(), GetTopSide()); }
	Vector2f GetBottomLeft(void) const { return Vector2f(GetLeftSide(), GetBottomSide()); }
	Vector2f GetBottomRight(void) const { return Vector2f(GetRightSide(), GetBottomSide()); }

	float GetCenterX(void) const { return x + (width * 0.5f); }
	float GetCenterY(void) const { return y + (height * 0.5f); }
	Vector2f GetCenter(void) const { return Vector2f(GetCenterX(), GetCenterY()); }

	Interval GetXInterval(void) const { return Interval(GetLeftSide(), GetRightSide(), 0.001f, true, true); }
	Interval GetYInterval(void) const { return Interval(GetTopSide(), GetBottomSide(), 0.001f, true, true); }

private:

	float x, y, width, height;
	
	//Finds if the given floats are within "GeometricError" from each other.
	static bool WithinError(float f1, float f2) { return BasicMath::Abs(f1 - f2) <= GeometricError; }
	//Finds if the first value is greater than the second value or within error of it.
	static bool GreaterThanOrEqual(float f1, float f2) { return (WithinError(f1, f2) || f1 > f2); }
	//Finds if the first value is less than the second value or within error of it.
	static bool LessThanOrEqual(float f1, float f2) { return (WithinError(f1, f2) || f1 < f2); }
	//Finds if the first value is greater than the second value and NOT within error of it.
	static bool GreaterThan(float f1, float f2) { return (!WithinError(f1, f2) && f1 > f2); }
	//Finds if the first value is less than the second value and NOT within error of it.
	static bool LessThan(float f1, float f2) { return (!WithinError(f1, f2) && f1 < f2); }

};

//Represents a rectangle shape useful for 3D collision.
class Box3D
{
public:
	
	//The margin of error for comparing equality between two floats.
	//This is generally bad practice, but the margin of error must be consistent across all Box3D's and must be choosable by the user of this code, so I made it a public static variable.
	//It is up to the user whether or not this error margin should be an invariant (e.x. he may want to change it in between levels for some reason).
	static float GeometricError;

	//Makes a rectangle from the given center position and dimensions.
	static void RectByCenter(Box3D & out, Vector3f center, Vector3f dimensions) { out.topLeftFront = center - (dimensions * 0.5f); out.dimensions = dimensions; }
	//Makes a rectangle from the given top-left-front corner and dimensions.
	Box3D(Vector3f topLeftFrontCorner, Vector3f _dimensions): topLeftFront(topLeftFrontCorner), dimensions(_dimensions) { }
	
	//If the rectangle has a negative width/height/depth, flips it to make it positive.
	//Returns whether it needed to be flipped at all.
	bool FixDimensions();

	//Finds if this Box3D touches the given one.
	bool Touches(const Box3D & other) const;
	//Finds if this Box3D is inside the given one.
	bool IsInside(const Box3D & other) const;

	//Finds if the given point touches this Box3D (on its edge or inside it).
	bool PointTouches(Vector3f point) const;
	//Finds if the given point is inside this Box3D.
	bool IsPointInside(Vector3f point) const;
	//Finds if the given point is on a face of this Box3D.
	bool IsPointOnFace(Vector3f point) const;

	//Finds if this Box3D is equal to the given one.
	bool IsEqual(const Box3D & other) const;


	//Getters/setters.

	void SetCenterOfBox(Vector3f pos) { topLeftFront += (pos - GetCenter()); }
	void Move(Vector3f amount) { topLeftFront += amount; }

	float GetWidth(void) const { return dimensions.x; }
	float GetHeight(void) const { return dimensions.y; }
	float GetDepth(void) const { return dimensions.z; }
	Vector3f GetDimensions(void) const { return dimensions; }
	void SetWidth(float newWidth) { dimensions.x = newWidth; }
	void SetHeight(float newHeight) { dimensions.y = newHeight; }
	void SetDepth(float newDepth) { dimensions.z = newDepth; }
	void SetDimensions(Vector3f newDimensions) { dimensions = newDimensions; }

	float GetLeftSide(void) const { return topLeftFront.x; }
	float GetRightSide(void) const { return topLeftFront.x + dimensions.x; }
	float GetTopSide(void) const { return topLeftFront.y; }
	float GetBottomSide(void) const { return topLeftFront.y + dimensions.y; }
	float GetFrontSide(void) const { return topLeftFront.z; }
	float GetBackSide(void) const { return topLeftFront.z + dimensions.z; }

	Vector3f GetTopLeftFront(void) const { return topLeftFront; }
	Vector3f GetTopLeftBack(void) const { return Vector3f(topLeftFront.x, topLeftFront.y, topLeftFront.z + dimensions.z); }
	Vector3f GetTopRightFront(void) const { return Vector3f(topLeftFront.x + dimensions.x, topLeftFront.y, topLeftFront.z); }
	Vector3f GetTopRightBack(void) const { return Vector3f(topLeftFront.x + dimensions.x, topLeftFront.y, topLeftFront.z + dimensions.z); }
	Vector3f GetBottomLeftFront(void) const { return Vector3f(topLeftFront.x, topLeftFront.y + dimensions.y, topLeftFront.z); }
	Vector3f GetBottomLeftBack(void) const { return Vector3f(topLeftFront.x, topLeftFront.y + dimensions.y, topLeftFront.z + dimensions.z); }
	Vector3f GetBottomRightFront(void) const { return Vector3f(topLeftFront.x + dimensions.x, topLeftFront.y + dimensions.y, topLeftFront.z); }
	Vector3f GetBottomRightBack(void) const { return Vector3f(topLeftFront.x + dimensions.x, topLeftFront.y + dimensions.y, topLeftFront.z + dimensions.z); }
	
	float GetCenterX(void) const { return topLeftFront.x + (0.5f * dimensions.x); }
	float GetCenterY(void) const { return topLeftFront.y + (0.5f * dimensions.y); }
	float GetCenterZ(void) const { return topLeftFront.z + (0.5f * dimensions.z); }
	Vector3f GetCenter(void) const { return Vector3f(GetCenterX(), GetCenterY(), GetCenterZ()); }

	Interval GetXInterval(void) const { return Interval(GetLeftSide(), GetRightSide(), 0.001f, true, true); }
	Interval GetYInterval(void) const { return Interval(GetTopSide(), GetBottomSide(), 0.001f, true, true); }
	Interval GetZInterval(void) const { return Interval(GetFrontSide(), GetBackSide(), 0.001f, true, true); }

private:

	Vector3f topLeftFront, dimensions;
	
	//Finds if the given floats are within "GeometricError" from each other.
	static bool WithinError(float f1, float f2) { return BasicMath::Abs(f1 - f2) <= GeometricError; }
	//Finds if the first value is greater than the second value or within error of it.
	static bool GreaterThanOrEqual(float f1, float f2) { return (WithinError(f1, f2) || f1 > f2); }
	//Finds if the first value is less than the second value or within error of it.
	static bool LessThanOrEqual(float f1, float f2) { return (WithinError(f1, f2) || f1 < f2); }
	//Finds if the first value is greater than the second value and NOT within error of it.
	static bool GreaterThan(float f1, float f2) { return (!WithinError(f1, f2) && f1 > f2); }
	//Finds if the first value is less than the second value and NOT within error of it.
	static bool LessThan(float f1, float f2) { return (!WithinError(f1, f2) && f1 < f2); }
};

