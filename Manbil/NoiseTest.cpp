#include "NoiseTest.h"

#include <assert.h>
#include <iostream>

#include "Math/LowerMath.hpp"
#include "Math/NoiseGeneration.hpp"
#include "Math/Noise Generation/ColorGradient.h"
#include "Math/Higher Math/BumpmapToNormalmap.h"
#include "Math/Higher Math/Gradient.h"



const int noiseSize = 513,
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
	//Set the color gradient.
    std::vector<GradientNode<3>> nodes;
	if (true)
	{
		#pragma region Black and white

        float zeroes[3] = { 0.0f, 0.0f, 0.0f },
              ones[3] = { 1.0f, 1.0f, 1.0f };
        nodes.insert(nodes.end(), GradientNode<3>(0.0f, zeroes));
        nodes.insert(nodes.end(), GradientNode<3>(1.0f, ones));

		#pragma endregion
	}
	else assert(false);


    //Generate the color gradient.
    Gradient<3> colGrad(nodes, Gradient<3>::SM_LINEAR);
    for (Vector2u loc; loc.y < noise.GetHeight(); ++loc.y)
    {
        for (loc.x = 0; loc.x < noise.GetWidth(); ++loc.x)
        {
            float noiseVal = BasicMath::Clamp(noise[loc], 0.0f, 1.0f);

            float gradientCol[3] = { 0.0f, 0.0f, 0.0f };
            colGrad.GetValue(noiseVal, gradientCol);

            Vector3b colorB((sf::Uint8)BasicMath::RoundToInt(gradientCol[0] * 255.0f),
                            (sf::Uint8)BasicMath::RoundToInt(gradientCol[1] * 255.0f),
                            (sf::Uint8)BasicMath::RoundToInt(gradientCol[2] * 255.0f));

            Vector2u pixelStart(loc.x * 4, loc.y);
            outPixels[pixelStart] = colorB.x;
            outPixels[Vector2u(pixelStart.x + 1, pixelStart.y)] = colorB.y;
            outPixels[Vector2u(pixelStart.x + 2, pixelStart.y)] = colorB.z;
            outPixels[Vector2u(pixelStart.x + 3, pixelStart.y)] = 255;
        }
    }

    nodes = nodes;
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


