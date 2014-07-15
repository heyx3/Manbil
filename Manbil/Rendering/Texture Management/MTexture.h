#pragma once

#include "../../RenderDataHandler.h"
#include "TextureSettings.h"


//Represents a texture.
//TODO: Whenever texture data is changed, mipmaps are immediately regenerated if enabled. Double-check that this is necessary and good design.
class MTexture
{
public:

    //Gets the currently-bound texture. Returns 0 if no texture is currently-bound.
    static const MTexture * CurrentlyBound(void) { return currentBound; }

    //Loads the file at the given path into the given array.
    //Automatically resizes the array to fit the image size.
    static bool LoadImageFromFile(std::string filePath, Array2D<Vector4b> & outData);


    //Constructors/destructors.

    MTexture(const TextureSampleSettings & _settings, PixelSizes _pixelSize, bool useMipmapping)
        : texHandle(0), width(0), height(0), settings(_settings), pixelSize(_pixelSize), usesMipmaps(useMipmapping)
    {

    }
    ~MTexture(void) { DeleteIfValid(); }

    MTexture(MTexture & cpy); //Intentionally not implemented.


    //Getters.

    RenderObjHandle GetTextureHandle(void) const { return texHandle; }
    bool IsValidTexture(void) const { return texHandle != 0; }

    unsigned int GetWidth(void) const { return width; }
    unsigned int GetHeight(void) const { return height; }
    const TextureSampleSettings & GetSamplingSettings(void) const { return settings; }
    bool UsesMipmaps(void) const { return usesMipmaps; }
    PixelSizes GetPixelSize(void) const { return pixelSize; }

    bool IsColorTexture(void) const { return IsPixelSizeColor(pixelSize); }
    bool IsGreyscaleTexture(void) const { return IsPixelSizeGreyscale(pixelSize); }
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
    void Create(void) { Create(settings, usesMipmaps, pixelSize); }
    //Creates a new texture with no data.
    //Deletes the previous texture held by this instance if one existed.
    void Create(const TextureSampleSettings & sampleSettings, bool useMipmaps, PixelSizes pixelSize);

    //If this is a valid texture, deletes it from OpenGL.
    //Returns whether anything needed to be deleted.
    bool DeleteIfValid(void);

    //Clears this texture's data.
    void ClearData(unsigned int newW = 0, unsigned int newH = 0);


    //Sets this texture as the active one.
    //If this isn't a valid texture, then the currently-active texture is just deactivated.
    void Bind(void) const
    {
        if (currentBound != this)
        {
            currentBound = (texHandle == 0) ? 0 : this;
            glBindTexture(GL_TEXTURE_2D, texHandle);
        }
    }


    //Sets this texture's data from the given file.
    //This texture must currently have a color pixel size (not depth or greyscale).
    //Outputs an error message and returns false if something failed.
    bool SetDataFromFile(std::string filePath, std::string & outError) { return SetDataFromFile(filePath, pixelSize, outError); }
    //Sets this texture's data from the given file.
    //The given new pixel size must be a color size (not depth or greyscale).
    //Outputs an error message and returns false if something failed.
    bool SetDataFromFile(std::string filePath, PixelSizes newSize, std::string & outError);

    //This operation only succeeds if this texture's pixel type is not a depth or greyscale type.
    //If a non-color type pixel size is passed in, the current pixel size is not changed.
    //Returns whether the operation succeeded.
    bool SetColorData(const Array2D<Vector4b> & pixelData, PixelSizes newSize = PixelSizes::PS_16U_DEPTH);
    //This operation only succeeds if this texture's pixel type is not a depth or greyscale type.
    //If a non-color pixel size is passed in, the current pixel size is not changed.
    //Returns whether the operation succeeded.
    bool SetColorData(const Array2D<Vector4f> & pixelData, PixelSizes newSize = PixelSizes::PS_16U_DEPTH);
    //Updates this texture's color data without having to allocate new space.
    //This operation fails if any part of the pixel array is outside the texture bounds.
    //Returns whether the operation succeeded.
    bool UpdateColorData(const Array2D<Vector4b> & pixelData, unsigned int offsetX = 0, unsigned int offsetY = 0);
    //Updates this texture's color data without having to allocate new space.
    //This operation fails if any part of the pixel array is outside the texture bounds.
    //Returns whether the operation succeeded.
    bool UpdateColorData(const Array2D<Vector4f> & pixelData, unsigned int offsetX = 0, unsigned int offsetY = 0);

