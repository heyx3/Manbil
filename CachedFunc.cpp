#include "CachedFunc.h"

#include "BasicMath.h"

void CachedFunc::CalcCache(void)
{
	int steps = BasicMath::RoundToInt(answersRange.GetRange() / cacheInterval);
	cacheInterval = answersRange.GetRange() / steps;
	cachedAnswers = new float[steps];

	float counter = answersRange.GetStart();
	for (int i = 0; i < steps; ++i)
	{
		cachedAnswers[i] = func(counter);
		counter += cacheInterval;
	}
}