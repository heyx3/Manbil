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

    MTexture(void) : texHandle(0), hasMipmaps(false), tWidth(0), tHeight(0) { }
    MTexture(MTexture & cpy); //Intentionally not implemented.
    MTexture(std::string filePath)
    {
        
    }



    //Getters.

    //Any non-zero value is considered valid. If 0 is returned, this texture is invalid.
    RenderObjHandle GetTextureHandle(void) const { return texHandle; }

    bool IsValidTexture(void) const { return texHandle != 0; }

    const TextureSettings & GetSettings(void) const { return settings; }
    bool UsesMipmaps(void) const { return hasMipmaps; }
    unsigned int GetWidth(void) const { return tWidth; }
    unsigned int GetHeight(void) const { return tHeight; }



    //Setters.

    void SetSettings(const TextureSettings & newSettings);

    void SetMinFilterType(TextureSettings::FilteringTypes newFiltering);
    void SetMagFilterType(TextureSettings::FilteringTypes newFiltering);
    void SetFilterType(TextureSettings::FilteringTypes newFiltering);

    void SetHorzWrappingType(TextureSettings::WrappingTypes wrapping);
    void SetVertWrappingType(TextureSettings::WrappingTypes wrapping);
    void SetWrappingType(TextureSettings::WrappingTypes wrapping);

    //Generates mipmaps. Every time texture data is changed after this call, mipmaps will be immediately re-generated.
    //TODO: Whenever texture data is changed, mipmaps are immediately regenerated if necessary. Double-check that this is necessary and good design.
    void GenerateMipmaps(void);


    //Texture operations.
    
    //Creates an empty texture. Deletes the texture this instance previously held.
    void Create(void);
    //Returns whether the file was successfully loaded. Deletes the texture this instance previously held.
    bool Create(std::string filePath);

    //If this is a valid texture, deletes it from OpenGL.
    //Also resets the mipmap settings to "no mipmaps".
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

    bool hasMipmaps;

    TextureSettings settings;

    unsigned int tWidth, tHeight;
};