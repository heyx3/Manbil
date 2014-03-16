#pragma once

#include <SFML/Graphics/Texture.hpp>
#include "../../OpenGLIncludes.h"
#include "../../RenderDataHandler.h"
#include "../../TextureSettings.h"



//A texture value that can be either an OpenGL texture or an SFML texture.
//If both the "SFML texture" and "OpenGL texture" values are set,
//    the value that takes priority is the OpenGL texture.
struct ManbilTexture
{
public:

    RenderObjHandle GLTex;
    sf::Texture * SFMLTex;

    //Creates an invalid ManbilTexture (i.e. doesn't have a texture).
    ManbilTexture(void) : SFMLTex(0), GLTex(0) { }
    ManbilTexture(RenderObjHandle glTex) : SFMLTex(0), GLTex(glTex) { }
    ManbilTexture(sf::Texture * sfmlTex) : SFMLTex(sfmlTex), GLTex(0) { }

    void SetData(RenderObjHandle glTex) { SFMLTex = 0; GLTex = glTex; }
    void SetData(sf::Texture * sfmlTex) { SFMLTex = sfmlTex; GLTex = 0; }

    //Returns whether or not either texture type is set to a valid texture.
    bool IsValidTexture(void) const { return SFMLTex != 0 || GLTex != 0; }

    bool UsesGLTex(void) const { return GLTex != 0; }
    bool UsesSFMLTex(void) const { return !UsesGLTex() && SFMLTex != 0; }


    //Binds this texture. Returns whether or not this texture is valid.
    bool BindTexture(void) const
    {
        if (UsesGLTex()) RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, GLTex);
        else if (UsesSFMLTex()) sf::Texture::bind(SFMLTex);
        else return false;
        return true;
    }

    //Deletes the texture this instance holds
    //   (if it happens to hold both an SFML tex and a GL tex,
    //    it will delete both of them).
    //Returns false if neither texture type is set to a valid texture.
    bool DeleteTexture(void);


    //Returns false if neither texture type is set to a valid texture.
    bool SetWrapping(TextureSettings::TextureWrapping wrap) const
    {
        if (!BindTexture()) return false;
        TextureSettings::SetWrappingData(wrap);
        return true;
    }
    //Returns false if neither texture type is set to a valid texture.
    bool SetWrapping(TextureSettings::TextureWrapping horizontal, TextureSettings::TextureWrapping vertical) const
    {
        if (!BindTexture()) return false;
        TextureSettings::SetWrappingData(horizontal, vertical);
        return true;
    }

    //Returns false if neither texture type is set to a valid texture.
    bool SetFiltering(TextureSettings::TextureFiltering filter, bool usesMipmaps) const
    {
        if (!BindTexture()) return false;
        TextureSettings::SetFilteringData(filter, usesMipmaps);
        return true;
    }
    //Returns false if neither texture type is set to a valid texture.
    bool SetFiltering(TextureSettings::TextureFiltering minFilter, TextureSettings::TextureFiltering magFilter, bool usesMipmaps) const
    {
        if (!BindTexture()) return false;
        TextureSettings::SetFilteringData(minFilter, magFilter, usesMipmaps);
        return true;
    }

    //Returns false if neither texture type is set to a valid texture.
    bool GenerateMipmaps(void)
    {
        if (!BindTexture()) return false;
        TextureSettings::GenMipmaps();
        return true;
    }
};