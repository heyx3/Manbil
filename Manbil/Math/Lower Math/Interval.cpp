#include "Interval.h"



Interval Union(Interval one, Interval two)
{
    return Interval(Mathf::Min(one.GetStart(), two.GetStart()),
                    Mathf::Max(one.GetEnd(), two.GetEnd()),
                    0.001f, true, true);
}
Interval Interval::Intersection(Interval one, Interval two)
{
	if (one.IsInside(two))
	{
		return one;
	}
	if (two.IsInside(one))
	{
		return two;
	}

	Interval lower = (one.GetStart() < two.GetStart()) ? one : two,
			 higher = (one.GetEnd() > two.GetEnd()) ? one : two;

	if (lower.GetEnd() > higher.GetStart())
	{
		return Interval(higher.GetStart(), lower.GetEnd(), 0.001f, true, true);
	}
	else
	{
		return GetInvalidInterval();
	}
}

Interval::Interval(float start, float end, float epsilon, bool startInclusive, bool endInclusive)
{
	float st = (startInclusive ? start : start + epsilon);
	float en = (endInclusive ? end : end - epsilon);

	center = (st + en) * 0.5f;
	range = Mathf::Abs(en - st);
	halfRange = range * 0.5f;
}

float Interval::WrapValueAroundInterval(float value) const
{
    if (range == 0) return center;

    float nominator = value - GetStart();

    if (nominator < 0.0f)
    {
        return Reflect(fmodf(-nominator, GetRange()) + GetStart());
    }
    else
    {
        return fmodf(nominator, GetRange()) + GetStart();
    }
}

float Interval::GetLerpComponent(float value) const
{
    if (range == 0.0f)
        return 0.5f;

    return Mathf::LerpComponent(GetStart(), GetEnd(), value);
}
float Interval::Lerp(float t) const
{
    if (range == 0.0f)
        return center;

    return Mathf::Lerp(GetStart(), GetEnd(), t);
}
float Interval::MapValue(Interval destination, float value) const
{
    if (range == 0.0f || destination.range == 0.0f)
        return destination.center;

    return Mathf::Remap(GetStart(), GetEnd(),
                        destination.GetStart(), destination.GetEnd(),
                        value);
}

bool Interval::IsInside(Interval outer) const
{
    return outer.IsInside(GetStart()) && outer.IsInside(GetEnd());
}
bool Interval::Touches(Interval other) const
{
    return IsInside(other.GetStart()) || IsInside(other.GetEnd()) ||
           other.IsInside(GetStart()) || other.IsInside(GetEnd());
}

bool Interval::operator==(const Interval & other) const
{
    return (center == other.GetCenter() && range == other.GetRange());
}
bool Interval::operator!=(const Interval & other) const
{
    return (center != other.GetCenter() || range != other.GetRange());
}