#pragma once

#include <vector>
#include <memory>
#include "../../Math/Vectors.h"
#include "../../Math/Interval.h"

//Filters a Vector2f value in the range { -1, -1 }, { 1, 1 } so that certain small values are zeroed out.
class Deadzone
{
public:
	virtual Vector2f Filter(Vector2f inV) = 0;
};

typedef std::shared_ptr<Deadzone> DeadzonePtr;


//Doesn't filter the input at all.
class EmptyDeadzone : Deadzone
{
public:
	virtual Vector2f Filter(Vector2f inV) override { return inV; }
};

//Any input with magnitude less than "Radius" is zeroed out.
class CircleDeadzone : Deadzone
{
public:

	float Radius;
	CircleDeadzone(float radius) : Radius(radius) { }
	virtual Vector2f Filter(Vector2f inV) override
	{
		if (inV.LengthSquared() < (Radius * Radius)) return Vector2f();
		else return inV;
	}
};

//Any Y values smaller than "Cutoff" are zeroed out.
class HorizontalDeadzone : Deadzone
{
public:
	float Cutoff;
	HorizontalDeadzone(float cutoff = 0.01f) : Cutoff(cutoff) { }
	virtual Vector2f Filter(Vector2f inV) override
	{
        float sign = (float)BasicMath::Sign(inV.y);
        Interval goodRange(Cutoff, 1.0f, 0.0001f);

        inV.y = goodRange.Clamp(sign * inV.y);
        inV.y = sign * goodRange.MapValue(Interval(0.0f, 1.0f, 0.0001f), inV.y);

		return inV;
	}
};
//Any X values smaller than "Cutoff" are zeroed out.
class VerticalDeadzone : Deadzone
{
public:
	float Cutoff;
	VerticalDeadzone(float cutoff = 0.01f) : Cutoff(cutoff) { }
	virtual Vector2f Filter(Vector2f inV) override
	{
        float sign = (float)BasicMath::Sign(inV.x);
        Interval goodRange(Cutoff, 1.0f, 0.0001f);

        inV.x = goodRange.Clamp(sign * inV.x);
        inV.x = sign * goodRange.MapValue(Interval(0.0f, 1.0f, 0.0001f), inV.x);

        return inV;
	}
};


//Any Y values smaller than a calculated value are zeroed out.
//The calculated value is proportional to the magnitude of the X value.
class HorizontalCrossDeadzone : Deadzone
{
public:

	//The Y value cutoff at the beginning (X == 0) and the end (|X| == 1) of the value range.
	Interval YRange;
	HorizontalCrossDeadzone(Interval yRange) : YRange(yRange) { }
	virtual Vector2f Filter(Vector2f inV) override
	{
		float yLimit = YRange.RangeLerp(BasicMath::Abs(inV.x));
        float sign = (float)BasicMath::Sign(inV.y);

        Interval goodRange(yLimit, 1.0f, 0.0001f);
        inV.y = goodRange.Clamp(sign * inV.y);
        inV.y = sign * goodRange.MapValue(Interval(0.0f, 1.0f, 0.0001f), inV.y);

		return inV;
	}
};
//Any X values smaller than a calculated value are zeroed out.
//The calculated value is proportional to the magnitude of the X value.
class VerticalCrossDeadzone : Deadzone
{
public:

	//The X value cutoff at the beginning (Y == 0) and the end (|Y| == 1) of the value range.
	Interval XRange;
	VerticalCrossDeadzone(Interval xRange) : XRange(xRange) { }
	virtual Vector2f Filter(Vector2f inV) override
	{
        float xLimit = XRange.RangeLerp(BasicMath::Abs(inV.y));
        float sign = (float)BasicMath::Sign(inV.x);

        Interval goodRange(xLimit, 1.0f, 0.0001f);
        inV.x = goodRange.Clamp(sign * inV.x);
        inV.x = sign * goodRange.MapValue(Interval(0.0f, 1.0f, 0.0001f), inV.x);

        return inV;
	}
};


//Applies an arbitrary amount of deadzone filters to the input.
class MultiDeadzone : Deadzone
{
public:

	std::vector<DeadzonePtr> Deadzones;

	MultiDeadzone(std::vector<DeadzonePtr> deadzones) : Deadzones(deadzones) { }
	MultiDeadzone(DeadzonePtr one, DeadzonePtr two)
	{
		Deadzones.insert(Deadzones.end(), one);
		Deadzones.insert(Deadzones.end(), two);
	}

	virtual Vector2f Filter(Vector2f inV) override
	{
		for (unsigned int i = 0; i < Deadzones.size(); ++i)
			inV = Deadzones[i]->Filter(inV);
		return inV;
	}
};