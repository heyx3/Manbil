#include "NoiseGenWorld.h"

#include "../Rendering/Rendering.hpp"
#include "../Rendering/GUI/GUIMaterials.h"

#include "../Math/NoiseGeneration.hpp"
#include "../Math/Higher Math/BumpmapToNormalmap.h"

#include <iostream>


NoiseGenWorld::NoiseGenWorld(void)
    : windowSize(256, 256), SFMLOpenGLWorld(256, 256), rng(13123),
      noiseTex(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PixelSizes::PS_8U, false),
      guiTexColor(UniformDictionary(), &noiseTex, 0),
      guiTexGreyscale(UniformDictionary(), &noiseTex, 0)
{

}


bool NoiseGenWorld::Assert(bool expr, const char* errIntro, std::string& err)
{
    if (!expr)
    {
        std::cout << errIntro << ": " << err << "\n";
        char dummy;
        std::cin >> dummy;

        OnWorldEnd();
        EndWorld();
    }
    return expr;
}

sf::VideoMode NoiseGenWorld::GetModeToUse(unsigned int windowW, unsigned int windowH)
{
    return sf::VideoMode(windowW, windowH);
}
std::string NoiseGenWorld::GetWindowTitle(void)
{
    return "NoiseGenWorld";
}
sf::Uint32 NoiseGenWorld::GetSFStyleFlags(void)
{
    return sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close;
}
sf::ContextSettings NoiseGenWorld::GenerateContext(void)
{
    return sf::ContextSettings(24, 0, 0, 4, 1);
}


