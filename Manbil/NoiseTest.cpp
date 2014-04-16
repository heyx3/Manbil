#include "NoiseTest.h"

#include <assert.h>

#include "Math/LowerMath.hpp"
#include "Math/NoiseGeneration.hpp"
#include "Math/Noise Generation/ColorGradient.h"


const int noiseSize = 256,
	pixelArrayWidth = noiseSize * 4,
	pixelArrayHeight = noiseSize;
#define GET_NOISE2D (Noise2D(noiseSize, noiseSize))

sf::Font guiFont;
void NoiseTest::InitializeWorld(void)
{
	guiFont = sf::Font();
	guiFont.loadFromFile("Content/Fonts/Candarab.ttf");

	//GetWindow()->setPosition(sf::Vector2i(0, 0));
	GetWindow()->setSize(sf::Vector2u(noiseSize, noiseSize));
	SetWindowTitle(sf::String("Noise test"));

	ReGenerateNoise(false);
}

void NoiseToPixels(const Noise2D & noise, Fake2DArray<sf::Uint8> & outPixels)
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

	NoiseFilterer nf;
	MaxFilterRegion mfr;
	nf.FillRegion = &mfr;


	if (false)
	{
		#pragma region Layered Perlin


		Perlin2D per1(128.0f, Perlin2D::Smoothness::Quintic, Vector2i(), fr.Seed),
                 per2(64.0f, Perlin2D::Smoothness::Quintic, Vector2i(), fr.Seed + 634356),
                 per3(32.0f, Perlin2D::Smoothness::Quintic, Vector2i(), fr.Seed + 6193498),
                 per4(16.0f, Perlin2D::Smoothness::Quintic, Vector2i(), fr.Seed + 1009346),
                 per5(8.0f, Perlin2D::Smoothness::Quintic, Vector2i(), fr.Seed + 6193498),
                 per6(4.0f, Perlin2D::Smoothness::Quintic, Vector2i(), fr.Seed + 6193498),
                 per7(2.0f, Perlin2D::Smoothness::Quintic, Vector2i(), fr.Seed + 6193498);
        Generator * gens[] = { &per1, &per2, &per3, &per4, &per5, &per6, &per7 };
        float weights[7];
        float counter = 0.5f;
        for (int i = 0; i < 7; ++i)
        {
            weights[i] = counter;
            counter *= 0.5f;
        }

        LayeredOctave layers(7, weights, gens);

        
		nf.FilterFunc = &NoiseFilterer::UpContrast;
        nf.UpContrast_Power = NoiseFilterer::UpContrastPowers::QUINTIC;
        nf.NoiseToFilter = &layers;
		nf.Generate(finalNoise);

        per4.Generate(finalNoise);
        

		#pragma endregion
	}
	else if (false)
	{
		#pragma region Worley

		fr.Seed = fr.GetRandInt();
		Worley wor(fr.GetRandInt(), 256, Interval(10, 2));
		wor.DistFunc = &Worley::StraightLineDistance;
		wor.ValueGenerator = [](Worley::DistanceValues v) { return -v.Values[0] + v.Values[5]; };
		wor.Generate(finalNoise);

		#pragma endregion
	}
	else if (false)
	{
        #pragma region Water

        Value2D basicNoise(fr.Seed);
        Interpolator interpNoise(&basicNoise, noiseSize, noiseSize, 10.0);
        interpNoise.Generate(finalNoise);

        NoiseFilterer nf;
        MaxFilterRegion mfr(1.0f, Interval(0.0f, 1.0f, 0.0001f, true, true));
        nf.FillRegion = &mfr;
        //nf.Increase_Amount = -1.0f;
        //nf.Increase(&finalNoise);

        nf.UpContrast_Power = NoiseFilterer::UpContrastPowers::QUINTIC;
        //nf.UpContrast(&finalNoise);
        //nf.UpContrast(&finalNoise);
        //nf.UpContrast(&finalNoise);

        #pragma endregion
	}
    else if (false)
    {
        #pragma region TwoD Perlin

        float offset = GetTotalElapsedSeconds() * 25.0f;
        Vector2i offsetVal((int)offset, (int)offset);
        std::cout << "Offset: " << offset << "\n";

        Perlin2D perl(64.0f, Perlin2D::Quintic, offsetVal, 6432235);
        perl.Generate(finalNoise);

        #pragma endregion
    }
    else if (true)
    {
        #pragma region ThreeD Perlin

        float currentTime = GetTotalElapsedSeconds();
        Vector3f offset(currentTime, currentTime, currentTime);
        offset *= 20.0f;

        Perlin3D perl3(32.0f, Perlin3D::Cubic, offset.CastToInt(), 123456);
        const int depth = 5;
        Noise3D tempNoise(noiseSize, noiseSize, depth);
        perl3.Generate(tempNoise);

        finalNoise.Fill([&tempNoise, currentTime, depth](Vector2i loc, float * outFl)
        {
        const float timeScale = 6.0f;
            //Get the Z layer.
            float z = currentTime * timeScale;
            z = std::fmodf(z, (float)(depth - 1));
            z = 3.0f;

            //Interpolate between layers to get the value.
            *outFl = BasicMath::Lerp(tempNoise[Vector3i(loc.x, loc.y, (int)floorf(z))],
                                     tempNoise[Vector3i(loc.x, loc.y, (int)ceilf(z))],
                                     BasicMath::Supersmooth(z - (int)floorf(z)));
        });

        NoiseFilterer filter;
        MaxFilterRegion mfr;
        filter.FillRegion = &mfr;
        filter.Increase_Amount = 0.2f;
        filter.Increase(&finalNoise);
        filter.UpContrast_Power = NoiseFilterer::UpContrastPowers::QUINTIC;
        filter.UpContrast(&finalNoise);

        #pragma endregion
    }
	else assert(false);


	InterpretNoise(finalNoise);
}
void NoiseTest::InterpretNoise(const Noise2D & noise)
{
	//Output noise to pixel array. The stack can't hold all the pixels, so put it on the heap.
	Fake2DArray<sf::Uint8> * pixels = new Fake2DArray<sf::Uint8>(pixelArrayWidth, pixelArrayHeight, 0); //The stack can't hold all these pixels.
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

void NoiseTest::UpdateWorld(float elapsedTime)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
	{
		ReGenerateNoise(true);
	}
}

void NoiseTest::RenderWorld(float elapsedTime)
{
	GetWindow()->clear();
	GetWindow()->draw(*renderedNoise);
	GetWindow()->display();
}