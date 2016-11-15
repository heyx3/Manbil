#include "DiamondSquare.h"

#include <vector>
#include <assert.h>


void DiamondSquare::IterateAlgorithm(unsigned int size, Vector2u topLeft,
                                     Interval* variances, Noise2D& noise) const
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
	if (Mathf::IsNaN(noise[m]))
	{
		f = noise[tl] + noise[tr] + noise[bl] + noise[br];
		f *= 0.25f;
		f2 = var.RandomInsideRange(FastRand(Vector3i((int)m.x, (int)m.y, Seed).GetHashCode()));

		noise[m] = f + f2;
	}

	//"Square" part of the algorithm.

	//Left point.
	if (Mathf::IsNaN(noise[l]))
	{
		f = (noise[tl] + noise[bl]) * 0.5f;
        f2 = var.RandomInsideRange(FastRand(Vector3i((int)l.x, (int)l.y, Seed).GetHashCode()));
		noise[l] = f + f2;
	}

	//Top point.
	if (Mathf::IsNaN(noise[tm]))
	{
		f = (noise[tl] + noise[tr]) * 0.5f;
        f2 = var.RandomInsideRange(FastRand(Vector3i((int)tm.x, (int)tm.y, Seed).GetHashCode()));
		noise[tm] = f + f2;
	}

	//Right point.
	if (Mathf::IsNaN(noise[r]))
	{
		f = (noise[tr] + noise[br]) * 0.5f;
        f2 = var.RandomInsideRange(FastRand(Vector3i((int)r.x, (int)r.y, Seed).GetHashCode()));
		noise[r] = f + f2;
	}

	//Bottom point.
	if (Mathf::IsNaN(noise[bm]))
	{
		f = (noise[bl] + noise[br]) * 0.5f;
        f2 = var.RandomInsideRange(FastRand(Vector3i((int)bm.x, (int)bm.y, Seed).GetHashCode()));
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
void DiamondSquare::Generate(Noise2D& noise) const
{
    assert(noise.GetWidth() == noise.GetHeight());

	unsigned int noiseSize = noise.GetWidth();


	//Create the step sizes for each level.
	std::vector<Interval> variances;
	unsigned int uses = 0;
	DiamondSquareStep* temp = 0;
	for (unsigned int i = 0; i < NumbVariances; ++i)
	{
		temp = &Variances[i];
		uses = 0;

		while (uses < temp->Iterations)
		{
			variances.insert(variances.end(), temp->VarianceValueRange);
			uses += 1;
		}
	}
	//If there aren't enough hard-coded variances, add the default variance.
	unsigned int steps = (unsigned int)Mathf::RoundToInt(Mathf::Log(noiseSize, 2.0f)) + 1;
	for (unsigned int i = steps - variances.size(); i > 0; --i)
		variances.insert(variances.end(), DefaultVariance);


    //Seed the corners of the noise.
	if (Mathf::IsNaN(noise[Vector2u(0, 0)]))
		noise[Vector2u(0, 0)] = StartingCornerValues;
	if (Mathf::IsNaN(noise[Vector2u(0, noiseSize - 1)]))
		noise[Vector2u(0, noiseSize - 1)] = StartingCornerValues;
	if (Mathf::IsNaN(noise[Vector2u(noiseSize - 1, 0)]))
		noise[Vector2u(noiseSize - 1, 0)] = StartingCornerValues;
	if (Mathf::IsNaN(noise[Vector2u(noiseSize - 1, noiseSize - 1)]))
		noise[Vector2u(noiseSize - 1, noiseSize - 1)] = StartingCornerValues;

    //Start the iteration algorithm.
	IterateAlgorithm(noiseSize, Vector2u(), variances.data(), noise);
}