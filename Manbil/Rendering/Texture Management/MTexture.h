#pragma once

#include "../../RenderDataHandler.h"
#include "TextureConverters.h"
#include "TextureSettings.h"
#include "Texture2DInitialization.h"
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
    
    //Returns whether the texture was created successfully.
    bool Create(const ColorTextureSettings & settings, Texture2DInitialization & texInitializer);

    //If this is a valid texture, deletes it from OpenGL.
    void DeleteIfValid(void);

    //Sets this texture as the active one.
    //If this isn't a valid texture, then the currently-active texture is just deactivated.
    void Bind(void) const { RenderDataHandler::BindTexture(TextureTypes::TT_2D, texHandle); }

    //Returns whether or not the texture setting was successful.
    bool SetData(Texture2DInitialization & init);
    //Returns whether or not the texture setting was successful.
    //Note that the given width/height may be overridden by certain sub-types of Texture2DInitialization.
    bool SetData(unsigned int newWidth, unsigned int newHeight, Texture2DInitialization & init);


private:

    RenderObjHandle texHandle;
    ColorTextureSettings settings;
};