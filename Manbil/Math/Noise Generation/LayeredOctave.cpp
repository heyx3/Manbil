#include "LayeredOctave.h"

LayeredOctave2D::LayeredOctave2D(unsigned int numbOctaves, const float octaveWeights[], const Generator2D *const*const octaves)
{
	Octaves = numbOctaves;

	OctaveStrengths = new float[Octaves];
    for (unsigned int i = 0; i < Octaves; ++i)
		OctaveStrengths[i] = octaveWeights[i];

	noises = new Generator2D*[Octaves];
    for (unsigned int i = 0; i < Octaves; ++i)
		noises[i] = const_cast<Generator2D*>(octaves[i]);
}

LayeredOctave2D::~LayeredOctave2D(void)
{
	delete[] noises;
	delete[] OctaveStrengths;
}

void LayeredOctave2D::Generate(Array2D<float> & outNoiseArray) const
{
	//Put this temp array on the heap; it won't fit on the stack.
	Array2D<float> * tempNoiseArray = new Array2D<float>(outNoiseArray.GetWidth(),
											             outNoiseArray.GetHeight(),
														 0.0f);
	outNoiseArray.Fill(0.0f);

	//Add successive octave noise into the "out" array.
	unsigned int i;
    Vector2u loc;
    for (i = 0; i < Octaves; ++i)
    {
        //Put the octave into the temp array.
        noises[i]->Generate(*tempNoiseArray);

        //Weight it and add it to the "out" array.
        for (loc.y = 0; loc.y < outNoiseArray.GetHeight(); ++loc.y)
            for (loc.x = 0; loc.x < outNoiseArray.GetWidth(); ++loc.x)
                outNoiseArray[loc] += (*tempNoiseArray)[loc] * OctaveStrengths[i];
    }

	//Clean up.
	delete tempNoiseArray;
}



LayeredOctave3D::LayeredOctave3D(unsigned int numbOctaves, const float octaveWeights[], const Generator3D *const*const octaves)
{
    Octaves = numbOctaves;

    OctaveStrengths = new float[Octaves];
    for (unsigned int i = 0; i < Octaves; ++i)
        OctaveStrengths[i] = octaveWeights[i];

    noises = new Generator3D*[Octaves];
    for (unsigned int i = 0; i < Octaves; ++i)
        noises[i] = const_cast<Generator3D*>(octaves[i]);
}

LayeredOctave3D::~LayeredOctave3D(void)
{
    delete[] noises;
    delete[] OctaveStrengths;
}

void LayeredOctave3D::Generate(Array3D<float> & outNoiseArray) const
{
    Array3D<float> tempNoiseArray(outNoiseArray.GetWidth(),
                                  outNoiseArray.GetHeight(),
                                  outNoiseArray.GetDepth(),
                                  0.0f);

    //Add successive octave noise into the "out" array.
    outNoiseArray.Fill(0.0f);
    unsigned int i;
    Vector3u loc;
    for (i = 0; i < Octaves; ++i)
    {
        //Put the octave into the temp array.
        noises[i]->Generate(tempNoiseArray);

        //Weight it and add it to the "out" array.
        for (loc.z = 0; loc.z < outNoiseArray.GetDepth(); ++loc.z)
            for (loc.y = 0; loc.y < outNoiseArray.GetHeight(); ++loc.y)
                for (loc.x = 0; loc.x < outNoiseArray.GetWidth(); ++loc.x)
                    outNoiseArray[loc] += (tempNoiseArray)[loc] * OctaveStrengths[i];
    }
}