#include "MTextureCubemap.h"


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


bool WriteMTexCubeFace(const MTextureCubemap * tex, CubeTextureTypes face, std::string faceName,
                       DataWriter * writer, std::string & outError)
{
    std::vector<char> texData;
    Array2D<Vector4b> rgbaBytes(1, 1);
    Array2D<Vector4f> rgbaFloats(1, 1);
    Array2D<unsigned char> greyBytes(1, 1);
    Array2D<float> greyFloats(1, 1);
    switch (tex->GetPixelSize())
    {
        case PS_8U:
            rgbaBytes.Reset(tex->GetWidth(), tex->GetHeight());
            tex->GetFaceColor(face, rgbaBytes);
            texData.resize(rgbaBytes.GetArea() * sizeof(Vector4b));
            memcpy(texData.data(), rgbaBytes.GetArray(), texData.size());
            rgbaBytes.Reset(1, 1);
            break;

        case PS_16U:
        case PS_16F:
        case PS_32F:
            rgbaFloats.Reset(tex->GetWidth(), tex->GetHeight());
            tex->GetFaceColor(face, rgbaFloats);
            texData.resize(rgbaBytes.GetArea() * sizeof(Vector4f));
            memcpy(texData.data(), rgbaFloats.GetArray(), texData.size());
            rgbaFloats.Reset(1, 1);
            break;

        case PS_8U_GREYSCALE:
            greyBytes.Reset(tex->GetWidth(), tex->GetHeight());
            tex->GetFaceGreyscale(face, greyBytes);
            texData.resize(greyBytes.GetArea() * sizeof(unsigned char));
            memcpy(texData.data(), greyBytes.GetArray(), texData.size());
            greyBytes.Reset(1, 1);
            break;

        case PS_16U_GREYSCALE:
        case PS_32F_GREYSCALE:
            greyFloats.Reset(tex->GetWidth(), tex->GetHeight());
            tex->GetFaceGreyscale(face, greyFloats);
            texData.resize(greyFloats.GetArea() * sizeof(float));
            memcpy(texData.data(), greyFloats.GetArray(), texData.size());
            greyFloats.Reset(1, 1);
            break;

        case PS_16U_DEPTH:
        case PS_24U_DEPTH:
        case PS_32F_DEPTH:
            break;

        default:
            outError = "Unknown pixel size value " + std::to_string(tex->GetPixelSize());
            return false;
    }

    std::string texDataString(texData.begin(), texData.end());
    texData.clear();

    if (!writer->WriteString(texDataString, faceName + " Binary Data", outError))
    {
        outError = "Error writing " + faceName + " binary data as a 'string': " + outError;
        return false;
    }

    return true;
}
bool ReadMTexCubeFace(MTextureCubemap * tex, CubeTextureTypes face, bool isLastFace,
                      DataReader * reader, std::string & outError)
{
    MaybeValue<std::string> tryStr = reader->ReadString(outError);
    if (!tryStr.HasValue())
    {
        outError = "Error reading in texture binary data as a string: " + outError;
        return false;
    }
    std::vector<char> binaryData(tryStr.GetValue().begin(), tryStr.GetValue().end());
    tryStr.GetValue().clear();

    std::vector<char> texData;
    Array2D<Vector4b> rgbaBytes(1, 1);
    Array2D<Vector4f> rgbaFloats(1, 1);
    Array2D<unsigned char> greyBytes(1, 1);
    Array2D<float> greyFloats(1, 1);

    unsigned int area = tex->GetWidth() * tex->GetHeight();

    switch (tex->GetPixelSize())
    {
        case PS_8U:
            if ((area * sizeof(Vector4b)) != binaryData.size())
            {
                outError = "Texture data should be " + std::to_string(area * sizeof(Vector4b)) +
                               " bytes, but it was " + std::to_string(binaryData.size());
                return false;
            }
            rgbaBytes.Reset(tex->GetWidth(), tex->GetHeight());
            memcpy(rgbaBytes.GetArray(), binaryData.data(), binaryData.size());
            binaryData.clear();
            tex->SetFaceColor(face, rgbaBytes, isLastFace);
            break;

        case PS_16U:
        case PS_16F:
        case PS_32F:
            if ((area * sizeof(Vector4f)) != binaryData.size())
            {
                outError = "Texture data should be " + std::to_string(area * sizeof(Vector4f)) +
                               " bytes, but it was " + std::to_string(binaryData.size());
                return false;
            }
            rgbaFloats.Reset(tex->GetWidth(), tex->GetHeight());
            memcpy(rgbaFloats.GetArray(), binaryData.data(), binaryData.size());
            binaryData.clear();
            tex->SetFaceColor(face, rgbaFloats, isLastFace);
            break;

        case PS_8U_GREYSCALE:
            if ((area * sizeof(unsigned char)) != binaryData.size())
            {
                outError = "Texture data should be " + std::to_string(area * sizeof(unsigned char)) +
                               " bytes, but it was " + std::to_string(binaryData.size());
                return false;
            }
            greyBytes.Reset(tex->GetWidth(), tex->GetHeight());
            memcpy(greyBytes.GetArray(), binaryData.data(), binaryData.size());
            binaryData.clear();
            tex->SetFaceGreyscale(face, greyBytes, isLastFace);
            break;

        case PS_16U_GREYSCALE:
        case PS_32F_GREYSCALE:
            if ((area * sizeof(float)) != binaryData.size())
            {
                outError = "Texture data should be " + std::to_string(area * sizeof(float)) +
                               " bytes, but it was " + std::to_string(binaryData.size());
                return false;
            }
            greyFloats.Reset(tex->GetWidth(), tex->GetHeight());
            memcpy(greyFloats.GetArray(), binaryData.data(), binaryData.size());
            binaryData.clear();
            tex->SetFaceGreyscale(face, greyFloats, isLastFace);
            break;

        case PS_16U_DEPTH:
        case PS_24U_DEPTH:
        case PS_32F_DEPTH:
            break;

        default:
            assert(false);
            outError = "Unknown pixel size";
            return false;
    }

    return true;
}

