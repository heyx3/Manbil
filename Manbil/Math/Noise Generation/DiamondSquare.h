#pragma once

#include "BasicGenerators.h"


//Represents the random noise range for one or more diamond-square iterations.
struct DiamondSquareStep
{
	//The range of acceptable random values to add/subtract to the height value this iteration.
	Interval VarianceValueRange;
	//The number of iterations to use this random value range for.
	unsigned int Iterations;

	DiamondSquareStep(Interval varianceValueRange, unsigned int iterations = 1)
        : VarianceValueRange(varianceValueRange), Iterations(iterations) { }
};


//Generates random 2D noise using the Diamond-Square algorithm.
//The noise array should be pre-filled with NaN; any values that aren't NaN will be left alone.
//This allows the user to seed values to effect the this algorithm.
//The noise array that uses this generator must be a square whose sides are one more than a power of two.
class DiamondSquare : Generator2D
{
public:

	int Seed;

	DiamondSquareStep* Variances;
	unsigned int NumbVariances;
    
	Interval DefaultVariance;

	float StartingCornerValues;


	DiamondSquare(int seed, DiamondSquareStep* forcedVariances, unsigned int nForcedVariances,
                  Interval defaultVariance, float startingCornerValues)
		: Seed(seed), DefaultVariance(defaultVariance), Variances(forcedVariances),
          NumbVariances(nForcedVariances), StartingCornerValues(startingCornerValues) { }


	virtual void Generate(Noise2D& noise) const override;
	

private:

	void DiamondSquare::IterateAlgorithm(unsigned int size, Vector2u topLeft, Interval* variances, Array2D<float>& noise) const;
};