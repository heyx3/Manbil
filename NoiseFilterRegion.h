#pragma once

#include "Vectors.h"
#include "Interval.h"
#include "Fake2DArray.h"

//Represents a way to select a region of 2D noise to be filtered.
class NoiseFilterRegion
{
public:

	typedef void (*ActionFunc)(void* pData, Vector2i location, float strength);

	//The strength of the filter this region applies to, from 0 to 1.
	float StrengthLerp;
	//This region only acts on noise values that are inside this range.
	Interval ActiveIn;

	NoiseFilterRegion(float strengthLerp = 1.0f, Interval activeIn = Interval(0.0f, 99999.0f)) : ActiveIn(activeIn), StrengthLerp(strengthLerp) { }

	virtual void DoToEveryPoint(void* pData, ActionFunc toDo, const Fake2DArray<float> & noise, Vector2i noiseSize, bool calcStrengthDropoff = true) = 0;
};

//Represents the entire noise area.
class MaxFilterRegion : public NoiseFilterRegion
{
public:

	MaxFilterRegion(float strengthLerp = 1.0f, Interval activeIn = Interval(0.0f, 999999.0f)) : NoiseFilterRegion(strengthLerp, activeIn) { }

	virtual void DoToEveryPoint(void* pData, ActionFunc toDo, const Fake2DArray<float> & noise, Vector2i noiseSize, bool calcStrengthDropoff = true) override;
};

//Represents a circular area.
class CircularFilterRegion : public NoiseFilterRegion
{
public:

	Vector2f Center;
	float Radius;

	//0 means no dropoff, 1 means dropoff starts at center moving outwards. -1 means dropoff starts at radius moving inwards.
	float DropoffRadiusPercent;

	CircularFilterRegion(Vector2f center, float radius, float strengthLerp = 1.0f, float dropoffRadiusPercent = 0.0f, Interval activeIn = Interval(0.0f, 99999.0f))
		: NoiseFilterRegion(strengthLerp, activeIn), Center(center), Radius(radius), DropoffRadiusPercent(dropoffRadiusPercent) { }

	virtual void DoToEveryPoint(void* pData, ActionFunc toDo, const Fake2DArray<float> & noise, Vector2i noiseSize, bool calcStrengthDropoff = true) override;
	
private:

	float GetStrengthDropoffScale(Vector2f pos) const;
};

//Represents a square area.
class RectangularFilterRegion : public NoiseFilterRegion
{
public:

	Vector2i TopLeft, BottomRight;

	RectangularFilterRegion(Vector2i topLeft, Vector2i bottomRight, float strengthLerp = 1.0f, Interval activeIn = Interval(0.0f, 99999.0f))
		: NoiseFilterRegion(strengthLerp, activeIn), TopLeft(topLeft), BottomRight(bottomRight) { }

	virtual void DoToEveryPoint(void* pData, ActionFunc toDo, const Fake2DArray<float> & noise, Vector2i noiseSize, bool calcStrengthDropoff = true) override;
};