#pragma warning(disable: 4127)

	if (false)
	{
		#pragma region Layered Perlin

        const int pScale = 4;
		Perlin2D per1(128.0f, Perlin2D::Smoothness::Cubic, Vector2i(), fr.Seed, true, Vector2u(1, 1) * pScale),
                 per2(64.0f, Perlin2D::Smoothness::Cubic, Vector2i(), fr.Seed + 634356, true, Vector2u(2, 2) * pScale),
                 per3(32.0f, Perlin2D::Smoothness::Cubic, Vector2i(), fr.Seed + 6193498, true, Vector2u(4, 4) * pScale),
                 per4(16.0f, Perlin2D::Smoothness::Cubic, Vector2i(), fr.Seed + 1009346, true, Vector2u(8, 8) * pScale),
                 per5(8.0f, Perlin2D::Smoothness::Quintic, Vector2i(), fr.Seed + 619398, true, Vector2u(16, 16) * pScale),
                 per6(4.0f, Perlin2D::Smoothness::Quintic, Vector2i(), fr.Seed + 45324, true, Vector2u(32, 32) * pScale),
                 per7(2.0f, Perlin2D::Smoothness::Quintic, Vector2i(), fr.Seed + 21234, true, Vector2u(64, 64) * pScale);
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

        //PRIORITY: Fix Worley noise.

		fr.Seed = fr.GetRandInt();
		Worley2D wor(fr.GetRandInt(), 64, 5, 10);
        wor.DistFunc = [](Vector2f o, Vector2f p) { return o.Distance(p); };
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

        const unsigned int pSize = 32;
        Perlin2D perlin(20.0f, Perlin2D::Quintic, Vector2i(), fr.Seed, true, Vector2u(noiseSize / 20, noiseSize / 20));
        perlin.Generate(finalNoise);

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

        Perlin3D perl3(Vector3f(64.0f, 32.0f, 5.0f), Perlin3D::Cubic, ToV3i(offset), 123456);
        const int depth = 10;
        Noise3D tempNoise(noiseSize, noiseSize, depth);
        perl3.Generate(tempNoise);

        finalNoise.FillFunc([&tempNoise, currentTime, depth](Vector2u loc, float * outFl)
        {
            const float timeScale = 0.01f;

            //Get the Z layer.
            float z = currentTime * timeScale;
            assert(z >= -0.00001f);
            z = std::fmodf(z, (float)(depth - 1));
            z = 0.0f;

            //Interpolate between layers to get the value.
            *outFl = BasicMath::Lerp(tempNoise[Vector3u(loc.x, loc.y, (unsigned int)floorf(z))],
                                     tempNoise[Vector3u(loc.x, loc.y, (unsigned int)ceilf(z))],
                                     BasicMath::Supersmooth(z - floorf(z)));
            *outFl = tempNoise[Vector3u(loc.x, loc.y, 0)];
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
    else if (false)
    {
        #pragma region Neuroscouting racer terrain heightmap


        //Create a very rocky mountainous area, then put a thin ring of plateau inside it, then some hills inside that.

        Noise2D rockyNoise(noiseSize, noiseSize),
                plateauNoise(noiseSize, noiseSize),
                hillNoise(noiseSize, noiseSize);

        Perlin2D rockyGeneratorBase(50.0f, Perlin2D::Quintic, Vector2i(), fr.GetRandInt());
        nf.NoiseToFilter = &rockyGeneratorBase;
        nf.FilterFunc = &NoiseFilterer2D::RemapValues;
        nf.RemapValues_OldVals = Interval::GetZeroToOne();
        nf.RemapValues_NewVals = Interval(0.85f, 1.0f, 0.00001f);
        nf.Generate(rockyNoise);

        FlatNoise2D plateauGeneratorBase(0.5f);
        nf.NoiseToFilter = &plateauGeneratorBase;
        nf.FilterFunc = &NoiseFilterer2D::Noise;
        nf.Noise_Seed = fr.GetRandInt();
        nf.Noise_Amount = 0.001f;
        nf.Generate(plateauNoise);

        Perlin2D hillGenerator(100.0f, Perlin2D::Quintic, Vector2i(), fr.GetRandInt());
        nf.NoiseToFilter = &hillGenerator;
        nf.FilterFunc = &NoiseFilterer2D::RemapValues;
        nf.RemapValues_OldVals = Interval::GetZeroToOne();
        nf.RemapValues_NewVals = Interval(0.0f, 0.4f, 0.00001f);
        nf.Generate(hillNoise);
        

        //Now assemble the noise together.
        
        const float halfNoiseSize = noiseSize * 0.5f,
                    halfNoiseInv = 1.0f / halfNoiseSize;
        const Vector2f noiseCenter(halfNoiseSize, halfNoiseSize);

        //Interpolate between the different segments of the noise.
        //These constants are relative to half the length/width of the noise texture.
        const float mountainBeginningRadius = 1.0f, //The distance at which the noise starts to be 100% rocky.
                    mountainStartFadeInRadius = 0.8f, //The distance at which the noise just starts to become rocky (from being a plateau).
                    plateauBeginningRadius = 0.765f, //The distance at which the noise starts to be 100% plateau (until the rocky noise interrupts it).
                    plateauStartFadeInRadius = 0.725f; //The distance at which the noise just starts to become plateau (from being hilly).

        Vector2f locF;
        for (Vector2u loc; loc.y < finalNoise.GetHeight(); ++loc.y)
        {
            locF.y = (float)loc.y;

            for (loc.x = 0; loc.x < finalNoise.GetWidth(); ++loc.x)
            {
                locF.x = (float)loc.x;
                
                //Come up with an interpolant that is on the same scale as the interpolation constants.
                float distLerp = locF.Distance(noiseCenter) * halfNoiseInv;
                float distortion = 0.01f * (-1.0f + (2.0f * FastRand(Vector3i((int)loc.x, (int)loc.y, fr.Seed).GetHashCode()).GetZeroToOne()));
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
    else if (false)
    {
        #pragma region Diamond-Square

        DiamondSquareStep variances[] =
        {
            Interval(0.0f, 320.0f),
            Interval(0.0f, 260.0f),
            Interval(0.0f, 137.0f),
            Interval(0.0f, 80.0f),
            Interval(0.0f, 50.0f),
            Interval(0.0f, 35.0f),
            Interval(0.0f, 25.0f),
            Interval(0.0f, 12.0f),
            Interval(0.0f, 5.0f),
        };

        finalNoise.Fill(BasicMath::NaN);
        DiamondSquare dsq(fr.Seed, Interval(0.0f, 3.0f), variances, sizeof(variances) / sizeof(DiamondSquareStep), 0.0f);
        dsq.Generate(finalNoise);

        NoiseAnalysis2D::MinMax mm = NoiseAnalysis2D::GetMinAndMax(finalNoise);
        nf.RemapValues_OldVals = Interval(mm.Min, mm.Max, 0.00001f, true, false);
        nf.RemapValues_NewVals = Interval(0.0f, 1.0f, 0.00001f);
        nf.RemapValues(&finalNoise);

        #pragma endregion
    }
	else assert(false);

#pragma warning(default: 4127)


	InterpretNoise(finalNoise);
}
void NoiseTest::InterpretNoise(const Noise2D & noise)
{
	Array2D<sf::Uint8> pixels(pixelArrayWidth, pixelArrayHeight, 0);
	NoiseToPixels(noise, pixels);


	//Output pixel array to texture.
	renderedNoiseTex = new sf::Texture();
	renderedNoiseTex->create(noiseSize, noiseSize);
	renderedNoiseTex->update(pixels.GetArray());


	//Output texture to Sprite.
	renderedNoise = new sf::Sprite(*renderedNoiseTex);
}

float bumpHeight = 1.0f;
bool pressedLast = false;
void NoiseTest::UpdateWorld(float elapsedTime)
{
    const float bumpIncrement = 0.99f;
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
        if (!pressedLast)
        {
            pressedLast = true;

            //Get texture data as an array.
            Array2D<Vector4b> texColor(0, 0);
            sf::Image img = renderedNoiseTex->copyToImage();
            texColor.Reset(img.getSize().x, img.getSize().y);
            texColor.Fill((Vector4b*)img.getPixelsPtr(), true);

            //Convert it to a heightmap array.
            Array2D<float> bumps(noiseSize, noiseSize);
            bumps.FillFunc([&texColor](Vector2u loc, float* outVal) { *outVal = (float)texColor[loc].z / 255.0f; });

            //Convert the heightmap to a normal map.
            Array2D<Vector3f> normals(noiseSize, noiseSize);
            BumpmapToNormalmap::Convert(bumps, bumpHeight, true, normals);

            //Output the normal map to the texture.
            Array2D<Vector4b> normalColors(noiseSize, noiseSize);
            normalColors.FillFunc([&normals](Vector2u loc, Vector4b * outCol)
            {
                Vector3f c = normals[loc];
                *outCol = Vector4b((unsigned char)BasicMath::Min(255.0f, c.x * 255.0f),
                                   (unsigned char)BasicMath::Min(255.0f, c.y * 255.0f),
                                   (unsigned char)BasicMath::Min(255.0f, c.z * 255.0f),
                                   255);
            });
            renderedNoiseTex->update((sf::Uint8*)normalColors.GetArray());

            //Update the sprite's texture.
            //TODO: I'm fairly certain this isn't necessary, as the texture handle doesn't change.
            renderedNoise->setTexture(*renderedNoiseTex);
        }
    }
    else pressedLast = false;


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