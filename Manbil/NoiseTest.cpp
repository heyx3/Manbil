#include "NoiseTest.h"

#include <assert.h>
#include <iostream>

#include "Math/LowerMath.hpp"
#include "Math/NoiseGeneration.hpp"
#include "Math/Noise Generation/ColorGradient.h"
#include "Math/Higher Math/BumpmapToNormalmap.h"
#include "Rendering/Texture Management/TextureConverters.h"


const int noiseSize = 1024,
	pixelArrayWidth = noiseSize * 4,
	pixelArrayHeight = noiseSize;
#define GET_NOISE2D (Noise2D(noiseSize, noiseSize))

sf::Font guiFont;
void NoiseTest::InitializeWorld(void)
{
	guiFont = sf::Font();
	guiFont.loadFromFile("Content/Fonts/Candarab.ttf");

	GetWindow()->setSize(sf::Vector2u(noiseSize, noiseSize));
    GetWindow()->setView(sf::View(sf::FloatRect(0.0f, 0.0f, noiseSize, noiseSize)));
    GetWindow()->setTitle("Noise test");


    std::cout << "Press 'left' and 'right' to change the bump height.\n" <<
                 "Press 'up' to convert the current texture from a bump map to a normal map.\n" <<
                 "Press 'Enter' to regenerate the noise.\n" <<
                 "Press 'Space' to save the noise as 'RenderedNoise.png'.\n" <<
                 "Press 'Escape' to quit.\n\n";


	ReGenerateNoise(false);
}

void NoiseToPixels(const Noise2D & noise, Array2D<sf::Uint8> & outPixels)
{
	ColorGradient colGrad;

	//Set the color gradient.

	if (false)
	{
		#pragma region Dirt

		Vector4f dirtColB(0.10f, 0.005f, 0, 1),
				 dirtColE(0.35f, 0.1f, 0, 1);
		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.begin(), ColorNode(0.0f, dirtColB));
		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.end(), ColorNode(0.1f, dirtColE, Vector4f(0, 0.25f, 0, 1)));
		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.end(), ColorNode(1.0f, Vector4f(0, 0.75f, 0, 1)));

		#pragma endregion
	}
	else if (false)
	{
		#pragma region Grass

		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.end(), ColorNode(0.0f, Vector4f(0.36f, 0.18f, 0, 1)));
		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.end(), ColorNode(0.3f, Vector4f(0.2f, 0.14f, 0.0f, 1.0f), Vector4f(0.05f, 0.25f, 0.05f, 1)));
		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.end(), ColorNode(1.0f, Vector4f(0, 0.8f, 0, 1)));

		#pragma endregion
	}
	else if (false)
	{
		#pragma region Sky

		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.end(), ColorNode(0.0f, Vector4f(1, 1, 1, 1)));
		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.end(), ColorNode(1.0f, Vector4f(0, 0, 1, 1)));

		#pragma endregion
	}
	else if (false)
	{
		#pragma region Stone

		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.begin(), ColorNode(0.0f, Vector4f(0.1f, 0.1f, 0.15f, 1)));
		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.end(), ColorNode(1.0f, Vector4f(0.05f, 0.05f, 0.1f, 1)));

		#pragma endregion
	}
	else if (true)
	{
		#pragma region Black and white

		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.end(), ColorNode(0.0f, Vector4f(0, 0, 0, 1)));
		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.end(), ColorNode(1.0f, Vector4f(1, 1, 1, 1)));

		#pragma endregion
	}
	else assert(false);





	int x, y, outX, outY;
	float readNoise;
	sf::Uint8 noiseVal;
	float tempF;

	//Go through every pixel.
	for (x = 0; x < noise.GetWidth(); ++x)
	{
		for (y = 0; y < noise.GetHeight(); ++y)
		{
			tempF = noise[Vector2i(x, y)];
			readNoise = BasicMath::Clamp(tempF, 0.0f, 1.0f);

			//Convert to a byte value.
			noiseVal = (sf::Uint8)BasicMath::RoundToInt(readNoise * 255.0f);

			//Get the coordinates of the pixel in the return value array.
			outX = x * 4;
			outY = y;

			//Set the pixel values.
			Vector4f col = colGrad.GetColor(readNoise) * 255.0f;
			col = Vector4f(BasicMath::Clamp(col.x, 0.0f, 255.0f), BasicMath::Clamp(col.y, 0.0f, 255.0f),
						   BasicMath::Clamp(col.z, 0.0f, 255.0f), BasicMath::Clamp(col.w, 0.0f, 255.0f));
			Vector4b colB((unsigned char)BasicMath::RoundToInt(col.x),
						  (unsigned char)BasicMath::RoundToInt(col.y),
						  (unsigned char)BasicMath::RoundToInt(col.z),
						  (unsigned char)BasicMath::RoundToInt(col.w));

			outPixels[Vector2i(outX, outY)] = colB.x;
			outPixels[Vector2i(outX + 1, outY)] = colB.y;
			outPixels[Vector2i(outX + 2, outY)] = colB.z;
			outPixels[Vector2i(outX + 3, outY)] = colB.w;
		}
	}
}

