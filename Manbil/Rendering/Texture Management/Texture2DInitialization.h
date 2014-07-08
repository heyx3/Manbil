#pragma once

#include "../../RenderDataHandler.h"
#include "TextureConverters.h"
#include "TextureSettings.h"


//TODO: Pull out into .cpp.
//TODO: Make UByte versions of these classes and related classes in MTexture and MTextureCubemap.


//Abstract class.
//Represents a way to initialize a 2D texture.
class Texture2DInitialization
{
public:

    //If necessary, writes the new width/height into the given settings. Otherwise, leaves it alone.
    //Returns whether the operation succeeded.
    virtual bool MakeTexture(const ColorTextureSettings & settings, Array2D<Vector4f> & outPixels) = 0;
};



//Initializes a texture with a flat color.
class Texture2DInitFlatColor : public Texture2DInitialization
{
public:

    Vector4f Color;
    Texture2DInitFlatColor(Vector4f color = Vector4f()) : Color(color) { }
    virtual bool MakeTexture(const ColorTextureSettings & settings, Array2D<Vector4f> & outPixels) override
    {
        outPixels.Reset(settings.Width, settings.Height, Color);
        return true;
    }
};


//Initializes a texture with float data using a function pointer.
class Texture2DInitFunction : public Texture2DInitialization
{
public:

    typedef void(*PixelGenerator)(Vector2i pixelLoc, Vector4f * outPixel, const void* data);

    const void* pData;
    PixelGenerator Generator;

    Texture2DInitFunction(PixelGenerator generator, const void* data = 0) : Generator(generator), pData(data) { }

    virtual bool MakeTexture(const ColorTextureSettings & settings, Array2D<Vector4f> & outPixels) override
    {
        const void* pDat = pData;
        PixelGenerator gen = Generator;

        outPixels.Reset(settings.Width, settings.Height);
        outPixels.Fill([pDat, &gen](Vector2i loc, Vector4f * outPixel) { gen(loc, outPixel, pDat); });

        return true;
    }
};


//Initializes a texture with data using a file path.
class Texture2DInitLoadFile : public Texture2DInitialization
{
public:

    std::string FilePath;
    
    Texture2DInitLoadFile(std::string filePath) : FilePath(filePath) { }

    virtual bool MakeTexture(const ColorTextureSettings & settings, Array2D<Vector4f> & outPixels) override
    {
        //Load the texture in using SFML.
        sf::Texture sfmlTex;
        if (!sfmlTex.loadFromFile(FilePath))
        {
            return false;
        }

        //Getthe pixel data from the SFML texture.
        TextureConverters::ToArray(sfmlTex, outPixels);

        return true;
    }
};


//Initializes a texture with given float data.
class Texture2DInitData : public Texture2DInitialization
{
public:

    const Vector4f * pData;
    unsigned int Width, Height;

    Texture2DInitData(unsigned int width, unsigned int height, const Vector4f * data = 0) : pData(data), Width(width), Height(height) { }
    Texture2DInitData(const Array2D<Vector4f> & pixels) : Width(pixels.GetWidth()), Height(pixels.GetHeight()), pData(pixels.GetArray()) { }

    virtual bool MakeTexture(const ColorTextureSettings & settings, Array2D<Vector4f> & outPixels) override
    {
        outPixels.Reset(Width, Height);
        outPixels.Fill(pData);

        return true;
    }
};