#pragma once

#include "../../RenderDataHandler.h"
#include "TextureSettings.h"


//Represents a cubemap texture.
//TODO: Whenever texture data is changed, mipmaps are immediately regenerated if enabled. Double-check that this is necessary and good design.
class MTextureCubemap
{
public:

    //Constructors/destructors.

    //The pixel size must not be a "depth" type!
    MTextureCubemap(const TextureSampleSettings & _settings, PixelSizes _pixelSize, bool useMipmapping)
        : texHandle(0), width(0), height(0), settings(_settings), pixelSize(_pixelSize), usesMipmaps(useMipmapping)
    {
        assert(!IsPixelSizeDepth(_pixelSize));
    }
    MTextureCubemap(MTextureCubemap & cpy); //Intentionally not implemented.


    //Getters.

    RenderObjHandle GetTextureHandle(void) const { return texHandle; }
    bool IsValidTexture(void) const { return texHandle != 0; }

    unsigned int GetWidth(void) const { return width; }
    unsigned int GetHeight(void) const { return height; }
    const TextureSampleSettings & GetSamplingSettings(void) const { return settings; }
    bool UsesMipmaps(void) const { return usesMipmaps; }
    PixelSizes GetPixelSize(void) const { return pixelSize; }

    bool IsGreyscale(void) const { return IsPixelSizeGreyscale(pixelSize); }


    //Setters.

    void SetSettings(const TextureSampleSettings & newSettings);

    void SetMinFilterType(TextureSampleSettings::FilteringTypes newFiltering);
    void SetMagFilterType(TextureSampleSettings::FilteringTypes newFiltering);
    void SetFilterType(TextureSampleSettings::FilteringTypes newFiltering);

    void SetHorzWrappingType(TextureSampleSettings::WrappingTypes wrapping);
    void SetVertWrappingType(TextureSampleSettings::WrappingTypes wrapping);
    void SetWrappingType(TextureSampleSettings::WrappingTypes wrapping);


    //Texture operations. All operations other than "Create" and "Bind" cause an OpenGL error if this is not a valid texture.

    //Creates a new texture with no data.
    //Deletes the previous texture held by this instance if one existed.
    void Create(const TextureSampleSettings & sampleSettings, bool useMipmaps, PixelSizes pixelSize);

    //If this is a valid texture, deletes it from OpenGL.
    //Returns whether anything needed to be deleted.
    bool DeleteIfValid(void);


    //Sets this cubemap texture as the active one.
    //If this isn't a valid texture, then the currently-active cubemap texture is just deactivated.
    void Bind(void) const { glBindTexture(GL_TEXTURE_CUBE_MAP, texHandle); }


    //Clears all faces' data.
    void ClearData(unsigned int newW, unsigned int newH);


    //Sets the given face's data from the given file.
    //"shouldUpdateMipmaps" is only applicable if this texture uses mipmaps.
    bool SetDataFromFile(CubeTextureTypes face, std::string filePath, bool shouldUpdateMipmaps);

    //If a "depth" pixel size is passed in, the current pixel size is used.
    //This operation fails if the faces aren't all the same width/height.
    //Returns whether this operation succeeded.
    bool SetData(Array2D<Vector4b> & negXData, Array2D<Vector4b> & negYData, Array2D<Vector4b> & negZData,
                 Array2D<Vector4b> & posXData, Array2D<Vector4b> & posYData, Array2D<Vector4b> & posZData,
                 bool useMipmaps, PixelSizes pixelSize = PS_16U_DEPTH);
    //If a "depth" pixel size is passed in, the current pixel size is used.
    //This operation fails if the faces aren't all the same width/height.
    //Returns whether this operation succeeded.
    bool SetData(Array2D<Vector4f> & negXData, Array2D<Vector4f> & negYData, Array2D<Vector4f> & negZData,
                 Array2D<Vector4f> & posXData, Array2D<Vector4f> & posYData, Array2D<Vector4f> & posZData,
                 bool useMipmaps, PixelSizes pixelSize = PS_16U_DEPTH);

    //If a "depth" pixel size is passed in, the current pixel size is used.
    //This operation fails if the given array's size is not the same as the rest of the cubemap faces.
    //Returns whether the operation succeeded.
    //"shouldUpdateMipmaps" is only applicable if this texture uses mipmaps.
    bool SetData(CubeTextureTypes face, const Array2D<Vector4b> & pixelData, bool shouldUpdateMipmaps);
    //If a "depth" pixel size is passed in, the current pixel size is used.
    //This operation fails if the given array's size is not the same as the rest of the cubemap faces.
    //Returns whether the operation succeeded.
    //"shouldUpdateMipmaps" is only applicable if this texture uses mipmaps.
    bool SetData(CubeTextureTypes face, const Array2D<Vector4f> & pixelData, bool shouldUpdateMipmaps);

    //Copies this texture's color data from the graphics card into the given array.
    //Assumes the given array is already the right size.
    void GetData(CubeTextureTypes face, Array2D<Vector4b> & outData);
    //Copies this texture's color data from the graphics card into the given array.
    //Assumes the given array is already the right size.
    void GetData(CubeTextureTypes face, Array2D<Vector4f> & outData);


private:

    RenderObjHandle texHandle;

    unsigned int width, height;

    TextureSampleSettings settings;
    PixelSizes pixelSize;
    bool usesMipmaps;
};