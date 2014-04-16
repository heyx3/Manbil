#pragma once

#include "../LowerMath.hpp"
#include "../Shapes/Boxes.h"


//Represents a way to select a region of 3D noise to be filtered.
class NoiseFilterVolume
{
public:

	typedef void (*ActionFunc)(void* pData, Vector3i location, float strength);

	//The strength of the filter this region applies to, from 0 to 1.
	float StrengthLerp;
	//This region only acts on noise values that are inside this range.
	Interval ActiveIn;
    //Whether this region should wrap around the edges of the noise.
    bool Wrap;

    NoiseFilterVolume(float strengthLerp = 1.0f, Interval activeIn = Interval(0.0f, GetMaxFloat()), bool wrap = false) : Wrap(wrap), ActiveIn(activeIn), StrengthLerp(strengthLerp) { }

	virtual void DoToEveryPoint(void* pData, ActionFunc toDo, const Fake3DArray<float> & noise, Vector3i noiseSize, bool calcStrengthDropoff = true) = 0;

protected:
    static float GetMaxFloat(void) { return std::numeric_limits<float>().max(); }
};

//Represents the entire noise area.
class MaxFilterVolume : public NoiseFilterVolume
{
public:

    MaxFilterVolume(float strengthLerp = 1.0f, Interval activeIn = Interval(0.0f, GetMaxFloat())) : NoiseFilterVolume(strengthLerp, activeIn, false) { }

	virtual void DoToEveryPoint(void* pData, ActionFunc toDo, const Fake3DArray<float> & noise, Vector3i noiseSize, bool calcStrengthDropoff = true) override;
};

//Represents a spherical area.
class SphereFilterVolume : public NoiseFilterVolume
{
public:

	Vector3f Center;
	float Radius;

	//0 means no dropoff, 1 means dropoff starts at center moving outwards. -1 means dropoff starts at radius moving inwards.
	float DropoffRadiusPercent;

    SphereFilterVolume(Vector3f center, float radius, float strengthLerp = 1.0f, float dropoffRadiusPercent = 0.0f, Interval activeIn = Interval(0.0f, GetMaxFloat()), bool wrap = false)
        : NoiseFilterVolume(strengthLerp, activeIn, wrap), Center(center), Radius(radius), DropoffRadiusPercent(dropoffRadiusPercent)
    { }

	virtual void DoToEveryPoint(void* pData, ActionFunc toDo, const Fake3DArray<float> & noise, Vector3i noiseSize, bool calcStrengthDropoff = true) override;
	
private:

	float GetStrengthDropoffScale(Vector3f pos) const;
};

//Represents a cube area.
class CubeFilterVolume : public NoiseFilterVolume
{
public:

    Vector3i MinCorner, MaxCorner;

    CubeFilterVolume(Vector3i minCorner, Vector3i maxCorner, float strengthLerp = 1.0f, Interval activeIn = Interval(0.0f, GetMaxFloat()), bool wrap = false)
		: NoiseFilterVolume(strengthLerp, activeIn, wrap), MinCorner(minCorner), MaxCorner(maxCorner) { }

	virtual void DoToEveryPoint(void* pData, ActionFunc toDo, const Fake3DArray<float> & noise, Vector3i noiseSize, bool calcStrengthDropoff = true) override;
};