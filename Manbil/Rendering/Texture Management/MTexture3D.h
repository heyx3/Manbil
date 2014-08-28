#pragma once

#include "../../RenderDataHandler.h"
#include "TextureSettings.h"


//Represents a 3D texture.
//TODO: Whenever texture data is changed, mipmaps are immediately regenerated if enabled. Double-check that this is necessary and good design.
class MTexture3D : public ISerializable
{
public:

    //Gets the currently-bound 3D texture. Returns 0 if no texture is currently-bound.
    static const MTexture3D * CurrentlyBound(void) { return currentBound; }


    //Constructors/destructors.

    MTexture3D(const TextureSampleSettings3D & _settings, PixelSizes _pixelSize, bool useMipmapping)
        : texHandle(0), width(0), height(0), depth(0), settings(_settings), pixelSize(_pixelSize), usesMipmaps(useMipmapping)
    {

    }
    ~MTexture3D(void) { DeleteIfValid(); }

    MTexture3D(MTexture3D & cpy) = delete;


    virtual bool WriteData(DataWriter * writer, std::string & outError) const override;
    virtual bool ReadData(DataReader * reader, std::string & outError) override;


    //Getters.

    RenderObjHandle GetTextureHandle(void) const { return texHandle; }
    bool IsValidTexture(void) const { return texHandle != 0; }

    unsigned int GetWidth(void) const { return width; }
    unsigned int GetHeight(void) const { return height; }
    unsigned int GetDepth(void) const { return depth; }
    const TextureSampleSettings3D & GetSamplingSettings(void) const { return settings; }
    bool UsesMipmaps(void) const { return usesMipmaps; }
    PixelSizes GetPixelSize(void) const { return pixelSize; }

    bool IsColorTexture(void) const { return IsPixelSizeColor(pixelSize); }
    bool IsGreyscaleTexture(void) const { return IsPixelSizeGreyscale(pixelSize); }
    bool IsDepthTexture(void) const { return IsPixelSizeDepth(pixelSize); }


    //Setters.

    void SetSettings(const TextureSampleSettings3D & newSettings);

    void SetMinFilterType(FilteringTypes newFiltering);
    void SetMagFilterType(FilteringTypes newFiltering);
    void SetFilterType(FilteringTypes newFiltering);

    void SetXWrappingType(WrappingTypes wrapping);
    void SetYWrappingType(WrappingTypes wrapping);
    void SetZWrappingType(WrappingTypes wrapping);
    void SetWrappingType(WrappingTypes wrapping);


    //Texture operations. All operations other than "Create" and "Bind" fail if this is not a valid texture.

    //Creates a new texture with no data.
    //Deletes the previous texture held by this instance if one existed.
    void Create(void) { Create(settings, usesMipmaps, pixelSize); }
    //Creates a new texture with no data.
    //Deletes the previous texture held by this instance if one existed.
    void Create(const TextureSampleSettings3D & sampleSettings, bool useMipmaps, PixelSizes pixelSize);

    //If this is a valid texture, deletes it from OpenGL.
    //Returns whether anything needed to be deleted.
    bool DeleteIfValid(void);

    //Clears this texture's data.
    void ClearData(unsigned int newW = 0, unsigned int newH = 0, unsigned int newD = 0);


    //Sets this texture as the active one.
    //If this isn't a valid texture, then the currently-active texture is just deactivated.
    void Bind(void) const
    {
        if (currentBound != this)
        {
            currentBound = (texHandle == 0) ? 0 : this;
            glBindTexture(GL_TEXTURE_3D, texHandle);
        }
    }


    //This operation only succeeds if this texture's pixel type is not a depth or greyscale type.
    //If a non-color type pixel size is passed in, the current pixel size is not changed.
    //Returns whether the operation succeeded.
    bool SetColorData(const Array3D<Vector4b> & pixelData, PixelSizes newSize = PixelSizes::PS_16U_DEPTH);
    //This operation only succeeds if this texture's pixel type is not a depth or greyscale type.
    //If a non-color pixel size is passed in, the current pixel size is not changed.
    //Returns whether the operation succeeded.
    bool SetColorData(const Array3D<Vector4f> & pixelData, PixelSizes newSize = PixelSizes::PS_16U_DEPTH);
    //Updates this texture's color data without having to allocate new space.
    //This operation fails if any part of the pixel array is outside the texture bounds.
    //Returns whether the operation succeeded.
    bool UpdateColorData(const Array3D<Vector4b> & pixelData, unsigned int offsetX = 0, unsigned int offsetY = 0, unsigned int offsetZ = 0);
    //Updates this texture's color data without having to allocate new space.
    //This operation fails if any part of the pixel array is outside the texture bounds.
    //Returns whether the operation succeeded.
    bool UpdateColorData(const Array3D<Vector4f> & pixelData, unsigned int offsetX = 0, unsigned int offsetY = 0, unsigned int offsetZ = 0);

