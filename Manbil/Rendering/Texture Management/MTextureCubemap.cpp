#include "MTextureCubemap.h"


const MTextureCubemap * MTextureCubemap::currentlyBound = 0;

void MTextureCubemap::SetSettings(const TextureSampleSettings3D & newSettings)
{
    settings = newSettings;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyAllSettings(TextureTypes::TT_CUBE, UsesMipmaps());
    }
}

void MTextureCubemap::SetMinFilterType(FilteringTypes newFiltering)
{
    settings.MinFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyMinFilter(TextureTypes::TT_CUBE, UsesMipmaps());
    }
}
void MTextureCubemap::SetMagFilterType(FilteringTypes newFiltering)
{
    settings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyMagFilter(TextureTypes::TT_CUBE, UsesMipmaps());
    }
}
void MTextureCubemap::SetFilterType(FilteringTypes newFiltering)
{
    settings.MinFilter = newFiltering;
    settings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyFilter(TextureTypes::TT_CUBE, UsesMipmaps());
    }
}

void MTextureCubemap::SetXWrappingType(WrappingTypes wrapping)
{
    settings.XWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyXWrapping(TextureTypes::TT_CUBE);
    }
}
void MTextureCubemap::SetYWrappingType(WrappingTypes wrapping)
{
    settings.YWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyYWrapping(TextureTypes::TT_CUBE);
    }
}
void MTextureCubemap::SetZWrappingType(WrappingTypes wrapping)
{
    settings.ZWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyZWrapping(TextureTypes::TT_CUBE);
    }
}
void MTextureCubemap::SetWrappingType(WrappingTypes wrapping)
{
    settings.XWrap = wrapping;
    settings.YWrap = wrapping;
    settings.ZWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyWrapping(TextureTypes::TT_CUBE);
    }
}

void MTextureCubemap::Create(const TextureSampleSettings3D & texSettings, bool useMipmaps, PixelSizes _pixelSize)
{
    DeleteIfValid();

    settings = texSettings;
    usesMipmaps = useMipmaps;
    pixelSize = _pixelSize;

    glGenTextures(1, &texHandle);
    Bind();
    texSettings.ApplyAllSettings(TextureTypes::TT_CUBE, usesMipmaps);
    ClearData(0, 0);
}

bool MTextureCubemap::DeleteIfValid(void)
{
    if (IsValidTexture())
    {
        glDeleteTextures(1, &texHandle);
        texHandle = 0;
        return true;
    }
    else
    {
        texHandle = 0;
        return false;
    }
}


void MTextureCubemap::ClearData(unsigned int newW, unsigned int newH)
{
    if (!IsValidTexture()) return;

    Bind();
    width = newW;
    height = newH;

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, ToGLenum(pixelSize), width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, ToGLenum(pixelSize), width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, ToGLenum(pixelSize), width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, ToGLenum(pixelSize), width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, ToGLenum(pixelSize), width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, ToGLenum(pixelSize), width, height, 0, GL_RGBA, GL_FLOAT, 0);
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