void NoiseGenWorld::GenerateNoise(MTexture2D& tex)
{
    //Generators are defined in Math/NoiseGeneration.hpp.
    //The base generator type is defined in Math/Noise Generation/BasicGenerators.h.
    //The system can do both 2D and 3D noise generation.

    //The type "Array2D<float>" is helpfully typedef-ed as "Noise2D".
    Noise2D noiseMap(windowSize.x, windowSize.y);

    //Several types of noise are provided below; pick whichever one you want to try.
    const int method = 2;
    switch (method)
    {
        case 0:
        {
            //Use standard Worley noise.
            Worley2D worley;

            worley.Seed = rng.GetRandInt();
            worley.Generate(noiseMap);
        }
        break;

        case 1:
        {
            //Use Worley noise with special settings.
            Worley2D worley;
            worley.CellSize = 23;
            worley.Variability = Vector2f(1.0f, 1.0f);
            worley.DistFunc = &Worley2D::StraightLineDistance;
            worley.ValueGenerator = [](Worley2D::DistanceValues dists)
            {
                //Use the distance to the second-closest point.
                return dists.Values[0];
            };

            worley.Seed = rng.GetRandInt();
            worley.Generate(noiseMap);
        }
        break;

        case 2:
        {
            //Use a 2D slice of 3D Worley noise.
            //This creates essentially the same effect as 2D worley noise (although more bias towards 1.0),
            //    but this extra step helps illustrate the relationship between 2D and 3D noise.

            Worley3D worley;
            worley.CellSize = 32;
            worley.CellOffset.z = (unsigned int)(GetTotalElapsedSeconds() * 1.0f);

            //Use a 3D array with a depth of 1, in order to get a small slice of the full 3D noise.
            Noise3D noiseMap3(noiseMap.GetWidth(), noiseMap.GetHeight(), 1);
            worley.Generate(noiseMap3);

            //Because our 3D noise array has a depth of 1, it is identical to a 2D array.
            assert(noiseMap.GetNumbElements() == noiseMap3.GetNumbElements());
            noiseMap3.CopyInto(noiseMap.GetArray());
        }
        break;

        case 3:
        {
            //Start with several layers of Perlin noise at different scales to create a cloudy effect.

            Perlin2D layers[] =
            {
                Perlin2D(128.0f, Perlin2D::Quintic, Vector2i(), rng.GetRandInt(), true,
                            Vector2u(windowSize.x / 128, windowSize.y / 128)),
                Perlin2D(64.0f, Perlin2D::Quintic, Vector2i(), rng.GetRandInt(), true,
                            Vector2u(windowSize.x / 64, windowSize.y / 64)),
                Perlin2D(32.0f, Perlin2D::Quintic, Vector2i(), rng.GetRandInt(), true,
                            Vector2u(windowSize.x / 32, windowSize.y / 32)),
                Perlin2D(16.0f, Perlin2D::Quintic, Vector2i(), rng.GetRandInt(), true,
                            Vector2u(windowSize.x / 16, windowSize.y / 16)),
                Perlin2D(8.0f, Perlin2D::Quintic, Vector2i(), rng.GetRandInt(), true,
                            Vector2u(windowSize.x / 8, windowSize.y / 8)),
                Perlin2D(4.0f, Perlin2D::Quintic, Vector2i(), rng.GetRandInt(), true,
                            Vector2u(windowSize.x / 4, windowSize.y / 4)),
            };
            float weights[] = { 0.5f, 0.25f, 0.125f, 0.0625f, 0.03125f, 0.015625f };

            Generator2D* generatorPtrs[] =
            {
                &layers[0],
                &layers[1],
                &layers[2],
                &layers[3],
                &layers[4],
                &layers[5],
            };
            LayeredOctave2D layerNoise(6, weights, generatorPtrs);


            //Filter the layered noise to increase contrast.

            NoiseFilterer2D noiseFilter;

            noiseFilter.NoiseToFilter = &layerNoise;

            //We can selectively set what part of the noise gets filtered,
            //    but in this case we want to filter the whole thing.
            MaxFilterRegion fullArea;
            noiseFilter.FillRegion = &fullArea;

            //One of the filter functions provided is "UpContrast", which does exactly what we want.
            noiseFilter.FilterFunc = &NoiseFilterer2D::UpContrast;
            noiseFilter.UpContrast_Power = NoiseFilterer2D::QUINTIC;
            noiseFilter.UpContrast_Passes = 2;

            //Run the generator.
            Generator2D* rootGenerator = &noiseFilter;
            rootGenerator->Generate(noiseMap);
        }
        break;

        case 4:
        {
            //Generate white noise on a grid and interpolate between those grid values.
            //This produces a result similar to Perlin noise, but much less expensive.
            WhiteNoise2D whiteNoise(rng.GetRandInt());
            Interpolator2D interpNoise(&whiteNoise, Interpolator2D::I2S_QUINTIC, 50.0f);
            interpNoise.Generate(noiseMap);
        }
        break;

        default:
            Assert(false, "Unknown generation method", std::to_string(method));
    }

    //Put the result into the texture.
    tex.SetGreyscaleData(noiseMap, PS_32F_GREYSCALE);
}
void NoiseGenWorld::GenerateBumpMap(MTexture2D& noiseTex)
{
    //Get the noise.
    Array2D<float> noise(noiseTex.GetWidth(), noiseTex.GetHeight());
    noiseTex.GetGreyscaleData(noise);

    //Treat it as a bumpmap and convert to a normalmap.
    Array2D<Vector3f> normals(noiseTex.GetWidth(), noiseTex.GetHeight());
    BumpmapToNormalmap::Convert(noise, bumpmapHeight, true, normals);

    //Put the normalmap into the texture.
    Array2D<Vector4f> texCols(noiseTex.GetWidth(), noiseTex.GetHeight());
    texCols.FillFunc([&normals](Vector2u loc, Vector4f* outVal)
    {
        *outVal = Vector4f(normals[loc], 1.0f);
    });
    noiseTex.SetColorData(texCols, PS_32F);
}

