#pragma once

#include "../../RenderDataHandler.h"
#include "TextureSettings.h"
#include <SFML/Graphics/Texture.hpp>


//Represents a depth texture.
class MTextureDepth
{
public:

    //Constructors/destructors.

    MTextureDepth(void) : texHandle(0) { }
    MTextureDepth(MTextureDepth & cpy); //Intentionally not implemented.


    //Getters.

    RenderObjHandle GetTextureHandle(void) const { return texHandle; }
    bool IsValidTexture(void) const { return texHandle != 0; }

    const TextureSettings & GetSettings(void) const { return settings.BaseSettings; }
    unsigned int GetWidth(void) const { return settings.Width; }
    unsigned int GetHeight(void) const { return settings.Height; }
    bool UsesMipmaps(void) const { return settings.GenerateMipmaps; }
    DepthTextureSettings::PixelSizes GetPixelSize(void) const { return settings.PixelSize; }


    //Setters.

    void SetSettings(const TextureSettings & newSettings);

    void SetMinFilterType(TextureSettings::FilteringTypes newFiltering);
    void SetMagFilterType(TextureSettings::FilteringTypes newFiltering);
    void SetFilterType(TextureSettings::FilteringTypes newFiltering);

    void SetHorzWrappingType(TextureSettings::WrappingTypes wrapping);
    void SetVertWrappingType(TextureSettings::WrappingTypes wrapping);
    void SetWrappingType(TextureSettings::WrappingTypes wrapping);


    //Texture operations.

    //Deletes the previous texture held by this instance if one existed.
    void Create(const DepthTextureSettings & settings);

    //If this is a valid texture, deletes it from OpenGL.
    void DeleteIfValid(void);

    //Sets this texture as the active one.
    //If this isn't a valid texture, then the currently-active texture is just deactivated.
    void Bind(void) const { RenderDataHandler::BindTexture(TextureTypes::TT_2D, texHandle); }

    void SetData(const Array2D<unsigned char> & pixelData);
    void SetData(const Array2D<float> & pixelData);

    //Copies this texture's pixel data from the graphics card into the given array.
    //Automatically resizes the array to fit.
    void GetData(Array2D<unsigned char> & outData);
    //Copies this texture's pixel data from the graphics card into the given array.
    //Automatically resizes the array to fit.
    void GetData(Array2D<float> & outData);


private:

    RenderObjHandle texHandle;
    DepthTextureSettings settings;
};