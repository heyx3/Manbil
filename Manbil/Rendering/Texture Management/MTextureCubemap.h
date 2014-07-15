#pragma once

#include "../../RenderDataHandler.h"
#include "TextureSettings.h"


//Represents a cubemap texture.
//TODO: Whenever texture data is changed, mipmaps are immediately regenerated if enabled. Double-check that this is necessary and good design.
//PRIORITY: Add depth component support.
class MTextureCubemap
{
public:

    //Gets the currently-bound cubemap texture. Returns 0 if no texture is currently-bound.
    static const MTextureCubemap * CurrentlyBound(void) { return currentlyBound; }


    //Constructors/destructors.

    MTextureCubemap(const TextureSampleSettings & _settings, PixelSizes _pixelSize, bool useMipmapping)
        : texHandle(0), width(0), height(0), settings(_settings), pixelSize(_pixelSize), usesMipmaps(useMipmapping)
    {
        assert(!IsPixelSizeDepth(_pixelSize));
    }
    ~MTextureCubemap(void) { DeleteIfValid(); }

    MTextureCubemap(MTextureCubemap & cpy); //Intentionally not implemented.


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


    //Texture operations. All operations other than "Create" and "Bind" cause an OpenGL error if this is not a valid texture.

    //Creates a new texture with no data.
    //Deletes the previous texture held by this instance if one existed.
    void Create(void) { Create(settings, usesMipmaps, pixelSize); }
    //Creates a new texture with no data.
    //Deletes the previous texture held by this instance if one existed.
    void Create(const TextureSampleSettings & sampleSettings, bool useMipmaps, PixelSizes pixelSize);

    //If this is a valid texture, deletes it from OpenGL.
    //Returns whether anything needed to be deleted.
    bool DeleteIfValid(void);

    //Clears all faces' data.
    void ClearData(unsigned int newW = 0, unsigned int newH = 0);


    //Sets this cubemap texture as the active one.
    //If this isn't a valid texture, then the currently-active cubemap texture is just deactivated.
    void Bind(void) const
    {
        if (currentlyBound != this)
        {
            currentlyBound = (texHandle == 0) ? 0 : this;
            glBindTexture(GL_TEXTURE_CUBE_MAP, texHandle);
        }
    }


    //Sets the given face's data from the given file.
    //"shouldUpdateMipmaps" is only applicable if this texture uses mipmaps.
    //This operation fails if the texture can't be found, or isn't the same size as the rest of the cubemap.
    //Returns an error message, or the empty string if everything went fine.
    std::string SetDataFromFile(CubeTextureTypes face, std::string filePath, bool shouldUpdateMipmaps);
    //Sets this cubemap's data from the given files.
    //Returns an error message, or the empty string if everything went fine.
    std::string SetDataFromFiles(std::string negXPath, std::string negYPath, std::string negZPath,
                                 std::string posXPath, std::string posYPath, std::string posZPath)
    {
        return SetDataFromFiles(negXPath, negYPath, negZPath, posXPath, posYPath, posZPath, usesMipmaps);
    }
    //Sets this cubemap's data from the given files.
    //Returns an error message, or the empty string if everything went fine.
    std::string SetDataFromFiles(std::string negXPath, std::string negYPath, std::string negZPath,
                                 std::string posXPath, std::string posYPath, std::string posZPath,
                                 bool useMipmapping);


    //Lots of macro abuse below because I am lazy.

    //If an invalid pixel size is passed in, the current pixel size is used.
    //All faces must be the same width/height.
    //Returns whether the operation succeeded.

#define DEF_SET_DATA(pixelType, colorType, defaultPixelSize) \
    bool SetData ## colorType(const Array2D<pixelType> & negXData, const Array2D<pixelType> & negYData, const Array2D<pixelType> & negZData, \
                              const Array2D<pixelType> & posXData, const Array2D<pixelType> & posYData, const Array2D<pixelType> & posZData, \
                              PixelSizes newPixelSize = defaultPixelSize) \
    { \
        return SetData ## colorType(negXData, negYData, negZData, posXData, posYData, posZData, usesMipmaps, newPixelSize); \
    } \
    bool SetData ## colorType(const Array2D<pixelType> & negXData, const Array2D<pixelType> & negYData, const Array2D<pixelType> & negZData, \
                              const Array2D<pixelType> & posXData, const Array2D<pixelType> & posYData, const Array2D<pixelType> & posZData, \
                              bool useMipmaps, PixelSizes newPixelSize = defaultPixelSize)

    DEF_SET_DATA(Vector4b, Color, PS_16U_DEPTH);
    DEF_SET_DATA(Vector4f, Color, PS_16U_DEPTH);
    DEF_SET_DATA(Vector4u, Color, PS_16U_DEPTH);
    DEF_SET_DATA(unsigned char, Greyscale, PS_16U_DEPTH);
    DEF_SET_DATA(float, Greyscale, PS_16U_DEPTH);
    DEF_SET_DATA(unsigned int, Greyscale, PS_16U_DEPTH);
    DEF_SET_DATA(unsigned char, Depth, PS_8U);
    DEF_SET_DATA(float, Depth, PS_8U);
    DEF_SET_DATA(unsigned int, Depth, PS_8U);


    //If the given data is not the same size as the rest of the cubemap, the operation fails.
    //Returns whether the operation succeeded.

