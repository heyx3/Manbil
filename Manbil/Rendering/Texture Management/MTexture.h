#pragma once

#include "../../RenderDataHandler.h"
#include "TextureConverters.h"
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
    
    //Creates an empty white texture. Deletes the texture this instance previously held.
    void Create(const ColorTextureSettings & texSettings);
    //Returns whether the file was successfully loaded. Deletes the texture this instance previously held.
    //Note that the width/height in "texSettings" will be ignored.
    bool Create(std::string filePath, const ColorTextureSettings & texSettings);

    //If this is a valid texture, deletes it from OpenGL.
    void DeleteIfValid(void);

    //Sets this texture as the active one.
    //If this isn't a valid texture, then the currently-active texture is just deactivated.
    void Bind(void) const { RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, texHandle); }

    void SetData(const Array2D<Vector4b> & inColor);
    void SetData(const Array2D<Vector4f> & inColor);
    void SetData(const Vector4b color, unsigned int width, unsigned int height);
    void SetData(const Vector4f color, unsigned int width, unsigned int height);


private:

    RenderObjHandle texHandle;
    ColorTextureSettings settings;
};