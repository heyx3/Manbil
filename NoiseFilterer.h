#pragma once

#include "Vectors.h"
#include "Fake2DArray.h"
#include "BasicMath.h"
#include "Interval.h"
#include "BasicGenerators.h"

#include "NoiseFilterRegion.h"

#include <iostream>

//Filters floating-point noise using different algorithms.
class NoiseFilterer : public Generator
{
public:

	NoiseFilterer(void) { InitData(); }
	void InitData(void)
	{
		RemapValues_OldVals = Interval::GetZeroToOneInterval();
		InvertFunc = false;
		Flatten_FlatValue = 0.0f;
		Noise_Amount = 1.0f;
		Noise_Seed = 12345;
		Increase_Amount = 1.0f;
		UpContrast_Power = UpContrastPowers::CUBIC;

		FillRegion = 0;
		NoiseToFilter = 0;
		FilterFunc = 0;
	}


	NoiseFilterRegion * FillRegion;
	Generator * NoiseToFilter;
	bool InvertFunc;
	typedef void (NoiseFilterer::*MemberFunc)(void) const;
	MemberFunc FilterFunc;


	Interval RemapValues_OldVals;
	//Remaps the noise from the given original range to the range 0.0-1.0.
	void RemapValues(void) const;


	//Reflects the noise's values around the center of the noise range. The strength will always be 1.0 regardless of what is passed in.
	void ReflectValues(void) const;


	//The higher the power, the more expensive the function.
	enum UpContrastPowers
	{
		CUBIC,
		QUINTIC,
	};
	UpContrastPowers UpContrast_Power;
	//Increases the contrast in the noise.
	void UpContrast(void) const;


	//Gets the average of the area and pushes all noise values to that average.
	void Average(void) const;


	float Flatten_FlatValue;
	//Flattens the noise.
	void Flatten(void) const;


	//Smooths the area.
	void Smooth(void) const;


	float Noise_Amount;
	int Noise_Seed;
	//Adds random noise to the area.
	void Noise(void) const;


	float Increase_Amount;
	//Increases the area by the given amount.
	void Increase(void) const;


	virtual void Generate(Noise2D & noise) const override;


	//Iterates through every spot in the noise and sets it to a value. Note:
	//   1) The given function should return the new noise value at full strength (1.0f) and not inverted;
	//   2) The new value at the given noise point will be set to
	//      lerp([original value], [function value], [function strength]).
	void SetAtEveryPoint(void * pData, float (*GetValue)(void * pData, Vector2i loc, Noise2D * noise)) const;


private:
	
	mutable Noise2D * noise;
};