#define DEF_SET_FACE(pixelType, colorType) \
    bool SetFace ## colorType(CubeTextureTypes face, const Array2D<pixelType> & pixelData, bool shouldUpdateMipmaps)

    DEF_SET_FACE(Vector4b, Color);
    DEF_SET_FACE(Vector4f, Color);
    DEF_SET_FACE(Vector4u, Color);
    DEF_SET_FACE(unsigned char, Greyscale);
    DEF_SET_FACE(float, Greyscale);
    DEF_SET_FACE(unsigned int, Greyscale);
    DEF_SET_FACE(unsigned char, Depth);
    DEF_SET_FACE(float, Depth);
    DEF_SET_FACE(unsigned int, Depth);


    //If the given array extends past the texture bounds, the operation fails.
    //Returns whether the operation succeeded.

#define DEF_UPDATE_FACE(pixelType, colorType) \
    bool UpdateFace ## colorType(CubeTextureTypes face, const Array2D<pixelType> & newData, bool updateMips, unsigned int offsetX = 0, unsigned int offsetY = 0)

    DEF_UPDATE_FACE(Vector4b, Color);
    DEF_UPDATE_FACE(Vector4f, Color);
    DEF_UPDATE_FACE(Vector4u, Color);
    DEF_UPDATE_FACE(unsigned char, Greyscale);
    DEF_UPDATE_FACE(float, Greyscale);
    DEF_UPDATE_FACE(unsigned int, Greyscale);
    DEF_UPDATE_FACE(unsigned char, Depth);
    DEF_UPDATE_FACE(float, Depth);
    DEF_UPDATE_FACE(unsigned int, Depth);


    //If the given array is the wrong size, the operation fails.
    //Returns whether the operation succeeded.

#define DEF_GET_FACE(pixelType, colorType) \
    bool GetFace ## colorType(CubeTextureTypes face, Array2D<pixelType> & outData)

    DEF_GET_FACE(Vector4b, Color);
    DEF_GET_FACE(Vector4f, Color);
    DEF_GET_FACE(Vector4u, Color);
    DEF_GET_FACE(unsigned char, Greyscale);
    DEF_GET_FACE(float, Greyscale);
    DEF_GET_FACE(unsigned int, Greyscale);
    DEF_GET_FACE(unsigned char, Depth);
    DEF_GET_FACE(float, Depth);
    DEF_GET_FACE(unsigned int, Depth);


private:

    RenderObjHandle texHandle;

    unsigned int width, height;

    TextureSampleSettings settings;
    PixelSizes pixelSize;
    bool usesMipmaps;


    template<typename Type>
    //Finds whether all given arrays have the exact same dimensions.
    static bool AreSameSize(const Array2D<Type> & one, const Array2D<Type> & two, const Array2D<Type> & three,
                            const Array2D<Type> & four, const Array2D<Type> & five, const Array2D<Type> & six)
    {
        //Oh dear god this expression.
        return one.GetWidth() == two.GetWidth() && one.GetHeight() == two.GetHeight() &&
            one.GetWidth() == three.GetWidth() && one.GetHeight() == three.GetHeight() &&
            one.GetWidth() == four.GetWidth() && one.GetHeight() == four.GetHeight() &&
            one.GetWidth() == five.GetWidth() && one.GetHeight() == five.GetHeight() &&
            one.GetWidth() == six.GetWidth() && one.GetHeight() == six.GetHeight() &&
            two.GetWidth() == three.GetWidth() && two.GetHeight() == three.GetHeight() &&
            two.GetWidth() == four.GetWidth() && two.GetHeight() == four.GetHeight() &&
            two.GetWidth() == five.GetWidth() && two.GetHeight() == five.GetHeight() &&
            two.GetWidth() == six.GetWidth() && two.GetHeight() == six.GetHeight() &&
            three.GetWidth() == four.GetWidth() && three.GetHeight() == four.GetHeight() &&
            three.GetWidth() == five.GetWidth() && three.GetHeight() == five.GetHeight() &&
            three.GetWidth() == six.GetWidth() && three.GetHeight() == six.GetHeight() &&
            four.GetWidth() == five.GetWidth() && four.GetHeight() == five.GetHeight() &&
            four.GetWidth() == six.GetWidth() && four.GetHeight() == six.GetHeight() &&
            five.GetWidth() == six.GetWidth() && five.GetHeight() == six.GetHeight();
    }


    static const MTextureCubemap * currentlyBound;
};