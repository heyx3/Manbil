#pragma once

#include "../Interval.h"


//Represents a rectangle shape useful for 2D collision.
class Box2D
{
public:

    //The margin of error for comparing equality between two floats when testing Box3D intersections.
    static float GeometricError;

    //Makes a rectangle from the given top-left corner pos and the given dimensions.
    Box2D(float left, float top, Vector2f _dimensions)
        : x(left), y(top), width(_dimensions.x), height(_dimensions.y)
    {
        width = BasicMath::Abs(width);
        height = BasicMath::Abs(height);
    }
    //Makes a rectangle from the given center position and dimensions.
    Box2D(Vector2f center, Vector2f _dimensions)
        : Box2D(center.x - (_dimensions.x * 0.5f), center.y - (_dimensions.y * 0.5f), _dimensions)
    {

    }
    Box2D(float left, float right, float top, float bottom)
        : Box2D(left, top, Vector2f(right - left, bottom - top))
    {

    }


    //If the rectangle has a negative width/height, flips it to make it positive.
    //Returns whether it needed to be flipped at all.
    bool FixDimensions();

    //Finds if this Box2D touches the given one.
    bool Touches(const Box2D & other) const;
    //Finds if this Box2D is inside the given one.
    bool IsInside(const Box2D & other) const;

    //Finds if the given point touches this Box2D (on its edge or inside it).
    bool PointTouches(Vector2f point) const
    {
        return (point.x >= GetXMin() && point.y >= GetYMin() &&
                point.x <= GetXMax() && point.y <= GetYMax());
    }
    //Finds if the given point is inside this Box2D.
    bool IsPointInside(Vector2f point) const
    {
        return (point.x > GetXMin() && point.y > GetYMin() &&
                point.x < GetXMax() && point.y < GetYMax());
    }
    //Finds if the given point is on an edge of this Box2D.
    bool IsPointOnEdge(Vector2f point) const;

    //Finds if this Box2D is equal to the given one, within the static GeometricError value.
    bool IsEqual(const Box2D & other) const;


    //Getters/setters.

    Vector2f GetPosition(void) const { return Vector2f(x, y); }
    void SetCenterOfBox(Vector2f pos) { x = pos.x - (width * 0.5f); y = pos.y - (height * 0.5f); }
    void Move(float deltaX, float deltaY) { x += deltaX; y += deltaY; }

    //Moves the min and max corners of this box out by half the given amount.
    void Inflate(Vector2f amount) { x -= amount.x * 0.5f; y -= amount.y * 0.5f; width += amount.x; height += amount.y; }
    //Scales the size of this box by the given amount.
    void Scale(Vector2f amount) { Inflate(Vector2f(width * amount.x, height * amount.y) - Vector2f(width, height)); }

    float GetXSize(void) const { return width; }
    float GetYSize(void) const { return height; }
    Vector2f GetDimensions(void) const { return Vector2f(width, height); }
    void SetWidth(float newWidth) { width = newWidth; }
    void SetHeight(float newHeight) { height = newHeight; }
    void SetDimensions(Vector2f newWidthAndHeight) { width = newWidthAndHeight.x; height = newWidthAndHeight.y; }

    float GetXMin(void) const { return x; }
    float GetXMax(void) const { return x + width; }
    float GetYMin(void) const { return y; }
    float GetYMax(void) const { return y + height; }

    Vector2f GetMinCorner(void) const { return Vector2f(GetXMin(), GetYMin()); }
    Vector2f GetMaxCorner(void) const { return Vector2f(GetXMax(), GetYMax()); }

    float GetCenterX(void) const { return x + (width * 0.5f); }
    float GetCenterY(void) const { return y + (height * 0.5f); }
    Vector2f GetCenter(void) const { return Vector2f(GetCenterX(), GetCenterY()); }

    Interval GetXInterval(void) const { return Interval(GetXMin(), GetXMax(), 0.001f, true, true); }
    Interval GetYInterval(void) const { return Interval(GetYMin(), GetYMax(), 0.001f, true, true); }

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

    //The margin of error for comparing equality between two floats when testing Box3D intersections.
    static float GeometricError;


    //Makes a rectangle from the given top-front-left corner pos and the given dimensions.
    Box3D(float minX, float minY, float minZ, Vector3f _dimensions)
        : minCorner(Vector3f(minX, minY, minZ)), dimensions(_dimensions)
    {
        dimensions = Vector3f(BasicMath::Abs(dimensions.x), BasicMath::Abs(dimensions.y), BasicMath::Abs(dimensions.z));
    }
    //Makes a rectangle from the given center position and dimensions.
    Box3D(Vector3f center, Vector3f _dimensions)
        : Box3D(center.x - (_dimensions.x * 0.5f), center.y - (_dimensions.y * 0.5f), center.z - (_dimensions.z * 0.5f), _dimensions)
    {

    }
    //Makes a rectangle from the given border values.
    Box3D(float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
        : Box3D(minX, minY, minZ, Vector3f(maxX - minX, maxY - minY, maxZ - minZ))
    {

    }
    Box3D(void) : Box3D(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f) { }


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

    //Finds if this Box3D is equal to the given one, within the static GeometricError value.
    bool IsEqual(const Box3D & other) const;


    //Getters/setters.

    void SetCenterOfBox(Vector3f pos) { minCorner += (pos - GetCenter()); }
    void Move(Vector3f amount) { minCorner += amount; }

    //Moves the min and max corners of this box out by half the given amount.
    void Inflate(Vector3f amount) { minCorner -= (amount * 0.5f); dimensions += amount; }
    //Scales the size of this box by the given amount.
    void Scale(Vector3f amount) { Inflate(dimensions.ComponentProduct(amount) - dimensions); }

    float GetXSize(void) const { return dimensions.x; }
    float GetYSize(void) const { return dimensions.y; }
    float GetZSize(void) const { return dimensions.z; }
    Vector3f GetDimensions(void) const { return dimensions; }
    void SetWidth(float newWidth) { dimensions.x = newWidth; }
    void SetHeight(float newHeight) { dimensions.y = newHeight; }
    void SetDepth(float newDepth) { dimensions.z = newDepth; }
    void SetDimensions(Vector3f newDimensions) { dimensions = newDimensions; }

    float GetXMin(void) const { return minCorner.x; }
    float GetXMax(void) const { return minCorner.x + dimensions.x; }
    float GetYMin(void) const { return minCorner.y; }
    float GetYMax(void) const { return minCorner.y + dimensions.y; }
    float GetZMin(void) const { return minCorner.z; }
    float GetZMax(void) const { return minCorner.z + dimensions.z; }

    Vector3f GetMinCorner(void) const { return minCorner; }
    Vector3f GetMaxCorner(void) const { return minCorner + dimensions; }

    float GetCenterX(void) const { return minCorner.x + (0.5f * dimensions.x); }
    float GetCenterY(void) const { return minCorner.y + (0.5f * dimensions.y); }
    float GetCenterZ(void) const { return minCorner.z + (0.5f * dimensions.z); }
    Vector3f GetCenter(void) const { return Vector3f(GetCenterX(), GetCenterY(), GetCenterZ()); }

    Interval GetXInterval(void) const { return Interval(GetXMin(), GetXMax(), 0.001f, true, true); }
    Interval GetYInterval(void) const { return Interval(GetYMin(), GetYMax(), 0.001f, true, true); }
    Interval GetZInterval(void) const { return Interval(GetZMin(), GetZMax(), 0.001f, true, true); }


private:

    Vector3f minCorner, dimensions;

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