void NoiseGenWorld::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();

    //If there was an error initializing the window/OpenGL, don’t bother with
    //    the rest of initialization.
    if (IsGameOver())
    {
        return;
    }


    DrawingQuad::InitializeQuad();

    
    //Initialize materials.
    GUIMaterials::GenMat tryMat("");

    //Greyscale material.
    tryMat = GUIMaterials::GenerateStaticQuadDrawMaterial(guiTexGreyscale.Params,
                                                          GUIMaterials::TT_GREYSCALE,
                                                          DrawingQuad::GetVertexInputData(), 0, 1, true,
                                                          true);
    if (!Assert(tryMat.ErrorMessage.empty(), "Error creating greyscale material", tryMat.ErrorMessage))
    {
        return;
    }
    guiTexGreyscale.Mat = tryMat.Mat;

    //Color material.
    tryMat = GUIMaterials::GenerateStaticQuadDrawMaterial(guiTexColor.Params, GUIMaterials::TT_COLOR,
                                                          DrawingQuad::GetVertexInputData(), 0, 1, true,
                                                          true);
    if (!Assert(tryMat.ErrorMessage.empty(), "Error creating color material", tryMat.ErrorMessage))
    {
        return;
    }
    guiTexColor.Mat = tryMat.Mat;


    //Set up the rendering camera.
    cam = Camera(Vector3f(), Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f));
    cam.MinOrthoBounds = Vector3f(0.0f, 0.0f, -1.0f);
    cam.MaxOrthoBounds = Vector3f(1.0f, 1.0f, 1.0f);


    std::cout << "Use left/right arrow keys to change bumpmap height, Space to re-generate, " <<
                 "and Enter to convert to bumpmap.\n";

    std::cout << "\nRegenerating...\n";
    noiseTex.Create();
    GenerateNoise(noiseTex);
    std::cout << "Generated!\n\n";
}
void NoiseGenWorld::OnWorldEnd(void)
{
    if (guiTexGreyscale.Mat != 0)
    {
        delete guiTexGreyscale.Mat;
        guiTexGreyscale.Mat = 0;
    }
    if (guiTexColor.Mat != 0)
    {
        delete guiTexColor.Mat;
        guiTexColor.Mat = 0;
    }

    noiseTex.DeleteIfValid();

    DrawingQuad::DestroyQuad();
}


void NoiseGenWorld::UpdateWorld(float elapsedSeconds)
{
    //Handle input.
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        EndWorld();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        std::cout << "\nGenerating...\n";
        GenerateNoise(noiseTex);
        std::cout << "Generated!\n\n";
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return) &&
        IsPixelSizeGreyscale(noiseTex.GetPixelSize()))
    {
        std::cout << "\nConverting to normalmap...\n";
        GenerateBumpMap(noiseTex);
        std::cout << "Converted!\n\n";
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        bumpmapHeight *= 0.95f;
        std::cout << "New bumpmap height is " << bumpmapHeight << "\n";
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        bumpmapHeight /= 0.95f;
        std::cout << "New bumpmap height is " << bumpmapHeight << "\n";
    }
}
void NoiseGenWorld::RenderOpenGL(float elapsedSeconds)
{
    //Set up rendering state.
    ScreenClearer(true, true, false, Vector4f(0.2f, 0.2f, 0.3f, 0.0f)).ClearScreen();
    RenderingState(RenderingState::C_NONE, true, true,
                   RenderingState::AT_GREATER, 0.0f).EnableState();
    Viewport(0, 0, windowSize.x, windowSize.y).Use();

    Matrix4f viewM, projM;
    cam.GetViewTransform(viewM);
    cam.GetOrthoProjection(projM);

    RenderInfo info(GetTotalElapsedSeconds(), &cam, &viewM, &projM);
    if (IsPixelSizeColor(noiseTex.GetPixelSize()))
    {
        guiTexColor.SetBounds(Box2D(0.0f, 1.0f, 0.0f, 1.0f));
        guiTexColor.Render(elapsedSeconds, info);
    }
    else
    {
        guiTexGreyscale.SetBounds(Box2D(0.0f, 1.0f, 0.0f, 1.0f));
        guiTexGreyscale.Render(elapsedSeconds, info);
    }
}


void NoiseGenWorld::OnInitializeError(std::string errorMsg)
{
    //Print the error message and end the game.
    SFMLOpenGLWorld::OnInitializeError(errorMsg);
    char dummy;
    std::cin >> dummy;
    EndWorld();
}
void NoiseGenWorld::OnWindowResized(unsigned int newWidth, unsigned int newHeight)
{
    windowSize.x = newWidth;
    windowSize.y = newHeight;
}