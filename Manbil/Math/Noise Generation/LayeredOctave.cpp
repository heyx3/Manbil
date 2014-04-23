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
	int i;
    Vector2i loc;
    for (i = 0; i < Octaves; ++i)
    {
        //Put the octave into the temp array.
        noises[i]->Generate(*tempNoiseArray);

        //Weight it and add it to the "out" array.
        for (loc.y = 0; loc.y < outNoiseArray.GetHeight(); ++loc.y)
        {
            for (loc.x = 0; loc.x < outNoiseArray.GetWidth(); ++loc.x)
            {
                float a = OctaveStrengths[i];
                a = (*tempNoiseArray)[loc];
                a = outNoiseArray[loc];
                outNoiseArray[loc] += (*tempNoiseArray)[loc] * OctaveStrengths[i];
            }
        }
    }

	//Clean up.
	delete tempNoiseArray;
}