namespace RandStuff { FastRand fr(255); }
using namespace RandStuff;

void NoiseTest::ReGenerateNoise(bool newSeeds)
{
	//Draw a "loading..." text over the normal noise.
	GetWindow()->clear();

	if (renderedNoise)
	{
		GetWindow()->draw(*renderedNoise);
	}

	sf::Text t(sf::String("Generating..."), guiFont);
	t.setColor(sf::Color(0, 0, 0, 255));
	t.setPosition(3.0f, 22.0f);
	GetWindow()->draw(t);
    t.setColor(sf::Color(255, 255, 255, 255));
    t.setPosition(0.0f, 19.0f);
	GetWindow()->draw(t);

	GetWindow()->display();


	DeleteData();


	if (newSeeds) fr.Seed = rand();


	//Do noise generation.
	
	Noise2D finalNoise(noiseSize, noiseSize);

	NoiseFilterer2D nf;
	MaxFilterRegion mfr;
	nf.FillRegion = &mfr;


	if (false)
	{
		#pragma region Layered Perlin

        const int pScale = 4;
		Perlin2D per1(128.0f, Perlin2D::Smoothness::Cubic, Vector2i(), fr.Seed, true, Vector2i(1, 1) * pScale),
                 per2(64.0f, Perlin2D::Smoothness::Cubic, Vector2i(), fr.Seed + 634356, true, Vector2i(2, 2) * pScale),
                 per3(32.0f, Perlin2D::Smoothness::Cubic, Vector2i(), fr.Seed + 6193498, true, Vector2i(4, 4) * pScale),
                 per4(16.0f, Perlin2D::Smoothness::Cubic, Vector2i(), fr.Seed + 1009346, true, Vector2i(8, 8) * pScale),
                 per5(8.0f, Perlin2D::Smoothness::Quintic, Vector2i(), fr.Seed + 619398, true, Vector2i(16, 16) * pScale),
                 per6(4.0f, Perlin2D::Smoothness::Quintic, Vector2i(), fr.Seed + 45324, true, Vector2i(32, 32) * pScale),
                 per7(2.0f, Perlin2D::Smoothness::Quintic, Vector2i(), fr.Seed + 21234, true, Vector2i(64, 64) * pScale);
        Generator2D * gens[] = { &per1, &per2, &per3, &per4, &per5, &per6, &per7 };
        float weights[7];
        float counter = 0.5f;
        for (int i = 0; i < 7; ++i)
        {
            weights[i] = counter;
            counter *= 0.5f;
        }

        LayeredOctave2D layers(7, weights, gens);

        
		nf.FilterFunc = &NoiseFilterer2D::UpContrast;
        nf.UpContrast_Power = NoiseFilterer2D::UpContrastPowers::QUINTIC;
        nf.NoiseToFilter = &layers;
		nf.Generate(finalNoise);

        //per4.Generate(finalNoise);


		#pragma endregion
	}
	else if (false)
	{
		#pragma region Worley

		fr.Seed = fr.GetRandInt();
		Worley2D wor(fr.GetRandInt(), 64, 5, 10);
		wor.DistFunc = &Worley2D::StraightLineDistance;
		wor.ValueGenerator = [](Worley2D::DistanceValues v) { return v.Values[2] - v.Values[0]; };
		wor.Generate(finalNoise);

        nf.Increase_Amount = -0.25f;
        //nf.Increase(&finalNoise);

        nf.UpContrast_Passes = 1;
        nf.UpContrast_Power = NoiseFilterer2D::UpContrastPowers::CUBIC;
        nf.UpContrast(&finalNoise);

        //nf.ReflectValues(&finalNoise);

		#pragma endregion
	}
    else if (false)
    {
        #pragma region TwoD Perlin

        const int pSize = 32;
        Perlin2D perl((float)pSize, Perlin2D::Quintic, Vector2i(), fr.Seed, true, Vector2i(noiseSize / pSize, noiseSize / pSize));
        perl.Generate(finalNoise);

        NoiseAnalysis2D::MinMax mm = NoiseAnalysis2D::GetMinAndMax(finalNoise);
        nf.RemapValues_OldVals = Interval(mm.Min, mm.Max, 0.00001f, true, false);
        nf.RemapValues(&finalNoise);

        #pragma endregion
    }
    else if (false)
    {
        #pragma region ThreeD Perlin varying over time

        float currentTime = GetTotalElapsedSeconds();
        Vector3f offset(currentTime, currentTime, currentTime);
        offset *= 4.0f;

        Perlin3D perl3(Vector3f(64.0f, 32.0f, 5.0f), Perlin3D::Cubic, offset.CastToInt(), 123456);
        const int depth = 10;
        Noise3D tempNoise(noiseSize, noiseSize, depth);
        perl3.Generate(tempNoise);

        finalNoise.Fill([&tempNoise, currentTime, depth](Vector2i loc, float * outFl)
        {
            const float timeScale = 0.01f;

            //Get the Z layer.
            float z = currentTime * timeScale;
            z = std::fmodf(z, (float)(depth - 1));
            z = 0.0f;

            //Interpolate between layers to get the value.
            *outFl = BasicMath::Lerp(tempNoise[Vector3i(loc.x, loc.y, (int)floorf(z))],
                                     tempNoise[Vector3i(loc.x, loc.y, (int)ceilf(z))],
                                     BasicMath::Supersmooth(z - (int)floorf(z)));
            *outFl = tempNoise[Vector3i(loc.x, loc.y, 0)];
        });

        NoiseFilterer2D filter;
        MaxFilterRegion mfr;
        filter.FillRegion = &mfr;
        filter.Increase_Amount = 0.35f;
        filter.Increase(&finalNoise);
        filter.UpContrast_Power = NoiseFilterer2D::UpContrastPowers::QUINTIC;
        filter.UpContrast_Passes = 10;
        filter.UpContrast(&finalNoise);

        #pragma endregion
    }
    else if (false)
    {
        #pragma region Layered Interpolated White Noise


        WhiteNoise2D wn(fr.Seed);
        const unsigned int numbGens = 8;
        Interpolator2D int1(&wn, Interpolator2D::I2S_QUINTIC, 200.0f),
                       int2(&wn, Interpolator2D::I2S_QUINTIC, 100.0f),
                       int3(&wn, Interpolator2D::I2S_QUINTIC, 50.0f),
                       int4(&wn, Interpolator2D::I2S_QUINTIC, 25.0f),
                       int5(&wn, Interpolator2D::I2S_QUINTIC, 12.5f),
                       int6(&wn, Interpolator2D::I2S_QUINTIC, 6.25f),
                       int7(&wn, Interpolator2D::I2S_QUINTIC, 3.125f),
                       int8(&wn, Interpolator2D::I2S_QUINTIC, 1.0f);
        Generator2D * gens[] = { &int1, &int2, &int3, &int4, &int5, &int6, &int7, &int8 };
        float weights[numbGens];
        float counter = 0.5f;
        for (int i = 0; i < numbGens; ++i)
        {
            weights[i] = counter;
            counter *= 0.6f;
        }

        LayeredOctave2D layers(numbGens, weights, gens);
        layers.Generate(finalNoise);
        
        nf.UpContrast_Power = NoiseFilterer2D::UpContrastPowers::QUINTIC;
        nf.UpContrast(&finalNoise);


		#pragma endregion
    }
    else if (true)
    {
        #pragma region Neuroscouting racer terrain heightmap


        //Create a very rocky mountainous area, then put a thin ring of plateau inside it, then some hills inside that.

        Noise2D rockyNoise(noiseSize, noiseSize),
                plateauNoise(noiseSize, noiseSize),
                hillNoise(noiseSize, noiseSize);

        Perlin2D rockyGeneratorBase(10.0f, Perlin2D::Quintic, Vector2i(), fr.GetRandInt());
        nf.NoiseToFilter = &rockyGeneratorBase;
        nf.FilterFunc = &NoiseFilterer2D::UpContrast;
        nf.UpContrast_Passes = 1;
        nf.UpContrast_Power = NoiseFilterer2D::UpContrastPowers::CUBIC;
        nf.Generate(rockyNoise);
        rockyGeneratorBase.Generate(rockyNoise);

        FlatNoise2D plateauGeneratorBase(0.5f);
        nf.NoiseToFilter = &plateauGeneratorBase;
        nf.FilterFunc = &NoiseFilterer2D::Noise;
        nf.Noise_Seed = fr.GetRandInt();
        nf.Noise_Amount = 0.005f;
        nf.Generate(plateauNoise);

        Perlin2D hillGenerator(100.0f, Perlin2D::Quintic, Vector2i(), fr.GetRandInt());
        FlatNoise2D hillScale(0.25f);
        Combine2Noises2D hillFinalGen(&Combine2Noises2D::Multiply2, &hillGenerator, &hillScale);
        hillFinalGen.Generate(hillNoise);
        

        //Now assemble the noise together.
        
        const float halfNoiseSize = noiseSize * 0.5f,
                    halfNoiseInv = 1.0f / halfNoiseSize;
        const Vector2f noiseCenter(halfNoiseSize, halfNoiseSize);

        //Interpolate between the different segments of the noise.
        //These constants are relative to half the length/width of the noise texture.
        const float mountainBeginningRadius = 1.0f, //The distance at which the noise starts to be 100% rocky.
                    mountainStartFadeInRadius = 0.9f, //The distance at which the noise just starts to become rocky (from being a plateau).
                    plateauBeginningRadius = 0.765f, //The distance at which the noise starts to be 100% plateau (until the rocky noise interrupts it).
                    plateauStartFadeInRadius = 0.725f; //The distance at which the noise just starts to become plateau (from being hilly).

        Vector2f locF;
        for (Vector2i loc; loc.y < finalNoise.GetHeight(); ++loc.y)
        {
            locF.y = loc.y;

            for (loc.x = 0; loc.x < finalNoise.GetWidth(); ++loc.x)
            {
                locF.x = loc.x;
                
                //Come up with an interpolant that is on the same scale as the interpolation constants.
                float distLerp = locF.Distance(noiseCenter) * halfNoiseInv;
                float distortion = 0.02f * (-1.0f + (2.0f * FastRand(Vector3i(loc.x, loc.y, fr.Seed).GetHashCode()).GetZeroToOne()));
                distLerp += distortion;

                //Fully rocky.
                if (distLerp >= mountainBeginningRadius)
                {
                    finalNoise[loc] = rockyNoise[loc];
                }
                //Part rocky, part plateau.
                else if (distLerp >= mountainStartFadeInRadius)
                {
                    float lerpComponent = BasicMath::LerpComponent(mountainStartFadeInRadius, mountainBeginningRadius, distLerp);
                    finalNoise[loc] = BasicMath::Lerp(plateauNoise[loc], rockyNoise[loc], lerpComponent);
                }
                //Fully plateau.
                else if (distLerp >= plateauBeginningRadius)
                {
                    finalNoise[loc] = plateauNoise[loc];
                }
                //Part plateau, part hilly.
                else if (distLerp >= plateauStartFadeInRadius)
                {
                    float lerpComponent = BasicMath::LerpComponent(plateauStartFadeInRadius, plateauBeginningRadius, distLerp);
                    finalNoise[loc] = BasicMath::Lerp(hillNoise[loc], plateauNoise[loc], lerpComponent);
                }
                //Fully hilly.
                else
                {
                    finalNoise[loc] = hillNoise[loc];
                }
            }
        }


        #pragma endregion
    }
	else assert(false);


	InterpretNoise(finalNoise);
}
void NoiseTest::InterpretNoise(const Noise2D & noise)
{
	//Output noise to pixel array. The stack can't hold all the pixels, so put it on the heap.
	Array2D<sf::Uint8> * pixels = new Array2D<sf::Uint8>(pixelArrayWidth, pixelArrayHeight, 0); //The stack can't hold all these pixels.
	NoiseToPixels(noise, *pixels);


	//Output pixel array to texture.
	renderedNoiseTex = new sf::Texture();
	renderedNoiseTex->create(noiseSize, noiseSize);
	renderedNoiseTex->update(pixels->GetArray());


	//Output texture to Sprite.
	renderedNoise = new sf::Sprite(*renderedNoiseTex);


	//Clean up.
	delete pixels;
}