std::string MTextureCubemap::SetDataFromFile(CubeTextureTypes face, std::string filePath, bool shouldUpdateMipmaps)
{
    if (!IsValidTexture())
        return "This cubemap hasn't been created yet!";

    if (!IsColorTexture())
        return "This cubemap isn't a color texture!";

    sf::Image img;
    if (!img.loadFromFile(filePath))
        return "Couldn't load file " + filePath;

    if (img.getSize().x != width || img.getSize().y != height)
        return "Cubemap size is " + std::to_string(width) + "x" + std::to_string(height) +
                  " but the image was " + std::to_string(img.getSize().x) + "x" + std::to_string(img.getSize().y);
  
    Bind();
    glTexImage2D(TextureTypeToGLEnum(face), 0, ToGLenum(pixelSize), width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
    if (shouldUpdateMipmaps) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    return "";
}
std::string MTextureCubemap::SetDataFromFiles(std::string negXPath, std::string negYPath, std::string negZPath,
                                               std::string posXPath, std::string posYPath, std::string posZPath,
                                               bool useMipmapping)
{
    if (!IsValidTexture())
        return "This cubemap hasn't been created yet!";

    if (!IsColorTexture())
        return "This cubemap isn't a color texture!";

    sf::Image negX, negY, negZ, posX, posY, posZ;
    if (!negX.loadFromFile(negXPath))
        return "Couldn't load texture '" + negXPath + "' for negative X face.";
    if (!negY.loadFromFile(negYPath))
        return "Couldn't load texture '" + negYPath + "' for negative Y face.";
    if (!negZ.loadFromFile(negZPath))
        return "Couldn't load texture '" + negZPath + "' for negative Z face.";
    if (!posX.loadFromFile(posXPath))
        return "Couldn't load texture '" + posXPath + "' for positive X face.";
    if (!posY.loadFromFile(posYPath))
        return "Couldn't load texture '" + posYPath + "' for positive Y face.";
    if (!posZ.loadFromFile(posZPath))
        return "Couldn't load texture '" + posZPath + "' for positive Z face.";

    sf::Vector2u sNegX = negX.getSize(), sNegY = negY.getSize(), sNegZ = negZ.getSize(),
                 sPosX = posX.getSize(), sPosY = posY.getSize(), sPosZ = posZ.getSize();
    if (sNegX != sNegY || sNegX != sNegZ || sNegX != sPosX || sNegX != sPosY || sNegX != sPosZ ||
        sNegY != sNegZ || sNegY != sPosX || sNegY != sPosY || sNegY != sPosZ ||
        sNegZ != sPosX || sNegZ != sPosY || sNegZ != sPosZ ||
        sPosX != sPosY || sPosX != sPosZ ||
        sPosY != sPosZ)
    {
        #define PRINT_SF_VECTOR2U(v2u) (std::to_string(v2u.x) + "x" + std::to_string(v2u.y))
        return "Not every image was the same size. NegX: " + PRINT_SF_VECTOR2U(sNegX) +
                    "; NegY: " + PRINT_SF_VECTOR2U(sNegY) + "; NegZ: " + PRINT_SF_VECTOR2U(sNegZ) +
                    "; PosX: " + PRINT_SF_VECTOR2U(sPosX) + "; PosY: " + PRINT_SF_VECTOR2U(sPosY) +
                    "; PosZ: " + PRINT_SF_VECTOR2U(sPosZ);
    }


    pixelSize = PS_8U;
    width = sNegX.x;
    height = sNegX.y;
    usesMipmaps = useMipmapping;

    Bind();
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, ToGLenum(PS_8U), sNegX.x, sNegX.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, negX.getPixelsPtr());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, ToGLenum(PS_8U), sNegX.x, sNegX.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, negY.getPixelsPtr());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, ToGLenum(PS_8U), sNegX.x, sNegX.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, negZ.getPixelsPtr());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, ToGLenum(PS_8U), sNegX.x, sNegX.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, posX.getPixelsPtr());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, ToGLenum(PS_8U), sNegX.x, sNegX.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, posY.getPixelsPtr());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, ToGLenum(PS_8U), sNegX.x, sNegX.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, posZ.getPixelsPtr());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    return "";
}



//Yay, more macro abuse!

#define IMPL_SET_DATA(pixelType, colorType, glDataType, glPixelType) \
    bool MTextureCubemap::SetData ## colorType(const Array2D<pixelType> & negX, const Array2D<pixelType> & negY, const Array2D<pixelType> & negZ, \
                                               const Array2D<pixelType> & posX, const Array2D<pixelType> & posY, const Array2D<pixelType> & posZ, \
                                               bool newUseMipmaps, PixelSizes newPixelSize) \
    { \
        if (!IsValidTexture() || !AreSameSize(negX, negY, negZ, posX, posY, posZ)) \
            return false; \
        \
        if (IsPixelSize ## colorType(newPixelSize)) pixelSize = newPixelSize; \
        if (!Is ## colorType ## Texture()) return false; \
        \
        usesMipmaps = newUseMipmaps; \
        width = negX.GetWidth(); \
        height = negX.GetHeight(); \
        \
        Bind(); \
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, ToGLenum(pixelSize), width, height, 0, glDataType, glPixelType, negX.GetArray()); \
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, ToGLenum(pixelSize), width, height, 0, glDataType, glPixelType, negY.GetArray()); \
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, ToGLenum(pixelSize), width, height, 0, glDataType, glPixelType, negZ.GetArray()); \
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, ToGLenum(pixelSize), width, height, 0, glDataType, glPixelType, posX.GetArray()); \
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, ToGLenum(pixelSize), width, height, 0, glDataType, glPixelType, posY.GetArray()); \
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, ToGLenum(pixelSize), width, height, 0, glDataType, glPixelType, posZ.GetArray()); \
        if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_CUBE_MAP); \
        \
        return true; \
    }

IMPL_SET_DATA(Vector4b, Color, GL_RGBA, GL_UNSIGNED_BYTE)
IMPL_SET_DATA(Vector4f, Color, GL_RGBA, GL_FLOAT)
IMPL_SET_DATA(Vector4u, Color, GL_RGBA, GL_UNSIGNED_INT)
IMPL_SET_DATA(unsigned char, Greyscale, GL_RED, GL_UNSIGNED_BYTE)
IMPL_SET_DATA(float, Greyscale, GL_RED, GL_FLOAT)
IMPL_SET_DATA(unsigned int, Greyscale, GL_RED, GL_UNSIGNED_INT)
IMPL_SET_DATA(unsigned char, Depth, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE)
IMPL_SET_DATA(float, Depth, GL_DEPTH_COMPONENT, GL_FLOAT)
IMPL_SET_DATA(unsigned int, Depth, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT)


