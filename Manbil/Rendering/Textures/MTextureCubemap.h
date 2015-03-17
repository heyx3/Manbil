#pragma once

#include "TextureSettings.h"
#include "../../Math/Lower Math/Array2D.h"



//Note for anybody perusing the source: this is a particularly ugly class.
//Just refer to MTexture2D and MTexture3D for an easier-to-read version of the same kind of stuff.



//Represents a cubemap texture, which is made up of six textures representing the six faces of a cube.
//Whereas 2D and 3D textures can be queried with 2D or 3D coordinates respectively,
//   cubemap textures are queried with a 3D vector (of any size; magnitude is unimportant).
//This vector points towards some spot on a certain face of the cube,
//    corresponding to a pixel on that face's texture.
//This is very useful for skyboxes, reflections, and certain lighting effects.
//It is also much less distorted than wrapping a 2D texture around a sphere's surface.
class MTextureCubemap
{
public:
    
    //Enables the ability to smoothly blend the seams between texture faces.
    static void EnableSmoothSeams(void) { glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); }
    //Disables the ability to smoothly blend the seams between texture faces.
    static void DisableSmoothSeams(void) { glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS); }


    //Constructors/destructors.

    MTextureCubemap(const TextureSampleSettings3D& _settings,
                    PixelSizes _pixelSize, bool useMipmapping);
    ~MTextureCubemap(void) { DeleteIfValid(); }

    MTextureCubemap(MTextureCubemap& cpy) = delete;


    //Getters.

    RenderObjHandle GetTextureHandle(void) const { return texHandle; }
    bool IsValidTexture(void) const { return texHandle != 0; }

    unsigned int GetWidth(void) const { return width; }
    unsigned int GetHeight(void) const { return height; }
    const TextureSampleSettings3D& GetSamplingSettings(void) const { return settings; }
    bool UsesMipmaps(void) const { return usesMipmaps; }
    PixelSizes GetPixelSize(void) const { return pixelSize; }

    bool IsColorTexture(void) const { return IsPixelSizeColor(pixelSize); }
    bool IsGreyscaleTexture(void) const { return IsPixelSizeGreyscale(pixelSize); }
    bool IsDepthTexture(void) const { return IsPixelSizeDepth(pixelSize); }


    //Setters.

    void SetSettings(const TextureSampleSettings3D& newSettings);

    void SetMinFilterType(FilteringTypes newFiltering);
    void SetMagFilterType(FilteringTypes newFiltering);
    void SetFilterType(FilteringTypes newFiltering);

    void SetXWrappingType(WrappingTypes wrapping);
    void SetYWrappingType(WrappingTypes wrapping);
    void SetZWrappingType(WrappingTypes wrapping);
    void SetWrappingType(WrappingTypes wrapping);


    //Creates a new texture with no data.
    //Deletes the previous texture held by this instance if one existed.
    void Create(void) { Create(settings, usesMipmaps, pixelSize); }
    //Creates a new texture with no data.
    //Deletes the previous texture held by this instance if one existed.
    void Create(const TextureSampleSettings3D& sampleSettings, bool useMipmaps, PixelSizes pixelSize);

    //If this is a valid texture, deletes it from OpenGL.
    //Returns whether anything needed to be deleted.
    bool DeleteIfValid(void);

    //Clears all faces' data.
    void ClearData(unsigned int newW = 0, unsigned int newH = 0);


    //Sets this cubemap texture as the active one.
    //If this isn't a valid texture, then the currently-active cubemap texture is just deactivated.
    void Bind(void) const { glBindTexture(GL_TEXTURE_CUBE_MAP, texHandle); }


    //Sets the given face's data from the given file.
    //"shouldUpdateMipmaps" is only applicable if this texture uses mipmaps.
    //This operation fails if the texture can't be found, or isn't the same size as the rest of the cubemap.
    //Returns an error message, or the empty string if everything went fine.
    std::string SetDataFromFile(CubeTextureTypes face, std::string filePath, bool shouldUpdateMipmaps);
    //Sets this cubemap's data from the given files.
    //Returns an error message, or the empty string if everything went fine.
    std::string SetDataFromFiles(std::string negXPath, std::string negYPath, std::string negZPath,
                                 std::string posXPath, std::string posYPath, std::string posZPath);
    //Sets this cubemap's data from the given files.
    //Returns an error message, or the empty string if everything went fine.
    std::string SetDataFromFiles(std::string negXPath, std::string negYPath, std::string negZPath,
                                 std::string posXPath, std::string posYPath, std::string posZPath,
                                 bool useMipmapping);


    //Trigger warning: macros.

    #pragma region Texture operations


    //The below macro defines functions that set a cubemap's data.
    //The functions will be named "SetDataColor", "SetDataGreyscale", and "SetDataDepth",
    //    signifying the various kinds of textues.
    //If an invalid pixel size is passed in to any of these, the current pixel size is kept unchanged.
    //All faces must be the same width/height.
    //Returns whether the operation succeeded.
