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

    const ColorTextureSettings & GetSettings(void) const { return settings; }


    //Setters.

    void SetSettings(const TextureSettings & newSettings);

    void SetMinFilterType(TextureSettings::FilteringTypes newFiltering);
    void SetMagFilterType(TextureSettings::FilteringTypes newFiltering);
    void SetFilterType(TextureSettings::FilteringTypes newFiltering);

    void SetHorzWrappingType(TextureSettings::WrappingTypes wrapping);
    void SetVertWrappingType(TextureSettings::WrappingTypes wrapping);
    void SetWrappingType(TextureSettings::WrappingTypes wrapping);


    //Texture operations.

    //All color arrays MUST be the exact size specified in "newSettings"!
    void Create(const ColorTextureSettings & newSettings,
                const Array2D<Vector4f> & dataNegX, const Array2D<Vector4f> & dataNegY, const Array2D<Vector4f> & dataNegZ,
                const Array2D<Vector4f> & dataPosX, const Array2D<Vector4f> & dataPosY, const Array2D<Vector4f> & dataPosZ);
    //All color arrays MUST be the exact size specified in "newSettings"!
    void Create(const ColorTextureSettings & newSettings,
                const Array2D<Vector4b> & dataNegX, const Array2D<Vector4b> & dataNegY, const Array2D<Vector4b> & dataNegZ,
                const Array2D<Vector4b> & dataPosX, const Array2D<Vector4b> & dataPosY, const Array2D<Vector4b> & dataPosZ);

    void DeleteIfValid(void);

    void Bind(void) const { RenderDataHandler::BindTexture(TextureTypes::TT_CUBE, texHandle); }

    //All color arrays MUST be the exact size specified in "newSettings"!
    void SetFaceData(const ColorTextureSettings & newSettings,
                     const Array2D<Vector4b> & dataNegX, const Array2D<Vector4b> & dataNegY, const Array2D<Vector4b> & dataNegZ,
                     const Array2D<Vector4b> & dataPosX, const Array2D<Vector4b> & dataPosY, const Array2D<Vector4b> & dataPosZ);
    //All color arrays MUST be the exact size specified in "newSettings"!
    void SetFaceData(const ColorTextureSettings & newSettings,
                     const Array2D<Vector4f> & dataNegX, const Array2D<Vector4f> & dataNegY, const Array2D<Vector4f> & dataNegZ,
                     const Array2D<Vector4f> & dataPosX, const Array2D<Vector4f> & dataPosY, const Array2D<Vector4f> & dataPosZ);
    //The size of "pixels" must be the same as the size of the rest of the cube!
    void SetFaceData(CubeTextureTypes face, const Array2D<Vector4f> & pixels);
    //The size of "pixels" must be the same as the size of the rest of the cube!
    void SetFaceData(CubeTextureTypes face, const Array2D<Vector4b> & pixels);

    //Copies this texture's pixel data from the graphics card into the given array.
    //Assumes the array is already the right size.
    void GetFaceData(CubeTextureTypes face, Array2D<Vector4b> & outData);
    //Copies this texture's pixel data from the graphics card into the given array.
    //Assumes the array is already the right size.
    void GetFaceData(CubeTextureTypes face, Array2D<Vector4f> & outData);


private:

    RenderObjHandle texHandle;
    ColorTextureSettings settings;
};