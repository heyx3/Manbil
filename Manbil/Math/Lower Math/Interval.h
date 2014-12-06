#pragma once

#include <string>
#include "FastRand.h"
#include "Vectors.h"


//Represents a continuous range of float values. Is immutable.
//An interval with a range of 0 or less is considered invalid.
class Interval
{
public:

    //Gets the interval range [0, 1].
	static Interval GetZeroToOne(void) { return Interval(0.0f, 1.0f, 0.001f, true, true); }
    //Gets the interval range [0, 255].
	static Interval GetByteInterval(void) { return Interval(0.0f, 255.0f, 0.001f, true, true); }
    //Gets an interval with an "invalid" value.
	static Interval GetInvalidInterval(void) { return Interval(0.0f, 0.0f); }

	//Gets an interval that completely covers the given two intervals.
	static Interval Union(Interval one, Interval two);
	//Gets an interval that represents the intersection of two intervals.
    //Returns an invalid interval if the two intervals don't intersect.
	static Interval Intersection(Interval one, Interval two);


	//Default constructor creates the range 0 : 1.
	Interval(void) : center(0.5f), range(1.0f), halfRange(0.5f) { }
    //Creates an interval with the given center and range.
	Interval(float _center, float _range)
        : center(_center), range(_range), halfRange(_range * 0.5f) { }
    //Creates an interval with the given start/end and margin of error.
    Interval(float start, float end, float epsilon, bool startInclusive = true, bool endInclusive = false);


    //Gets whether this interval is exactly equal to the given one.
    bool operator==(const Interval& other) const;
    //Gets whether this interval is NOT exactly equal to the given one.
    bool operator!= (const Interval& other) const;


    //Gets whether this interval is vaid (i.e. its range is greater than 0.0f).
    bool IsValid(void) const { return range > 0.0f; }


    //Gets the beginning (inclusive) of this interval.
	float GetStart(void) const { return center - halfRange; }
    //Gets the end (inclusive) of this interval.
	float GetEnd(void) const { return center + halfRange; }
    //Gets the center of this interval.
	float GetCenter(void) const { return center; }

    //Gets the span of this interval.
	float GetRange(void) const { return range; }
    //Gets half the span of this interval.
	float GetHalfRange(void) const { return halfRange; }
    //Gets a quarter of the span of this interval.
	float GetQuarterRange(void) const { return range * 0.25f; }


    //Gets an interval covering the first half of this interval.
    Interval GetFirstHalf(void) const { return Interval(center - GetQuarterRange(), halfRange); }
    //Gets an interval covering the second half of this interval.
    Interval GetSecondHalf(void) const { return Interval(center + GetQuarterRange(), halfRange); }
    //Gets an interval covering the first quarter of this interval.
    Interval GetFirstQuarter(void) const { return GetFirstHalf().GetFirstHalf(); }
    //Gets an interval covering the second quarter of this interval.
    Interval GetSecondQuarter(void) const { return GetFirstHalf().GetSecondHalf(); }
    //Gets an interval covering the third quarter of this interval.
    Interval GetThirdQuarter(void) const { return GetSecondHalf().GetFirstHalf(); }
    //Gets an interval covering the fourth quarter of this interval.
	Interval GetFourthQuarter(void) const { return GetSecondHalf().GetSecondHalf(); }


    //Scales this interval's range by the given amount.
    Interval Inflate(float rangeScale) const { return Interval(center, range * rangeScale); }
    //Increases this interval's range by the given amount.
    Interval Widen(float deltaRange) const { return Interval(center, range + deltaRange); }
    //Moves this interval's center by the given amount.
    Interval Translate(float deltaCenter) const { return Interval(center + deltaCenter, range); }


    //Clamps the given value to stay inside this interval.
    float Clamp(float value) const { return BasicMath::Clamp(value, GetStart(), GetEnd()); }
    //Wraps the given value around the edges of this interval.
	float WrapValueAroundInterval(float value) const;
	

    //Gets the value "t" such that lerp(intervalStart, intervalEnd, "t") == "value".
	float GetLerpComponent(float value) const;
    //Interpolates from the start of this interval to the end.
	float Lerp(float t) const;
    //Remaps the given value from this interval to the given interval.
	float MapValue(Interval destination, float value) const;


	//Reflects the given value around the center of this Interval.
	float Reflect(float val) const { return Lerp(1.0f - GetLerpComponent(val)); }


    //Gets whether the given value is inside this interal.
    bool IsInside(float value) const { return (value >= GetStart() && value <= GetEnd()); }
    //Gets whether this interval is inside the given interval.
    bool IsInside(Interval outer) const;
    //Gets whether the given interval intersects this interval at all.
    bool Touches(Interval other) const;


    //Gets a random value inside this interval's range.
	float RandomInsideRange(FastRand& fr) const { return Lerp(fr.GetZeroToOne()); }


    //Gets a hash code for this interval.
	int GetHashCode(void) const { return Vector2f(center, range).GetHashCode(); }


private:

	float center, range, halfRange;
};