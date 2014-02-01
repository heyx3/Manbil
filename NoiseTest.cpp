#include "NoiseTest.h"

#include <assert.h>

#include "Math/LowerMath.hpp"
#include "Math/NoiseGeneration.hpp"
#include "Math/Noise Generation/ColorGradient.h"


const int noiseSize = 725,
	pixelArrayWidth = noiseSize * 4,
	pixelArrayHeight = noiseSize;
#define GET_NOISE2D (Noise2D(noiseSize, noiseSize))

sf::Font guiFont;
void NoiseTest::InitializeWorld(void)
{
	guiFont = sf::Font();
	guiFont.loadFromFile("Candarab.ttf");

	GetWindow()->setPosition(sf::Vector2i(0, 0));
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
	else if (true)
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
	else if (false)
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
			readNoise = BasicMath::Clamp(tempF);

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

	sf::Text t(sf::String("Loading..."), guiFont);
	GetWindow()->draw(t);
	t.setColor(sf::Color(0, 0, 0, 255));
	t.setPosition(0.0f, 100.0f);
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
		#pragma region Grass


		Perlin per(40.0f, Perlin::Smoothness::Quintic, fr.Seed);
		per.Generate(finalNoise);


		nf.FilterFunc = &NoiseFilterer::Increase;
		mfr.ActiveIn = Interval(0.0f, 1.0f, 0.001f, true, true);
		mfr.StrengthLerp = 1.0f;
		nf.Increase_Amount = 0.0f;
		nf.Generate(finalNoise);

		nf.FilterFunc = &NoiseFilterer::UpContrast;
		mfr.ActiveIn = Interval(0.0f, 1.0f, 0.001f, true, true);
		mfr.StrengthLerp = 0.5f;
		nf.Generate(finalNoise);

		nf.FilterFunc = &NoiseFilterer::Noise;
		mfr.ActiveIn = Interval(0.25f, 0.35f, 0.001f, true, true);
		mfr.StrengthLerp = 1.0f;
		nf.Noise_Amount = 0.08f;
		nf.Noise_Seed = rand();
		nf.Generate(finalNoise);

		nf.FilterFunc = &NoiseFilterer::Smooth;
		mfr.ActiveIn = Interval(0.0f, 1.0f, 0.001f, true, true);
		mfr.StrengthLerp = 1.0f;
		nf.Generate(finalNoise);

		nf.FilterFunc = &NoiseFilterer::Noise;
		mfr.ActiveIn = Interval(0.3001f, 1.0f, 0.00001f, true, true);
		mfr.StrengthLerp = 1.0f;
		nf.Noise_Amount = 0.05f;
		nf.Noise_Seed = rand();
		nf.Generate(finalNoise);

		nf.FilterFunc = &NoiseFilterer::Noise;
		mfr.ActiveIn = Interval(0.0f, 1.0f, 0.001f, true, true);
		mfr.StrengthLerp = 1.0f;
		nf.Noise_Amount = 0.05f;
		nf.Noise_Seed = rand();
		nf.Generate(finalNoise);


		#pragma endregion
	}
	else if (false)
	{
		#pragma region Faint bubbly clouds


		fr.Seed = fr.GetRandInt();
		Worley wor(fr.GetRandInt(), 300, Interval(20, 2));
		wor.DistFunc = &Worley::QuadraticDistance;
		wor.ValueGenerator = [](Worley::DistanceValues v) { return -v.Values[0] + v.Values[5]; };
		wor.Generate(finalNoise);

		NoiseFilterer nf;
		nf.FilterFunc = &NoiseFilterer::Average;
		MaxFilterRegion mfr;
		mfr.StrengthLerp = 0.5f;
		nf.FillRegion = &mfr;
		nf.Noise_Seed = fr.GetRandInt();
		nf.Generate(finalNoise);


		#pragma endregion
	}
	else if (true)
	{
		#pragma region Layered Perlin


		const int numb = 6;

		Generator * pers[numb] = { new Perlin(200.0f, Perlin::Smoothness::Cubic, fr.Seed),
								   new Perlin(100.0f, Perlin::Smoothness::Cubic, fr.Seed + 12456),
								   new Perlin(50.0f, Perlin::Smoothness::Cubic, fr.Seed + 5643457),
								   new Perlin(25.0f, Perlin::Smoothness::Cubic, fr.Seed + 235678),
								   new Perlin(12.5f, Perlin::Smoothness::Linear, fr.Seed + 358746),
								   new Perlin(1.0f, Perlin::Smoothness::Linear, fr.Seed + 987654),
						 };
		float scales[numb] = { 0.5f, 0.25f, 0.125f, 0.0625f, 0.03125f, 0.015625f };


		LayeredOctave lo(numb, scales, pers);
		lo.Generate(finalNoise);

		for (int i = 0; i < numb; ++i) delete pers[i];


		NoiseFilterer nf;
		MaxFilterRegion mfr;
		nf.FillRegion = &mfr;
		nf.FilterFunc = &NoiseFilterer::UpContrast;
		nf.UpContrast_Power = NoiseFilterer::UpContrastPowers::QUINTIC;
		mfr.StrengthLerp = 1.0f;

		nf.Generate(finalNoise);



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