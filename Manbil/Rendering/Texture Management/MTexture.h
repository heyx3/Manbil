#pragma once

#include "../../RenderDataHandler.h"
#include "TextureSettings.h"
#include <SFML/Graphics/Texture.hpp>


//Represents a texture.
class MTexture
{
public:

    //Constructors/destructors.

    MTexture(void) : texHandle(0) { }
    MTexture(MTexture & cpy); //Intentionally not implemented.


    //Getters.

    RenderObjHandle GetTextureHandle(void) const { return texHandle; }
    bool IsValidTexture(void) const { return texHandle != 0; }

    const TextureSettings & GetSettings(void) const { return settings.BaseSettings; }
    unsigned int GetWidth(void) const { return settings.Width; }
    unsigned int GetHeight(void) const { return settings.Height; }
    bool UsesMipmaps(void) const { return settings.GenerateMipmaps; }
    ColorTextureSettings::PixelSizes GetPixelSize(void) const { return settings.PixelSize; }


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
    void Create(const ColorTextureSettings & settings, const Array2D<Vector4b> & pixelData);
    //Deletes the previous texture held by this instance if one existed.
    void Create(const ColorTextureSettings & settings, const Array2D<Vector4f> & pixelData);

    //If this is a valid texture, deletes it from OpenGL.
    void DeleteIfValid(void);

    //Sets this texture as the active one.
    //If this isn't a valid texture, then the currently-active texture is just deactivated.
    void Bind(void) const { RenderDataHandler::BindTexture(TextureTypes::TT_2D, texHandle); }

    void SetData(const Array2D<Vector4b> & pixelData);
    void SetData(const Array2D<Vector4f> & pixelData);

    //Copies this texture's pixel data from the graphics card into the given array.
    //Automatically resizes the array to fit.
    void GetData(Array2D<Vector4b> & outData);
    //Copies this texture's pixel data from the graphics card into the given array.
    //Automatically resizes the array to fit.
    void GetData(Array2D<Vector4f> & outData);


private:

    RenderObjHandle texHandle;
    ColorTextureSettings settings;
};