float bumpHeight = 1.0f;
void NoiseTest::UpdateWorld(float elapsedTime)
{
    const float bumpIncrement = 0.999f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        bumpHeight *= bumpIncrement;
        bumpHeight = BasicMath::Max(0.00001f, bumpHeight);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        bumpHeight /= bumpIncrement;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        Array2D<float> bumps(noiseSize, noiseSize);
        sf::Image rni = renderedNoiseTex->copyToImage();
        TextureConverters::ToArray(rni, ChannelsIn::CI_Blue, bumps);

        Array2D<Vector3f> normals(noiseSize, noiseSize);
        BumpmapToNormalmap::Convert(bumps, bumpHeight, true, normals);

        TextureConverters::ToImage(normals, rni);
        renderedNoiseTex->update(rni);
        renderedNoise->setTexture(*renderedNoiseTex);
    }


	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
	{
		ReGenerateNoise(true);
	}
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        renderedNoiseTex->copyToImage().saveToFile("RenderedNoise.png");
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        EndWorld();
    }
}

void NoiseTest::RenderWorld(float elapsedTime)
{
	GetWindow()->clear();

	GetWindow()->draw(*renderedNoise);

    sf::Text t(sf::String("Bump height: ") + sf::String(std::to_string(bumpHeight)), guiFont);
    t.setColor(sf::Color(0, 0, 0, 255));
    t.setPosition(3.0f, GetWindow()->getSize().y - 35.0f);
    GetWindow()->draw(t);
    t.setColor(sf::Color(255, 255, 255, 255));
    t.setPosition(0.0f, GetWindow()->getSize().y - 34.0f);
    GetWindow()->draw(t);

	GetWindow()->display();
}