#pragma once

#include "Interval.h"


//Caches the results of some float operation and interpolates to get values in between.
class CachedFunc
{
public:

	CachedFunc(Interval _answersRange, float _cacheInterval, float (*_func)(float)) : answersRange(_answersRange), cacheInterval(_cacheInterval), func(_func) { cachedAnswers = NULL; CalcCache(); }
	~CachedFunc(void) { if (cachedAnswers) delete cachedAnswers; }

	Interval GetAnswersRange(void) const { return answersRange; }
	float GetCacheInterval(void) const { return cacheInterval; }

	inline float GetFuncVal(float f) const
	{
        BasicMath::ClampIntervalResult rt = BasicMath::ClampToIntervalBounds(answersRange.GetStart(), cacheInterval, f);
		Interval dest(cachedAnswers[rt.zero], cachedAnswers[rt.one], 0.0001f, true, true),
				 src(cacheInterval * rt.zero, cacheInterval * rt.one, 0.0001f, true, true);
		return src.MapValue(dest, f);
	}

private:

	void CalcCache(void);

	Interval answersRange;
	float cacheInterval;
	float * cachedAnswers;
	float (*func)(float);
};