    //This operation only succeeds if this texture's pixel type is a greyscale type.
    //If a non-greyscale type pixel size is passed in, the current pixel size is not changed.
    //Returns whether the operation succeeded.
    bool SetGreyscaleData(const Array2D<unsigned char> & greyscaleData, PixelSizes newSize = PixelSizes::PS_16U_DEPTH);
    //This operation only succeeds if this texture's pixel type is a greyscale type.
    //If a non-greyscale pixel size is passed in, the current pixel size is not changed.
    //Returns whether the operation succeeded.
    bool SetGreyscaleData(const Array2D<float> & greyscaleData, PixelSizes newSize = PixelSizes::PS_16U_DEPTH);
    //Updates this texture's greyscale data without having to allocate new space.
    //This operation fails if any part of the pixel array is outside the texture bounds.
    //Returns whether the operation succeeded.
    bool UpdateGreyscaleData(const Array2D<unsigned char> & pixelData, unsigned int offsetX = 0, unsigned int offsetY = 0);
    //Updates this texture's greyscale data without having to allocate new space.
    //This operation fails if any part of the pixel array is outside the texture bounds.
    //Returns whether the operation succeeded.
    bool UpdateGreyscaleData(const Array2D<float> & pixelData, unsigned int offsetX = 0, unsigned int offsetY = 0);


    //This operation only succeeds if this texture's pixel type is a depth type.
    //If a non-"depth" pixel size is passed in, the current pixel size is not changed.
    //Returns whether the operation succeeded.
    bool SetDepthData(const Array2D<unsigned char> & depthData, PixelSizes newSize = PixelSizes::PS_8U);
    //This operation only succeeds if this texture's pixel type is a depth type.
    //If a non-"depth" pixel size is passed in, the current pixel size is not changed.
    //Returns whether the operation succeeded.
    bool SetDepthData(const Array2D<float> & depthData, PixelSizes newSize = PixelSizes::PS_8U);
    //Updates this texture's depth data without having to allocate new space.
    //This operation fails if this is not a depth texture.
    //This operation fails if any part of the pixel array is outside the texture bounds.
    //Returns whether the operation succeeded.
    bool UpdateDepthData(const Array2D<unsigned char> & pixelData, unsigned int offsetX = 0, unsigned int offsetY = 0);
    //Updates this texture's depth data without having to allocate new space.
    //This operation fails if this is not a depth texture.
    //This operation fails if any part of the pixel array is outside the texture bounds.
    //Returns whether the operation succeeded.
    bool UpdateDepthData(const Array2D<float> & pixelData, unsigned int offsetX = 0, unsigned int offsetY = 0);


    //Copies this texture's color data from the graphics card into the given array.
    //This operation fails if this is not a color texture.
    //If the out array extends beyond the texture bounds, this function fails.
    //Returns whether the operation succeeded.
    bool GetColorData(Array2D<Vector4b> & outData);
    //Copies this texture's color data from the graphics card into the given array.
    //This operation fails if this is not a color texture.
    //If the out array extends beyond the texture bounds, this function fails.
    //Returns whether the operation succeeded.
    bool GetColorData(Array2D<Vector4f> & outData);

    //Copies this texture's color data from the graphics card into the given array.
    //This operation fails if this is not a greyscale texture.
    //If the out array extends beyond the texture bounds, this function fails.
    //Returns whether the operation succeeded.
    bool GetGreyscaleData(Array2D<unsigned char> & outData);
    //Copies this texture's color data from the graphics card into the given array.
    //This operation fails if this is not a greyscale texture.
    //If the out array extends beyond the texture bounds, this function fails.
    //Returns whether the operation succeeded.
    bool GetGreyscaleData(Array2D<float> & outData);

    //Copies this texture's depth data from the graphics card into the given array.
    //This operation fails if this is not a depth texture.
    //If the out array extends beyond the texture bounds, this function fails.
    //Returns whether the operation succeeded.
    bool GetDepthData(Array2D<unsigned char> & outData);
    //Copies this texture's depth data from the graphics card into the given array.
    //This operation fails if this is not a depth texture.
    //If the out array extends beyond the texture bounds, this function fails.
    //Returns whether the operation succeeded.
    bool GetDepthData(Array2D<float> & outData);


private:

    RenderObjHandle texHandle;

    unsigned int width, height;

    TextureSampleSettings settings;
    PixelSizes pixelSize;
    bool usesMipmaps;

    //Gets the texture format for pixel data based on this texture's pixel size.
    GLenum GetInternalFormat(void) const { return ToGLenum(pixelSize); }
    //Gets the input format for pixel data based on this texture's pixel size.
    GLenum GetCPUFormat(void) const;
    //Gets the input type (float, byte, etc.) based on this texture's pixel size.
    GLenum GetComponentType(void) const;


    static const MTexture * currentBound;
};