bool MTextureCubemap::WriteData(DataWriter * writer, std::string & outError) const
{
    if (!writer->WriteUInt(width, "Width", outError))
    {
        outError = "Error writing width (" + std::to_string(width) + "): " + outError;
        return false;
    }
    if (!writer->WriteUInt(height, "Height", outError))
    {
        outError = "Error writing height (" + std::to_string(height) + "): " + outError;
        return false;
    }

    std::string pixelSizeStr;
    switch (pixelSize)
    {
        case PS_8U: pixelSizeStr = "8U Color"; break;
        case PS_16U: pixelSizeStr = "16U Color"; break;
        case PS_16F: pixelSizeStr = "16F Color"; break;
        case PS_32F: pixelSizeStr = "32F Color"; break;

        case PS_8U_GREYSCALE: pixelSizeStr = "8U Greyscale"; break;
        case PS_16U_GREYSCALE: pixelSizeStr = "16U Greyscale"; break;
        case PS_32F_GREYSCALE: pixelSizeStr = "32F Greyscale"; break;

        case PS_16U_DEPTH: pixelSizeStr = "16U Depth"; break;
        case PS_24U_DEPTH: pixelSizeStr = "24U Depth"; break;
        case PS_32F_DEPTH: pixelSizeStr = "32F Depth"; break;

        default:
            assert(false);
            outError = "Unknown pixel size: " + std::to_string(pixelSize);
            return false;
    }
    if (!writer->WriteString(pixelSizeStr, "Pixel Size (8U-32F)/Type (Color/Greyscale/Depth)", outError))
    {
        outError = "Error writing out pixel size/type '" + pixelSizeStr + "': " + outError;
        return false;
    }

    if (!writer->WriteBool(usesMipmaps, "Uses Mipmaps", outError))
    {
        outError = "Error writing out whether this texture uses mipmaps (" +
                       std::to_string(usesMipmaps) + "): " + outError;
        return false;
    }

    if (!writer->WriteDataStructure(settings, "Sampling Settings", outError))
    {
        outError = "Error writing out the sampler settings: " + outError;
        return false;
    }

    //Write out the texture data.
    if (!WriteMTexCubeFace(this, CTT_X_NEG, "NegX Face", writer, outError) ||
        !WriteMTexCubeFace(this, CTT_Y_NEG, "NegY Face", writer, outError) ||
        !WriteMTexCubeFace(this, CTT_Z_NEG, "NegZ Face", writer, outError) ||
        !WriteMTexCubeFace(this, CTT_X_POS, "PosX Face", writer, outError) ||
        !WriteMTexCubeFace(this, CTT_Y_POS, "PosY Face", writer, outError) ||
        !WriteMTexCubeFace(this, CTT_Z_POS, "PosZ Face", writer, outError))
    {
        return false;
    }


    return true;
}
bool MTextureCubemap::ReadData(DataReader * reader, std::string & outError)
{
    MaybeValue<unsigned int> tryUInt = reader->ReadUInt(outError);
    if (!tryUInt.HasValue())
    {
        outError = "Error reading texture width: " + outError;
        return false;
    }
    width = tryUInt.GetValue();
    tryUInt = reader->ReadUInt(outError);
    if (!tryUInt.HasValue())
    {
        outError = "Error reading texture height: " + outError;
        return false;
    }
    height = tryUInt.GetValue();

    MaybeValue<std::string> tryStr = reader->ReadString(outError);
    if (!tryStr.HasValue())
    {
        outError = "Error reading pixel size: " + outError;
        return false;
    }
    std::string pxSize = tryStr.GetValue();
    if (pxSize.compare("8U Color") == 0)
        pixelSize = PS_8U;
    else if (pxSize.compare("16U Color") == 0)
        pixelSize = PS_16U;
    else if (pxSize.compare("16F Color") == 0)
        pixelSize = PS_16F;
    else if (pxSize.compare("32F Color") == 0)
        pixelSize = PS_32F;
    else if (pxSize.compare("8U Greyscale") == 0)
        pixelSize = PS_8U_GREYSCALE;
    else if (pxSize.compare("16U Greyscale") == 0)
        pixelSize = PS_16U_GREYSCALE;
    else if (pxSize.compare("32F Greyscale") == 0)
        pixelSize = PS_32F_GREYSCALE;
    else if (pxSize.compare("16U Depth") == 0)
        pixelSize = PS_16U_DEPTH;
    else if (pxSize.compare("24U Depth") == 0)
        pixelSize = PS_24U_DEPTH;
    else if (pxSize.compare("32F Depth") == 0)
        pixelSize = PS_32F_DEPTH;
    else
    {
        outError = "Unknown pixel size input '" + pxSize + "'";
        return false;
    }


    MaybeValue<bool> tryBool = reader->ReadBool(outError);
    if (!tryBool.HasValue())
    {
        outError = "Error reading in whether this texture uses mipmaps: " + outError;
        return false;
    }
    usesMipmaps = tryBool.GetValue();

    if (!reader->ReadDataStructure(settings, outError))
    {
        outError = "Error reading the sampling settings: " + outError;
        return false;
    }


    Create();
    if (!ReadMTexCubeFace(this, CTT_X_NEG, false, reader, outError) ||
        !ReadMTexCubeFace(this, CTT_Y_NEG, false, reader, outError) ||
        !ReadMTexCubeFace(this, CTT_Z_NEG, false, reader, outError) ||
        !ReadMTexCubeFace(this, CTT_X_POS, false, reader, outError) ||
        !ReadMTexCubeFace(this, CTT_Y_POS, false, reader, outError) ||
        !ReadMTexCubeFace(this, CTT_Z_POS, true, reader, outError))
    {
        return false;
    }


    return true;
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
    bool MTextureCubemap::GetFace ## colorType(CubeTextureTypes face, Array2D<pixelType> & outData) const \
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

IMPL_GET_FACE(Vector4b, Color, GL_RGBA, GL_UNSIGNED_BYTE)
IMPL_GET_FACE(Vector4f, Color, GL_RGBA, GL_FLOAT)
IMPL_GET_FACE(Vector4u, Color, GL_RGBA, GL_UNSIGNED_INT)
IMPL_GET_FACE(unsigned char, Greyscale, GL_RED, GL_UNSIGNED_BYTE)
IMPL_GET_FACE(float, Greyscale, GL_RED, GL_FLOAT)
IMPL_GET_FACE(unsigned int, Greyscale, GL_RED, GL_UNSIGNED_INT)
IMPL_GET_FACE(unsigned char, Depth, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE)
IMPL_GET_FACE(float, Depth, GL_DEPTH_COMPONENT, GL_FLOAT)
IMPL_GET_FACE(unsigned int, Depth, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT)