#pragma once

#include "BasicGenerators.h"

//Represents the random noise range for one or more diamond-square iterations.
struct DiamondSquareStep
{
	//The range of acceptable random values to add/subtract to the height value this iteration.
	Interval VarianceValueRange;
	//The number of iterations to use this random value range for.
	int Iterations;

	DiamondSquareStep(void) : VarianceValueRange(Interval(0.5f, 0.5f)), Iterations(1) { }
	DiamondSquareStep(Interval varianceValueRange, int iterations) : VarianceValueRange(varianceValueRange), Iterations(iterations) { }
	~DiamondSquareStep(void) { }
};

//Generates random 2D noise using the Diamond-Square algorithm.
//Any noise values not set to NaN will be untouched by the algorithm, allowing the user to seed values.
class DiamondSquare : Generator
{
public:

	static int NOISE_IS_BAD_SIZE_EXCEPTION;


	int Seed;
	void GenerateNewSeed(void) { Seed = fr.GetRandInt(); }

	Interval DefaultVariance;
	DiamondSquareStep * ForcedVariances;
	int NumbForcedVariances;
	float StartingCornerValues;


	DiamondSquare(int seed, Interval defaultVariance, DiamondSquareStep * forcedVariances, int nForcedVariances, float startingCornerValues)
		: Seed(seed), DefaultVariance(defaultVariance), ForcedVariances(forcedVariances), NumbForcedVariances(nForcedVariances), StartingCornerValues(startingCornerValues) { }


	//If the given noise array is not M x M, where M is (2^n) + 1,
	//    an exception of type NOISE_IS_BAD_SIZE_EXCEPTION will be thrown.
	//The noise is assumed to be pre-filled with NaN values; if some value other than NaN is in
	//   the noise, it will be left alone -- NOT filled in with a generated value.
	//This can be used to "seed" the noise with values for the first few iterations.
	virtual void Generate(Noise2D & noise) const override;
	

private:

	mutable FastRand fr;

	void DiamondSquare::IterateAlgorithm(int size, Vector2i topLeft, Interval * variances, Fake2DArray<float> & noise) const;
};