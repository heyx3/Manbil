#pragma once

#include "../../RenderDataHandler.h"
#include "TextureConverters.h"
#include <SFML/Graphics/Texture.hpp>


//Handles texture settings.
struct TextureSettings
{
public:

    //The types of texture filtering.
    enum FilteringTypes
    {
        MTF_NEAREST,
        MTF_LINEAR,
    };

    //The types of texture wrapping.
    enum WrappingTypes
    {
        MTF_CLAMP,
        MTF_WRAP,
    };


    FilteringTypes MinFilter = FilteringTypes::MTF_NEAREST,
                   MagFilter = FilteringTypes::MTF_NEAREST;
    WrappingTypes HorzWrap = WrappingTypes::MTF_WRAP,
                  VertWrap = WrappingTypes::MTF_WRAP;


    //Sets the currently-bound texture to use this setting's min filter.
    void ApplyMinFilter(bool usesMipmaps) const
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ToGLInt(MinFilter, true, usesMipmaps));
    }
    //Sets the currently-bound texture to use this setting's mag filter.
    void ApplyMagFilter(bool usesMipmaps) const
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ToGLInt(MagFilter, false, usesMipmaps));
    }
    //Sets the currently-bound texture to use this setting's min and mag filters.
    void ApplyFilter(bool usesMipmaps) const
    {
        ApplyMinFilter(usesMipmaps);
        ApplyMagFilter(usesMipmaps);
    }

    //Sets the currently-bound texture to use this setting's horizontal wrapping behavior.
    void ApplyHorzWrapping(void) const
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ToGLInt(HorzWrap));
    }
    //Sets the currently-bound texture to use this setting's vertical wrapping behavior.
    void ApplyVertWrapping(void) const
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ToGLInt(VertWrap));
    }
    //Sets the currently-bound texture to use this setting's horizontal/vertical wrapping behavior.
    void ApplyWrapping(void) const
    {
        ApplyHorzWrapping();
        ApplyVertWrapping();
    }

    //Applies all this instance's settings to the currently-bound texture.
    void ApplyAllSettings(bool usesMipmaps) const
    {
        ApplyFilter(usesMipmaps);
        ApplyWrapping();
    }


private:


    static GLint ToGLInt(FilteringTypes tf, bool minFilter, bool genMips)
    {
        return (tf == FilteringTypes::MTF_LINEAR) ?
                    ((minFilter && genMips) ?
                        GL_LINEAR_MIPMAP_LINEAR :
                        GL_LINEAR) :
                    GL_NEAREST;
    }
    static GLint ToGLInt(WrappingTypes twa)
    {
        return (twa == WrappingTypes::MTF_CLAMP) ?        
                    GL_CLAMP_TO_EDGE :
                    GL_REPEAT;
    }
};



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

    void SetSettings(const TextureSettings & newSettings)
    {
        settings = newSettings;

        if (IsValidTexture())
        {
            Bind();
            settings.ApplyAllSettings(hasMipmaps);
        }
    }

    void SetMinFilterType(TextureSettings::FilteringTypes newFiltering)
    {
        settings.MinFilter = newFiltering;

        if (IsValidTexture())
        {
            Bind();
            settings.ApplyMinFilter(hasMipmaps);
        }
    }
    void SetMagFilterType(TextureSettings::FilteringTypes newFiltering)
    {
        settings.MagFilter = newFiltering;

        if (IsValidTexture())
        {
            Bind();
            settings.ApplyMagFilter(hasMipmaps);
        }
    }
    void SetFilterType(TextureSettings::FilteringTypes newFiltering)
    {
        settings.MinFilter = newFiltering;
        settings.MagFilter = newFiltering;

        if (IsValidTexture())
        {
            Bind();
            settings.ApplyFilter(hasMipmaps);
        }
    }

    void SetHorzWrappingType(TextureSettings::WrappingTypes wrapping)
    {
        settings.HorzWrap = wrapping;

        if (IsValidTexture())
        {
            Bind();
            settings.ApplyHorzWrapping();
        }
    }
    void SetVertWrappingType(TextureSettings::WrappingTypes wrapping)
    {
        settings.VertWrap = wrapping;

        if (IsValidTexture())
        {
            Bind();
            settings.ApplyVertWrapping();
        }
    }
    void SetWrappingType(TextureSettings::WrappingTypes wrapping)
    {
        settings.HorzWrap = wrapping;
        settings.VertWrap = wrapping;

        if (IsValidTexture())
        {
            Bind();
            settings.ApplyWrapping();
        }
    }

    //TODO: Whenever texture data is changed, mipmaps are immediately regenerated if necessary. Double-check that this is necessary and good design.
    void GenerateMipmaps(void)
    {
        if (IsValidTexture())
        {
            hasMipmaps = true;
            Bind();
            glGenerateMipmap(GL_TEXTURE_2D);
            settings.ApplyFilter(true);
        }
    }


    //Texture operations.
    
    //Creates an empty texture. Deletes the texture this instance previously held.
    void Create(void)
    {
        DeleteIfValid();
        RenderDataHandler::CreateTexture2D(texHandle);
        tWidth = 0;
        tHeight = 0;

        Bind();
        settings.ApplyAllSettings(false);
    }
    //Returns whether the file was successfully loaded. Deletes the texture this instance previously held.
    bool Create(std::string filePath)
    {
        //Load the file using SFML.
        sf::Texture tex;
        if (!tex.loadFromFile(filePath))
        {
            return false;
        }

        //Copy the texture data into this texture.
        Create();
        Array2D<Vector4f> texCol(0, 0);
        TextureConverters::ToArray(tex, texCol);
        SetData(texCol);
    }

    //If this is a valid texture, deletes it from OpenGL.
    //Also resets the mipmap settings to "no mipmaps".
    void DeleteIfValid(void)
    {
        if (IsValidTexture())
        {
            RenderDataHandler::DeleteTexture2D(texHandle);
        }
        texHandle = 0;
        hasMipmaps = false;
    }

    //Sets this texture as the active one.
    //If this isn't a valid texture, then the currently-active texture is just deactivated.
    void Bind(void) const { RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, texHandle); }

    void SetData(const Array2D<Vector4b> & inColor)
    {
        tWidth = inColor.GetWidth();
        tHeight = inColor.GetHeight();
        TextureConverters::ToTexture(inColor, texHandle);
        if (hasMipmaps) GenerateMipmaps();
    }
    void SetData(const Array2D<Vector4f> & inColor)
    {
        tWidth = inColor.GetWidth();
        tHeight = inColor.GetHeight();
        TextureConverters::ToTexture(inColor, texHandle);
        if (hasMipmaps) GenerateMipmaps();
    }
    void SetData(const Vector4b color, unsigned int width, unsigned int height)
    {
        tWidth = width;
        tHeight = height;
        RenderDataHandler::SetTexture2DDataColor(texHandle, Vector2i((int)tWidth, (int)tHeight), color);
        if (hasMipmaps) GenerateMipmaps();
    }
    void SetData(const Vector4f color, unsigned int width, unsigned int height)
    {
        tWidth = width;
        tHeight = height;
        RenderDataHandler::SetTexture2DDataColor(texHandle, Vector2i((int)tWidth, (int)tHeight), color);
        if (hasMipmaps) GenerateMipmaps();
    }


private:

    RenderObjHandle texHandle;

    bool hasMipmaps;

    TextureSettings settings;

    unsigned int tWidth, tHeight;
};