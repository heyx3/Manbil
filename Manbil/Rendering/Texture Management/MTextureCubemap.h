#pragma once

#include "../../RenderDataHandler.h"
#include "TextureConverters.h"
#include "TextureSettings.h"
#include "Texture2DInitialization.h"
#include <SFML/Graphics/Texture.hpp>


//Represents a cubemap texture.
class MTextureCubemap
{
public:

    //Constructors/destructors.

    MTextureCubemap(void) : texHandle(0) { }
    MTextureCubemap(MTextureCubemap & cpy); //Intentionally not implemented.


    //Getters.

    RenderObjHandle GetTextureHandle(void) const { return texHandle; }
    bool IsValidTexture(void) const { return texHandle != 0; }

    const ColorTextureSettings & GetSettings(CubeTextureTypes face) const { return settings[GetIndex(face)]; }


    //Setters.

    void SetSettings(CubeTextureTypes face, const TextureSettings & newSettings);

    void SetMinFilterType(CubeTextureTypes face, TextureSettings::FilteringTypes newFiltering);
    void SetMagFilterType(CubeTextureTypes face, TextureSettings::FilteringTypes newFiltering);
    void SetFilterType(CubeTextureTypes face, TextureSettings::FilteringTypes newFiltering);

    void SetHorzWrappingType(CubeTextureTypes face, TextureSettings::WrappingTypes wrapping);
    void SetVertWrappingType(CubeTextureTypes face, TextureSettings::WrappingTypes wrapping);
    void SetWrappingType(CubeTextureTypes face, TextureSettings::WrappingTypes wrapping);


    //Texture operations.

    //Returns whether the creation was successful.
    bool Create(const ColorTextureSettings & settingsNegX, Texture2DInitialization & initNegX,
                const ColorTextureSettings & settingsNegY, Texture2DInitialization & initNegY,
                const ColorTextureSettings & settingsNegZ, Texture2DInitialization & initNegZ,
                const ColorTextureSettings & settingsPosX, Texture2DInitialization & initPosX,
                const ColorTextureSettings & settingsPosY, Texture2DInitialization & initPosY,
                const ColorTextureSettings & settingsPosZ, Texture2DInitialization & initPosZ);

    void DeleteIfValid(void);

    void Bind(void) const { RenderDataHandler::BindTexture(TextureTypes::TT_CUBE, texHandle); }

    //Returns whether this attempt at setting a face's data was successful.
    bool SetFaceData(CubeTextureTypes face, const ColorTextureSettings & newSettings, Texture2DInitialization & init);



private:

    RenderObjHandle texHandle;
    ColorTextureSettings settings[6];

    static const unsigned int NEG_X, NEG_Y, NEG_Z, POS_X, POS_Y, POS_Z;
    static unsigned int GetIndex(CubeTextureTypes faceType);
};