#define IMPL_SET_FACE(pixelType, colorType, glDataType, glPixelType) \
    bool MTextureCubemap::SetFace ## colorType(CubeTextureTypes face, const Array2D<pixelType> & pixelData, bool shouldUpdateMips) \
    { \
        if (!IsValidTexture() || !Is ## colorType ## Texture() || \
            pixelData.GetWidth() != width || pixelData.GetHeight() != height) \
            return false; \
        \
        Bind(); \
        glTexImage2D(TextureTypeToGLEnum(face), 0, ToGLenum(pixelSize), width, height, 0, glDataType, glPixelType, pixelData.GetArray()); \
        if (shouldUpdateMips && usesMipmaps) \
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP); \
        \
        return true; \
    }

IMPL_SET_FACE(Vector4b, Color, GL_RGBA, GL_UNSIGNED_BYTE)
IMPL_SET_FACE(Vector4f, Color, GL_RGBA, GL_FLOAT)
IMPL_SET_FACE(Vector4u, Color, GL_RGBA, GL_UNSIGNED_INT)
IMPL_SET_FACE(unsigned char, Greyscale, GL_RED, GL_UNSIGNED_BYTE)
IMPL_SET_FACE(float, Greyscale, GL_RED, GL_FLOAT)
IMPL_SET_FACE(unsigned int, Greyscale, GL_RED, GL_UNSIGNED_INT)
IMPL_SET_FACE(unsigned char, Depth, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE)
IMPL_SET_FACE(float, Depth, GL_DEPTH_COMPONENT, GL_FLOAT)
IMPL_SET_FACE(unsigned int, Depth, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT)


#define IMPL_UPDATE_FACE(pixelType, colorType, glDataType, glPixelType) \
    bool MTextureCubemap::UpdateFace ## colorType(CubeTextureTypes face, const Array2D<pixelType> & newData, bool updateMips, \
                                                  unsigned int offX, unsigned int offY) \
    { \
        if (!IsValidTexture() || !Is ## colorType ## Texture() || \
            offX + newData.GetWidth() > width || offX + newData.GetHeight() > height) \
        { \
            return false; \
        } \
        \
        Bind(); \
        glTexSubImage2D(TextureTypeToGLEnum(face), 0, offX, offY, newData.GetWidth(), newData.GetHeight(), glDataType, glPixelType, newData.GetArray()); \
        if (updateMips && usesMipmaps) \
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP); \
        \
        return true; \
    }

IMPL_UPDATE_FACE(Vector4b, Color, GL_RGBA, GL_UNSIGNED_BYTE)
IMPL_UPDATE_FACE(Vector4f, Color, GL_RGBA, GL_FLOAT)
IMPL_UPDATE_FACE(Vector4u, Color, GL_RGBA, GL_UNSIGNED_INT)
IMPL_UPDATE_FACE(unsigned char, Greyscale, GL_RED, GL_UNSIGNED_BYTE)
IMPL_UPDATE_FACE(float, Greyscale, GL_RED, GL_FLOAT)
IMPL_UPDATE_FACE(unsigned int, Greyscale, GL_RED, GL_UNSIGNED_INT)
IMPL_UPDATE_FACE(unsigned char, Depth, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE)
IMPL_UPDATE_FACE(float, Depth, GL_DEPTH_COMPONENT, GL_FLOAT)
IMPL_UPDATE_FACE(unsigned int, Depth, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT)


#define IMPL_GET_FACE(pixelType, colorType, glDataType, glPixelType) \
    bool MTextureCubemap::GetFace ## colorType(CubeTextureTypes face, Array2D<pixelType> & outData) \
    { \
        if (!IsValidTexture() || !Is ## colorType ## Texture() || \
            outData.GetWidth() != width || outData.GetHeight() != height) \
        { \
            return false; \
        } \
        \
        Bind(); \
        glGetTexImage(TextureTypeToGLEnum(face), 0, glDataType, glPixelType, outData.GetArray()); \
        \
        return true; \
    }

IMPL_GET_FACE(Vector4f, Color, GL_RGBA, GL_FLOAT)
IMPL_GET_FACE(Vector4u, Color, GL_RGBA, GL_UNSIGNED_INT)
IMPL_GET_FACE(unsigned char, Greyscale, GL_RED, GL_UNSIGNED_BYTE)
IMPL_GET_FACE(float, Greyscale, GL_RED, GL_FLOAT)
IMPL_GET_FACE(unsigned int, Greyscale, GL_RED, GL_UNSIGNED_INT)
IMPL_GET_FACE(unsigned char, Depth, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE)
IMPL_GET_FACE(float, Depth, GL_DEPTH_COMPONENT, GL_FLOAT)
IMPL_GET_FACE(unsigned int, Depth, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT)