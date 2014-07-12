#pragma once

#include "../../RenderDataHandler.h"
#include "TextureSettings.h"


//Represents a texture.
//TODO: Whenever texture data is changed, mipmaps are immediately regenerated if enabled. Double-check that this is necessary and good design.
class MTexture
{
public:

    //Loads the file at the given path into the given array.
    //Automatically resizes the array to fit the image size.
    static bool LoadImageFromFile(std::string filePath, Array2D<Vector4b> & outData);


    //Constructors/destructors.

    MTexture(const TextureSampleSettings & _settings, PixelSizes _pixelSize, bool useMipmapping)
        : texHandle(0), width(0), height(0), settings(_settings), pixelSize(_pixelSize), usesMipmaps(useMipmapping)
    {

    }
    MTexture(MTexture & cpy); //Intentionally not implemented.


    //Getters.

    RenderObjHandle GetTextureHandle(void) const { return texHandle; }
    bool IsValidTexture(void) const { return texHandle != 0; }

    unsigned int GetWidth(void) const { return width; }
    unsigned int GetHeight(void) const { return height; }
    const TextureSampleSettings & GetSamplingSettings(void) const { return settings; }
    bool UsesMipmaps(void) const { return usesMipmaps; }
    PixelSizes GetPixelSize(void) const { return pixelSize; }

    bool IsGreyscale(void) const { return IsPixelSizeGreyscale(pixelSize); }
    bool IsDepthTexture(void) const { return IsPixelSizeDepth(pixelSize); }


    //Setters.

    void SetSettings(const TextureSampleSettings & newSettings);

    void SetMinFilterType(TextureSampleSettings::FilteringTypes newFiltering);
    void SetMagFilterType(TextureSampleSettings::FilteringTypes newFiltering);
    void SetFilterType(TextureSampleSettings::FilteringTypes newFiltering);

    void SetHorzWrappingType(TextureSampleSettings::WrappingTypes wrapping);
    void SetVertWrappingType(TextureSampleSettings::WrappingTypes wrapping);
    void SetWrappingType(TextureSampleSettings::WrappingTypes wrapping);


    //Texture operations. All operations other than "Create" and "Bind" fail if this is not a valid texture.

    //Creates a new texture with no data.
    //Deletes the previous texture held by this instance if one existed.
    void Create(const TextureSampleSettings & sampleSettings, bool useMipmaps, PixelSizes pixelSize);

    //If this is a valid texture, deletes it from OpenGL.
    //Returns whether anything needed to be deleted.
    bool DeleteIfValid(void);

    //Clears this texture's data.
    void ClearData(unsigned int newW = 0, unsigned int newH = 0);


    //Sets this texture's data from the given file.
    bool SetDataFromFile(std::string filePath, PixelSizes newSize);

    //This operation only succeeds if this texture's pixel type is not a depth type.
    //If a "depth" type pixel size is passed in, the current pixel size is not changed.
    //Returns whether the operation succeeded.
    bool SetData(const Array2D<Vector4b> & pixelData, PixelSizes newSize = PixelSizes::PS_16U_DEPTH);
    //This operation only succeeds if this texture's pixel type is not a depth type.
    //If a "depth" pixel size is passed in, the current pixel size is not changed.
    //Returns whether the operation succeeded.
    bool SetData(const Array2D<Vector4f> & pixelData, PixelSizes newSize = PixelSizes::PS_16U_DEPTH);

    //This operation only succeeds if this texture's pixel type is a depth type.
    //If a non-"depth" pixel size is passed in, the current pixel size is not changed.
    //Returns whether the operation succeeded.
    bool SetDepthData(const Array2D<unsigned char> & depthData, PixelSizes newSize = PixelSizes::PS_8U);
    //This operation only succeeds if this texture's pixel type is a depth type.
    //If a non-"depth" pixel size is passed in, the current pixel size is not changed.
    //Returns whether the operation succeeded.
    bool SetDepthData(const Array2D<float> & depthData, PixelSizes newSize = PixelSizes::PS_8U);

    //Sets this texture as the active one.
    //If this isn't a valid texture, then the currently-active texture is just deactivated.
    void Bind(void) const { glBindTexture(GL_TEXTURE_2D, texHandle); }

    //Copies this texture's color data from the graphics card into the given array.
    //This operation fails if this is a depth texture.
    //Assumes the given array is already the right size.
    //Returns whether the operation succeeded.
    bool GetData(Array2D<Vector4b> & outData);
    //Copies this texture's color data from the graphics card into the given array.
    //This operation fails if this is a depth texture.
    //Assumes the given array is already the right size.
    //Returns whether the operation succeeded.
    bool GetData(Array2D<Vector4f> & outData);

    //Copies this texture's depth data from the graphics card into the given array.
    //This operation fails if this isn't a depth texture.
    //Assumes the given array is already the right size.
    //Returns whether the operation succeeded.
    bool GetDepthData(Array2D<unsigned char> & outData);
    //Copies this texture's depth data from the graphics card into the given array.
    //This operation fails if this isn't a depth texture.
    //Assumes the given array is already the right size.
    //Returns whether the operation succeeded.
    bool GetDepthData(Array2D<float> & outData);


private:

    RenderObjHandle texHandle;

    unsigned int width, height;

    TextureSampleSettings settings;
    PixelSizes pixelSize;
    bool usesMipmaps;
};