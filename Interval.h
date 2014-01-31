#pragma once

//#include <algorithm>
#include <string>
#include "FastRand.h"
#include "Vectors.h"


class Interval
{
public:

	static Interval GetZeroToOneInterval(void) { return Interval(0.0f, 1.0f, 0.001f, true, true); }
	static Interval GetByteInterval(void) { return Interval(0.0f, 255.0f, 0.001f, true, true); }
	static Interval GetInvalidInterval(void) { return Interval(0.0f, 0.0f); }

	//Gets an interval that completely covers the given two intervals.
	static Interval Union(Interval one, Interval two) { return Interval(BasicMath::Min(one.GetStart(), two.GetStart()), BasicMath::Max(one.GetEnd(), two.GetEnd()), 0.001f, true, true); }
	//Gets an interval that represents the intersection of two intervals.
	static Interval Intersection(Interval one, Interval two);

	//Default constructor creates the range 0 : 1.
	Interval(void) : center(0.5f), range(0.5f), halfRange(0.25f) { }
	Interval(float _center, float _range) : center(_center), range(BasicMath::Abs(_range)), halfRange(_range * 0.5f) { }
	Interval(float start, float end, float epsilon, bool startInclusive = true, bool endInclusive = false);
	~Interval(void) { }

	float GetStart(void) const { return center - halfRange; }
	float GetEnd(void) const { return center + halfRange; }
	float GetCenter(void) const { return center; }
	float GetFirstQuarter(void) const { return center - GetQuarterRange(); }
	float GetThirdQuarter(void) const { return center + GetQuarterRange(); }

	float GetRange(void) const { return range; }
	float GetHalfRange(void) const { return halfRange; }
	float GetQuarterRange(void) const { return halfRange * 0.5f; }

	Interval GetFirstHalfInterval(void) const { return Interval(GetFirstQuarter(), halfRange); }
	Interval GetSecondHalfInterval(void) const { return Interval(center, halfRange); }
	Interval GetFirstQuarterInterval(void) const { return GetFirstHalfInterval().GetFirstHalfInterval(); }
	Interval GetSecondQuarterInterval(void) const { return GetFirstHalfInterval().GetSecondHalfInterval(); }
	Interval GetThirdQuarterInterval(void) const { return GetSecondHalfInterval().GetSecondHalfInterval(); }
	Interval GetFourthQuarterInterval(void) const { return GetSecondHalfInterval().GetSecondHalfInterval(); }

	float ClampValueToInterval(float value) const { if (range == 0) return center; return (value > GetEnd()) ? GetEnd() : (value < GetStart() ? GetStart() : value); }
	float WrapValueAroundInterval(float value) const { if (range == 0) return center; return fmodf(value - GetStart(), GetRange()) + GetStart(); }
	
	float GetLerpComponent(float value) const { if (range == 0) return 0.0f; return (value - GetStart()) / GetEnd(); }
	float RangeLerp(float zeroToOne) const { if (range == 0) return center; return BasicMath::Lerp(GetStart(), GetEnd(), zeroToOne); }
	float MapValue(Interval destination, float value) const
	{ 
		if (range == 0 || destination.range == 0) 
			return destination.center; 
		return BasicMath::Remap(GetStart(), GetEnd(), destination.GetStart(), destination.GetEnd(), value); 
	}

	//Reflects the given value around the center of this Interval.
	float Reflect(float val) const { return RangeLerp(1.0f - GetLerpComponent(val)); }

	bool Touches(float value) const { return (value >= GetStart() && value <= GetEnd()); }
	bool Touches(Interval other) const { return Touches(other.GetStart()) || Touches(other.GetEnd()) || other.Touches(GetStart()) || other.Touches(GetEnd()); }
	bool IsInside(Interval outer) const { return outer.Touches(GetStart()) && outer.Touches(GetEnd()); }

	Interval Inflate(float rangeScale) const { return Interval(center, range * rangeScale); }
	Interval Widen(float deltaRange) const { return Interval(center, range + deltaRange); }
	Interval Translate(float deltaCenter) const { return Interval(center + deltaCenter, range); }

	float RandomInsideRange(FastRand & fr) const { return RangeLerp(fr.GetZeroToOne()); }

	int GetHashCode(void) const { return Vector2f(center, range).GetHashCode(); }
	
	bool operator==(const Interval & other) const { return (center == other.GetCenter() && range == other.GetRange()); }
	bool operator!= (const Interval & other) const { return (center != other.GetCenter() || range != other.GetRange()); }

private:

	float center, range, halfRange;
};