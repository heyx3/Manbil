#include "DiamondSquare.h"

#include <queue>
#include <math.h>

int DiamondSquare::NOISE_IS_BAD_SIZE_EXCEPTION = 1;

#define NaN (BasicMath::NaN)
#define IsNaN (BasicMath::IsNaN)

void DiamondSquare::IterateAlgorithm(int size, Vector2i topLeft, Interval * variances, Noise2D & noise) const
{
	Interval var = variances[0];
	int halfSize = size / 2;
	float f, f2;
	
	Vector2i tl = topLeft,
			 tm = tl + Vector2i(halfSize, 0),
			 tr = tl + Vector2i(size - 1, 0),
			 l = topLeft + Vector2i(0, halfSize),
			 m = l + Vector2i(halfSize, 0),
			 r = l + Vector2i(size - 1, 0),
			 bl = topLeft + Vector2i(0, size - 1),
			 bm = bl + Vector2i(halfSize, 0),
			 br = bl + Vector2i(size - 1, 0);


	//"Diamond" part of the algorithm.
	if (IsNaN(noise[m]))
	{
		f = noise[tl] + noise[tr] + noise[bl] + noise[br];
		f *= 0.25f;
		f2 = var.RandomInsideRange(fr);

		noise[m] = f + f2;
	}

	//"Square" part of the algorithm.

	//Left point.
	if (tl.x == 0 && IsNaN(noise[l]))
	{
		f = (noise[tl] + noise[bl]) * 0.5f;
		f2 = var.RandomInsideRange(fr);
		noise[l] = f + f2;
	}

	//Top point.
	if (tl.y == 0 && IsNaN(noise[tm]))
	{
		f = (noise[tl] + noise[tr]) * 0.5f;
		f2 = var.RandomInsideRange(fr);
		noise[tm] = f + f2;
	}

	//Right point.
	if (IsNaN(noise[r]))
	{
		f = (noise[tr] + noise[br]) * 0.5f;
		f2 = var.RandomInsideRange(fr);
		noise[r] = f + f2;
	}

	//Bottom point.
	if (IsNaN(noise[bm]))
	{
		f = (noise[bl] + noise[br]) * 0.5f;
		f2 = var.RandomInsideRange(fr);
		noise[bm] = f + f2;
	}


	//Recursive calls.
	if (size > 3)
	{
		//size += 1;
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
	int noiseSize = noise.GetWidth();

	#pragma region Error-checking

	//Make sure the noise arrayis a square.
	if (noise.GetWidth() != noise.GetHeight())
	{
		throw NOISE_IS_BAD_SIZE_EXCEPTION;
	}

	//Make sure the noise array is of size (2^n) + 1.
	int pow2 = 0;
	float value = BasicMath::Round(pow(2.0f, pow2), 3);
	for (pow2 = 0; ; ++pow2)
	{
		value = BasicMath::Round(pow(2.0f, pow2), 3);

		if (noiseSize == value + 1)
		{
			break;
		}
		if (noiseSize < value + 1)
		{
			throw NOISE_IS_BAD_SIZE_EXCEPTION;
		}
	}

	#pragma endregion

	#pragma region Create variances array

	//Create an array representing the forced random variances.
	std::vector<Interval> variances;
	int uses = 0;
	DiamondSquareStep temp;
	std::vector<Interval>::iterator it;
	for (int i = 0; i < NumbForcedVariances; ++i)
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
	int steps = BasicMath::RoundToInt(BasicMath::Log(noiseSize, 2.0f)) + 1;
	for (int i = steps - variances.size(); i > 0; --i)
	{
		it = variances.end();

		variances.insert(it, DefaultVariance);
	}

	#pragma endregion

	if (IsNaN(noise[Vector2i(0, 0)]))
	{
		noise[Vector2i(0, 0)] = StartingCornerValues;
	}
	if (IsNaN(noise[Vector2i(0, noiseSize - 1)]))
	{
		noise[Vector2i(0, noiseSize - 1)] = StartingCornerValues;
	}
	if (IsNaN(noise[Vector2i(noiseSize - 1, 0)]))
	{
		noise[Vector2i(noiseSize - 1, 0)] = StartingCornerValues;
	}
	if (IsNaN(noise[Vector2i(noiseSize - 1, noiseSize - 1)]))
	{
		noise[Vector2i(noiseSize - 1, noiseSize - 1)] = StartingCornerValues;
	}

	fr.Seed = Seed;

	IterateAlgorithm(noiseSize, Vector2i(), variances.data(), noise);
}