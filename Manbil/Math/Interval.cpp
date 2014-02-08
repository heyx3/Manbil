#include "Interval.h"

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
	range = BasicMath::Abs(en - st);
	halfRange = range * 0.5f;
}