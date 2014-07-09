#pragma once

#include "../../RenderDataHandler.h"
#include "TextureSettings.h"
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

    void Create(const ColorTextureSettings & settingsNegX, const Array2D<Vector4f> & dataNegX,
                const ColorTextureSettings & settingsNegY, const Array2D<Vector4f> & dataNegY,
                const ColorTextureSettings & settingsNegZ, const Array2D<Vector4f> & dataNegZ,
                const ColorTextureSettings & settingsPosX, const Array2D<Vector4f> & dataPosX,
                const ColorTextureSettings & settingsPosY, const Array2D<Vector4f> & dataPosY,
                const ColorTextureSettings & settingsPosZ, const Array2D<Vector4f> & dataPosZ);
    void Create(const ColorTextureSettings & settingsNegX, const Array2D<Vector4b> & dataNegX,
                const ColorTextureSettings & settingsNegY, const Array2D<Vector4b> & dataNegY,
                const ColorTextureSettings & settingsNegZ, const Array2D<Vector4b> & dataNegZ,
                const ColorTextureSettings & settingsPosX, const Array2D<Vector4b> & dataPosX,
                const ColorTextureSettings & settingsPosY, const Array2D<Vector4b> & dataPosY,
                const ColorTextureSettings & settingsPosZ, const Array2D<Vector4b> & dataPosZ);

    void DeleteIfValid(void);

    void Bind(void) const { RenderDataHandler::BindTexture(TextureTypes::TT_CUBE, texHandle); }

    //Returns whether this attempt at setting a face's data was successful.
    void SetFaceData(CubeTextureTypes face, const ColorTextureSettings & newSettings, const Array2D<Vector4f> & pixels);
    //Returns whether this attempt at setting a face's data was successful.
    void SetFaceData(CubeTextureTypes face, const ColorTextureSettings & newSettings, const Array2D<Vector4b> & pixels);

    //Copies this texture's pixel data from the graphics card into the given array.
    //Automatically resizes the array to fit.
    void GetFaceData(CubeTextureTypes face, Array2D<Vector4b> & outData);
    //Copies this texture's pixel data from the graphics card into the given array.
    //Automatically resizes the array to fit.
    void GetFaceData(CubeTextureTypes face, Array2D<Vector4f> & outData);


private:

    RenderObjHandle texHandle;
    ColorTextureSettings settings[6];

    static const unsigned int NEG_X, NEG_Y, NEG_Z, POS_X, POS_Y, POS_Z;
    static unsigned int GetIndex(CubeTextureTypes faceType);
};