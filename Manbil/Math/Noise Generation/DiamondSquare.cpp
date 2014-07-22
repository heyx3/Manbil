#include "DiamondSquare.h"

#include <queue>
#include <math.h>
#include <assert.h>


int DiamondSquare::NOISE_IS_BAD_SIZE_EXCEPTION = 1;


void DiamondSquare::IterateAlgorithm(unsigned int size, Vector2u topLeft, Interval * variances, Noise2D & noise) const
{
    assert(size > 0);

	Interval var = variances[0];
	unsigned int halfSize = size / 2;
	float f, f2;
	
	Vector2u tl = topLeft,
			 tm = tl + Vector2u(halfSize, 0),
			 tr = tl + Vector2u(size - 1, 0),
			 l = topLeft + Vector2u(0, halfSize),
			 m = l + Vector2u(halfSize, 0),
			 r = l + Vector2u(size - 1, 0),
			 bl = topLeft + Vector2u(0, size - 1),
			 bm = bl + Vector2u(halfSize, 0),
			 br = bl + Vector2u(size - 1, 0);


	//"Diamond" part of the algorithm.
	if (BasicMath::IsNaN(noise[m]))
	{
		f = noise[tl] + noise[tr] + noise[bl] + noise[br];
		f *= 0.25f;
		f2 = var.RandomInsideRange(fr);

		noise[m] = f + f2;
	}

	//"Square" part of the algorithm.

	//Left point.
	if (tl.x == 0 && BasicMath::IsNaN(noise[l]))
	{
		f = (noise[tl] + noise[bl]) * 0.5f;
		f2 = var.RandomInsideRange(fr);
		noise[l] = f + f2;
	}

	//Top point.
	if (tl.y == 0 && BasicMath::IsNaN(noise[tm]))
	{
		f = (noise[tl] + noise[tr]) * 0.5f;
		f2 = var.RandomInsideRange(fr);
		noise[tm] = f + f2;
	}

	//Right point.
	if (BasicMath::IsNaN(noise[r]))
	{
		f = (noise[tr] + noise[br]) * 0.5f;
		f2 = var.RandomInsideRange(fr);
		noise[r] = f + f2;
	}

	//Bottom point.
	if (BasicMath::IsNaN(noise[bm]))
	{
		f = (noise[bl] + noise[br]) * 0.5f;
		f2 = var.RandomInsideRange(fr);
		noise[bm] = f + f2;
	}


	//Recursive calls.
	if (size > 3)
	{
		size = ((size - 1) / 2) + 1;

		variances = &variances[1];

		IterateAlgorithm(size, tl, variances, noise);
		IterateAlgorithm(size, tm, variances, noise);
		IterateAlgorithm(size, l, variances, noise);
		IterateAlgorithm(size, m, variances, noise);
	}
}
void DiamondSquare::Generate(Noise2D & noise) const
{
	unsigned int noiseSize = noise.GetWidth();

	#pragma region Error-checking

	//Make sure the noise array is a square.
	if (noise.GetWidth() != noise.GetHeight())
	{
		throw NOISE_IS_BAD_SIZE_EXCEPTION;
	}

	//Make sure the noise array is of size (2^n) + 1.
	unsigned int pow2 = 0;
    unsigned int powValue;
	for (pow2 = 0; ; ++pow2)
	{
		powValue = BasicMath::IntPow(2, pow2);

		if (noiseSize == (powValue + 1))
		{
			break;
		}
		if (noiseSize < (powValue + 1))
		{
			throw NOISE_IS_BAD_SIZE_EXCEPTION;
		}
	}

	#pragma endregion

	#pragma region Create variances array

	//Create an array representing the forced random variances.
	std::vector<Interval> variances;
	unsigned int uses = 0;
	DiamondSquareStep temp;
	std::vector<Interval>::iterator it;
	for (unsigned int i = 0; i < NumbForcedVariances; ++i)
	{
		temp = ForcedVariances[i];
		uses = 0;

		while (uses < temp.Iterations)
		{
			it = variances.end();

			variances.insert(it, temp.VarianceValueRange);

			uses += 1;
		}
	}

	//If there aren't enough variances, add in the default.
	unsigned int steps = (unsigned int)BasicMath::RoundToInt(BasicMath::Log(noiseSize, 2.0f)) + 1;
	for (unsigned int i = steps - variances.size(); i > 0; --i)
	{
		it = variances.end();

		variances.insert(it, DefaultVariance);
	}

	#pragma endregion

	if (BasicMath::IsNaN(noise[Vector2u(0, 0)]))
	{
		noise[Vector2u(0, 0)] = StartingCornerValues;
	}
	if (BasicMath::IsNaN(noise[Vector2u(0, noiseSize - 1)]))
	{
		noise[Vector2u(0, noiseSize - 1)] = StartingCornerValues;
	}
	if (BasicMath::IsNaN(noise[Vector2u(noiseSize - 1, 0)]))
	{
		noise[Vector2u(noiseSize - 1, 0)] = StartingCornerValues;
	}
	if (BasicMath::IsNaN(noise[Vector2u(noiseSize - 1, noiseSize - 1)]))
	{
		noise[Vector2u(noiseSize - 1, noiseSize - 1)] = StartingCornerValues;
	}

	fr.Seed = Seed;

	IterateAlgorithm(noiseSize, Vector2u(), variances.data(), noise);
}