    //This operation only succeeds if this texture's pixel type is a greyscale type.
    //If a non-greyscale type pixel size is passed in, the current pixel size is not changed.
    //Returns whether the operation succeeded.
    bool SetGreyscaleData(const Array3D<unsigned char> & greyscaleData, PixelSizes newSize = PixelSizes::PS_16U_DEPTH);
    //This operation only succeeds if this texture's pixel type is a greyscale type.
    //If a non-greyscale pixel size is passed in, the current pixel size is not changed.
    //Returns whether the operation succeeded.
    bool SetGreyscaleData(const Array3D<float> & greyscaleData, PixelSizes newSize = PixelSizes::PS_16U_DEPTH);
    //Updates this texture's greyscale data without having to allocate new space.
    //This operation fails if any part of the pixel array is outside the texture bounds.
    //Returns whether the operation succeeded.
    bool UpdateGreyscaleData(const Array3D<unsigned char> & pixelData, unsigned int offsetX = 0, unsigned int offsetY = 0, unsigned int offsetZ = 0);
    //Updates this texture's greyscale data without having to allocate new space.
    //This operation fails if any part of the pixel array is outside the texture bounds.
    //Returns whether the operation succeeded.
    bool UpdateGreyscaleData(const Array3D<float> & pixelData, unsigned int offsetX = 0, unsigned int offsetY = 0, unsigned int offsetZ = 0);


    //This operation only succeeds if this texture's pixel type is a depth type.
    //If a non-"depth" pixel size is passed in, the current pixel size is not changed.
    //Returns whether the operation succeeded.
    bool SetDepthData(const Array3D<unsigned char> & depthData, PixelSizes newSize = PixelSizes::PS_8U);
    //This operation only succeeds if this texture's pixel type is a depth type.
    //If a non-"depth" pixel size is passed in, the current pixel size is not changed.
    //Returns whether the operation succeeded.
    bool SetDepthData(const Array3D<float> & depthData, PixelSizes newSize = PixelSizes::PS_8U);
    //Updates this texture's depth data without having to allocate new space.
    //This operation fails if this is not a depth texture.
    //This operation fails if any part of the pixel array is outside the texture bounds.
    //Returns whether the operation succeeded.
    bool UpdateDepthData(const Array3D<unsigned char> & pixelData, unsigned int offsetX = 0, unsigned int offsetY = 0, unsigned int offsetZ = 0);
    //Updates this texture's depth data without having to allocate new space.
    //This operation fails if this is not a depth texture.
    //This operation fails if any part of the pixel array is outside the texture bounds.
    //Returns whether the operation succeeded.
    bool UpdateDepthData(const Array3D<float> & pixelData, unsigned int offsetX = 0, unsigned int offsetY = 0, unsigned int offsetZ = 0);


    //Copies this texture's color data from the graphics card into the given array.
    //This operation fails if this is not a color texture.
    //If the out array extends beyond the texture bounds, this function fails.
    //Returns whether the operation succeeded.
    bool GetColorData(Array3D<Vector4b> & outData) const;
    //Copies this texture's color data from the graphics card into the given array.
    //This operation fails if this is not a color texture.
    //If the out array extends beyond the texture bounds, this function fails.
    //Returns whether the operation succeeded.
    bool GetColorData(Array3D<Vector4f> & outData) const;

    //Copies this texture's color data from the graphics card into the given array.
    //This operation fails if this is not a greyscale texture.
    //If the out array extends beyond the texture bounds, this function fails.
    //Returns whether the operation succeeded.
    bool GetGreyscaleData(Array3D<unsigned char> & outData) const;
    //Copies this texture's color data from the graphics card into the given array.
    //This operation fails if this is not a greyscale texture.
    //If the out array extends beyond the texture bounds, this function fails.
    //Returns whether the operation succeeded.
    bool GetGreyscaleData(Array3D<float> & outData) const;

    //Copies this texture's depth data from the graphics card into the given array.
    //This operation fails if this is not a depth texture.
    //If the out array extends beyond the texture bounds, this function fails.
    //Returns whether the operation succeeded.
    bool GetDepthData(Array3D<unsigned char> & outData) const;
    //Copies this texture's depth data from the graphics card into the given array.
    //This operation fails if this is not a depth texture.
    //If the out array extends beyond the texture bounds, this function fails.
    //Returns whether the operation succeeded.
    bool GetDepthData(Array3D<float> & outData) const;


private:

    RenderObjHandle texHandle;

    unsigned int width, height, depth;

    TextureSampleSettings3D settings;
    PixelSizes pixelSize;
    bool usesMipmaps;

    //Gets the texture format for pixel data based on this texture's pixel size.
    GLenum GetInternalFormat(void) const { return ToGLenum(pixelSize); }
    //Gets the input format for pixel data based on this texture's pixel size.
    GLenum GetCPUFormat(void) const;
    //Gets the input type (float, byte, etc.) based on this texture's pixel size.
    GLenum GetComponentType(void) const;


    static const MTexture3D * currentBound;
};