#define DEF_SET_DATA(pixelType, colorType, defaultPixelSize) \
    bool SetData ## colorType(const Array2D<pixelType>& negXData, const Array2D<pixelType>& negYData, \
                              const Array2D<pixelType>& negZData, \
                              const Array2D<pixelType>& posXData, const Array2D<pixelType>& posYData, \
                              const Array2D<pixelType>& posZData, \
                              PixelSizes newPixelSize = defaultPixelSize) \
    { \
        return SetData ## colorType(negXData, negYData, negZData, posXData, posYData, posZData, \
                                    usesMipmaps, newPixelSize); \
    } \
    bool SetData ## colorType(const Array2D<pixelType>& negXData, const Array2D<pixelType>& negYData, \
                              const Array2D<pixelType>& negZData, \
                              const Array2D<pixelType>& posXData, const Array2D<pixelType>& posYData, \
                              const Array2D<pixelType>& posZData, \
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


    //Another macro, for defining the functions to set a single cubemap face.
    //The functions are named "SetFaceColor", "SetFaceGreyscale", and "SetFaceDepth",
    //    signifying the various kinds of textues.
    //If the given data is not the same size as the rest of the cubemap, the operation fails.
    //Returns whether the operation succeeded.
#define DEF_SET_FACE(pixelType, colorType) \
    bool SetFace ## colorType(CubeTextureTypes face, const Array2D<pixelType>& pixelData, \
                              bool shouldUpdateMipmaps)

    DEF_SET_FACE(Vector4b, Color);
    DEF_SET_FACE(Vector4f, Color);
    DEF_SET_FACE(Vector4u, Color);
    DEF_SET_FACE(unsigned char, Greyscale);
    DEF_SET_FACE(float, Greyscale);
    DEF_SET_FACE(unsigned int, Greyscale);
    DEF_SET_FACE(unsigned char, Depth);
    DEF_SET_FACE(float, Depth);
    DEF_SET_FACE(unsigned int, Depth);


    //Yet another macro, for defining functions to update part of a face's data.
    //The functions are named "UpdateFaceColor", "UpdateFaceGreyscale", and "UpdateFaceDepth",
    //    signifying the various kinds of textues.
    //If the given array extends past the texture bounds, the operation fails.
    //Returns whether the operation succeeded.
#define DEF_UPDATE_FACE(pixelType, colorType) \
    bool UpdateFace ## colorType(CubeTextureTypes face, const Array2D<pixelType>& newData, \
                                 bool updateMips, unsigned int offsetX = 0, unsigned int offsetY = 0)

    DEF_UPDATE_FACE(Vector4b, Color);
    DEF_UPDATE_FACE(Vector4f, Color);
    DEF_UPDATE_FACE(Vector4u, Color);
    DEF_UPDATE_FACE(unsigned char, Greyscale);
    DEF_UPDATE_FACE(float, Greyscale);
    DEF_UPDATE_FACE(unsigned int, Greyscale);
    DEF_UPDATE_FACE(unsigned char, Depth);
    DEF_UPDATE_FACE(float, Depth);
    DEF_UPDATE_FACE(unsigned int, Depth);


    //One last macro, for defining functions to read in a face's data from the GPU's memory.
    //The functions are named "GetFaceColor", "GetFaceGreyscale", and "GetFaceDepth",
    //    signifying the various kinds of textues.
    //If the given array is the wrong size, the operation fails.
    //Returns whether the operation succeeded.
#define DEF_GET_FACE(pixelType, colorType) \
    bool GetFace ## colorType(CubeTextureTypes face, Array2D<pixelType>& outData) const

    DEF_GET_FACE(Vector4b, Color);
    DEF_GET_FACE(Vector4f, Color);
    DEF_GET_FACE(Vector4u, Color);
    DEF_GET_FACE(unsigned char, Greyscale);
    DEF_GET_FACE(float, Greyscale);
    DEF_GET_FACE(unsigned int, Greyscale);
    DEF_GET_FACE(unsigned char, Depth);
    DEF_GET_FACE(float, Depth);
    DEF_GET_FACE(unsigned int, Depth);


    #pragma endregion


private:

    RenderObjHandle texHandle;

    unsigned int width, height;

    TextureSampleSettings3D settings;
    PixelSizes pixelSize;
    bool usesMipmaps;


    template<typename Type>
    //Finds whether all given arrays have the exact same dimensions.
    //Used to make sure that a cubemap's data is the same size on each face.
    static bool AreSameSize(const Array2D<Type>& one, const Array2D<Type>& two,
                            const Array2D<Type>& three, const Array2D<Type>& four,
                            const Array2D<Type>& five, const Array2D<Type>& six)
    {
        #define EQUALDIMS(arr1, arr2) \
            (arr1.GetWidth() == arr2.GetWidth() && arr1.GetHeight() == arr2.GetHeight())

        return EQUALDIMS(one, two) && EQUALDIMS(two, three) &&
               EQUALDIMS(three, four) && EQUALDIMS(four, five) &&
               EQUALDIMS(five, six);
    }
};