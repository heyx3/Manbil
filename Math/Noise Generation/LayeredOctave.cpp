#include "LayeredOctave.h"

LayeredOctave::LayeredOctave(int numbOctaves, const float octaveWeights[], const Generator *const*const octaves)
{
	Octaves = numbOctaves;

	OctaveStrengths = new float[Octaves];
	for (int i = 0; i < Octaves; ++i)
	{
		OctaveStrengths[i] = octaveWeights[i];
	}

	noises = new Generator*[Octaves];
	for (int i = 0; i < Octaves; ++i)
	{
		noises[i] = const_cast<Generator*>(octaves[i]);
	}
}

LayeredOctave::~LayeredOctave(void)
{
	delete [] noises;

	delete[] OctaveStrengths;
}

void LayeredOctave::Generate(Fake2DArray<float> & outNoiseArray) const
{
	//Put this temp array on the heap; it won't fit on the stack.
	Fake2DArray<float> * tempNoiseArray = new Fake2DArray<float>(outNoiseArray.GetWidth(),
															 	 outNoiseArray.GetHeight(),
																 0.0f);

	outNoiseArray.Fill(float(0.0f));

	//Add successive octave noise into the "out" array.
	int i, x, y;
	for (i = 0; i < Octaves; ++i)
	{
		//Put the octave into the temp array.
		noises[i]->Generate(*tempNoiseArray);

		//Weight it and add it to the "out" array.
		for (x = 0; x < outNoiseArray.GetWidth(); ++x)
		{
			for (y = 0; y < outNoiseArray.GetHeight(); ++y)
			{
				float a = OctaveStrengths[i];
				a = (*tempNoiseArray)[Vector2i(x, y)];
				a = outNoiseArray[Vector2i(x, y)];
				outNoiseArray[Vector2i(x, y)] += (*tempNoiseArray)[Vector2i(x, y)] *
												   OctaveStrengths[i];
			}
		}
	}

	//Clean up.
	delete tempNoiseArray;
}