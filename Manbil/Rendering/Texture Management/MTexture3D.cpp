#include "MTexture3D.h"

#include <iostream>
#include "../../DebugAssist.h"


const MTexture3D * MTexture3D::currentBound = 0;

void MTexture3D::SetSettings(const TextureSampleSettings3D & newSettings)
{
    settings = newSettings;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyAllSettings(TextureTypes::TT_3D, UsesMipmaps());
    }
}

void MTexture3D::SetMinFilterType(FilteringTypes newFiltering)
{
    settings.MinFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyMinFilter(TextureTypes::TT_3D, UsesMipmaps());
    }
}
void MTexture3D::SetMagFilterType(FilteringTypes newFiltering)
{
    settings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyMagFilter(TextureTypes::TT_3D, UsesMipmaps());
    }
}
void MTexture3D::SetFilterType(FilteringTypes newFiltering)
{
    settings.MinFilter = newFiltering;
    settings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyFilter(TextureTypes::TT_3D, UsesMipmaps());
    }
}

void MTexture3D::SetXWrappingType(WrappingTypes wrapping)
{
    settings.XWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyXWrapping(TextureTypes::TT_3D);
    }
}
void MTexture3D::SetYWrappingType(WrappingTypes wrapping)
{
    settings.YWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyYWrapping(TextureTypes::TT_3D);
    }
}
void MTexture3D::SetZWrappingType(WrappingTypes wrapping)
{
    settings.ZWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyZWrapping(TextureTypes::TT_3D);
    }
}
void MTexture3D::SetWrappingType(WrappingTypes wrapping)
{
    settings.XWrap = wrapping;
    settings.YWrap = wrapping;
    settings.ZWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyWrapping(TextureTypes::TT_3D);
    }
}

bool MTexture3D::WriteData(DataWriter * writer, std::string & outError) const
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
    if (!writer->WriteUInt(depth, "Depth", outError))
    {
        outError = "Error writing depth (" + std::to_string(depth) + "): " + outError;
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
    std::vector<char> texData;
    Array3D<Vector4b> rgbaBytes(1, 1, 1);
    Array3D<Vector4f> rgbaFloats(1, 1, 1);
    Array3D<unsigned char> greyBytes(1, 1, 1);
    Array3D<float> greyFloats(1, 1, 1);
    switch (pixelSize)
    {
        case PS_8U:
            rgbaBytes.Reset(width, height, depth);
            GetColorData(rgbaBytes);
            texData.resize(rgbaBytes.GetVolume() * sizeof(Vector4b));
            memcpy(texData.data(), rgbaBytes.GetArray(), texData.size());
            rgbaBytes.Reset(1, 1, 1);
            break;

        case PS_16U:
        case PS_16F:
        case PS_32F:
            rgbaFloats.Reset(width, height, depth);
            GetColorData(rgbaFloats);
            texData.resize(rgbaBytes.GetVolume() * sizeof(Vector4f));
            memcpy(texData.data(), rgbaFloats.GetArray(), texData.size());
            rgbaFloats.Reset(1, 1, 1);
            break;

        case PS_8U_GREYSCALE:
            greyBytes.Reset(width, height, depth);
            GetGreyscaleData(greyBytes);
            texData.resize(greyBytes.GetVolume() * sizeof(unsigned char));
            memcpy(texData.data(), greyBytes.GetArray(), texData.size());
            greyBytes.Reset(1, 1, 1);
            break;

        case PS_16U_GREYSCALE:
        case PS_32F_GREYSCALE:
            greyFloats.Reset(width, height, depth);
            GetGreyscaleData(greyFloats);
            texData.resize(greyFloats.GetVolume() * sizeof(float));
            memcpy(texData.data(), greyFloats.GetArray(), texData.size());
            greyFloats.Reset(1, 1, 1);
            break;

        case PS_16U_DEPTH:
        case PS_24U_DEPTH:
        case PS_32F_DEPTH:
            break;

        default:
            outError = "Unknown pixel size value " + std::to_string(pixelSize);
            return false;
    }

    std::string texDataString(texData.begin(), texData.end());
    texData.clear();

    if (!writer->WriteString(texDataString, "Texture Binary Data", outError))
    {
        outError = "Error writing texture binary data as a 'string': " + outError;
        return false;
    }


    return true;
}
bool MTexture3D::ReadData(DataReader * reader, std::string & outError)
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
    tryUInt = reader->ReadUInt(outError);
    if (!tryUInt.HasValue())
    {
        outError = "Error reading texture depth: " + outError;
        return false;
    }
    depth = tryUInt.GetValue();


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

    tryStr = reader->ReadString(outError);
    if (!tryStr.HasValue())
    {
        outError = "Error reading in texture binary data as a string: " + outError;
        return false;
    }
    std::vector<char> binaryData(tryStr.GetValue().begin(), tryStr.GetValue().end());
    tryStr.GetValue().clear();

    std::vector<char> texData;
    Array3D<Vector4b> rgbaBytes(1, 1, 1);
    Array3D<Vector4f> rgbaFloats(1, 1, 1);
    Array3D<unsigned char> greyBytes(1, 1, 1);
    Array3D<float> greyFloats(1, 1, 1);

    unsigned int volume = width * height * depth;

    switch (pixelSize)
    {
        case PS_8U:
            if ((volume * sizeof(Vector4b)) != binaryData.size())
            {
                outError = "Texture data should be " + std::to_string(volume * sizeof(Vector4b)) +
                               " bytes, but it was " + std::to_string(binaryData.size());
                return false;
            }
            rgbaBytes.Reset(width, height, depth);
            memcpy(rgbaBytes.GetArray(), binaryData.data(), binaryData.size());
            binaryData.clear();
            SetColorData(rgbaBytes);
            break;

        case PS_16U:
        case PS_16F:
        case PS_32F:
            if ((volume * sizeof(Vector4f)) != binaryData.size())
            {
                outError = "Texture data should be " + std::to_string(volume * sizeof(Vector4f)) +
                               " bytes, but it was " + std::to_string(binaryData.size());
                return false;
            }
            rgbaFloats.Reset(width, height, depth);
            memcpy(rgbaFloats.GetArray(), binaryData.data(), binaryData.size());
            binaryData.clear();
            SetColorData(rgbaFloats);
            break;

        case PS_8U_GREYSCALE:
            if ((volume * sizeof(unsigned char)) != binaryData.size())
            {
                outError = "Texture data should be " + std::to_string(volume * sizeof(unsigned char)) +
                               " bytes, but it was " + std::to_string(binaryData.size());
                return false;
            }
            greyBytes.Reset(width, height, depth);
            memcpy(greyBytes.GetArray(), binaryData.data(), binaryData.size());
            binaryData.clear();
            SetGreyscaleData(greyBytes);
            break;

        case PS_16U_GREYSCALE:
        case PS_32F_GREYSCALE:
            if ((volume * sizeof(float)) != binaryData.size())
            {
                outError = "Texture data should be " + std::to_string(volume * sizeof(float)) +
                               " bytes, but it was " + std::to_string(binaryData.size());
                return false;
            }
            greyFloats.Reset(width, height, depth);
            memcpy(greyFloats.GetArray(), binaryData.data(), binaryData.size());
            binaryData.clear();
            SetGreyscaleData(greyFloats);
            break;

        case PS_16U_DEPTH:
        case PS_24U_DEPTH:
        case PS_32F_DEPTH:
            break;
    }

    return true;
}

void MTexture3D::Create(const TextureSampleSettings3D & texSettings, bool useMipmaps, PixelSizes _pixelSize)
{
    DeleteIfValid();

    settings = texSettings;
    usesMipmaps = useMipmaps;
    pixelSize = _pixelSize;
    width = 0;
    height = 0;

    glGenTextures(1, &texHandle);
    Bind();
    settings.ApplyAllSettings(TextureTypes::TT_3D, usesMipmaps);

    glTexImage3D(GL_TEXTURE_3D, 0, ToGLenum(pixelSize), 0, 0, 0, 0, GetCPUFormat(), GetComponentType(), 0);
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_3D);
}

bool MTexture3D::DeleteIfValid(void)
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


void MTexture3D::ClearData(unsigned int newW, unsigned int newH, unsigned int newD)
{
    if (!IsValidTexture()) return;

    Bind();

    width = newW;
    height = newH;
    depth = newD;

    glTexImage3D(GL_TEXTURE_3D, 0, ToGLenum(pixelSize), width, height, depth, 0,
                 GetCPUFormat(), GetComponentType(), 0);
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_3D);
}

bool MTexture3D::SetColorData(const Array3D<Vector4b> & pixelData, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    if (IsPixelSizeColor(newSize)) pixelSize = newSize;

    if (!IsColorTexture()) return false;

    width = pixelData.GetWidth();
    height = pixelData.GetHeight();
    depth = pixelData.GetDepth();

    Bind();
    glTexImage3D(GL_TEXTURE_3D, 0, ToGLenum(pixelSize), width, height, depth, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixelData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_3D);

    return true;
}
bool MTexture3D::SetColorData(const Array3D<Vector4f> & pixelData, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    if (IsPixelSizeColor(newSize)) pixelSize = newSize;

    if (!IsColorTexture()) return false;

    width = pixelData.GetWidth();
    height = pixelData.GetHeight();
    depth = pixelData.GetDepth();

    Bind();
    glTexImage3D(GL_TEXTURE_3D, 0, ToGLenum(pixelSize), width, height, depth, 0,
                 GL_RGBA, GL_FLOAT, pixelData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_3D);

    return true;
}
bool MTexture3D::UpdateColorData(const Array3D<Vector4b> & pixelData, unsigned int offX, unsigned int offY, unsigned int offZ)
{
    if (offX + pixelData.GetWidth() > width ||
        offY + pixelData.GetHeight() > height ||
        offZ + pixelData.GetDepth() > depth)
    {
        return false;
    }

    Bind();
    glTexSubImage3D(GL_TEXTURE_3D, 0, offX, offY, offZ, pixelData.GetWidth(), pixelData.GetHeight(), pixelData.GetDepth(),
                    GL_RGBA, GL_UNSIGNED_BYTE, pixelData.GetArray());

    return true;
}
bool MTexture3D::UpdateColorData(const Array3D<Vector4f> & pixelData, unsigned int offX, unsigned int offY, unsigned int offZ)
{
    if (offX + pixelData.GetWidth() > width ||
        offY + pixelData.GetHeight() > height ||
        offZ + pixelData.GetDepth() > depth)
    {
        return false;
    }

    Bind();
    glTexSubImage3D(GL_TEXTURE_3D, 0, offX, offY, offZ, pixelData.GetWidth(), pixelData.GetHeight(), pixelData.GetDepth(),
                    GL_RGBA, GL_FLOAT, pixelData.GetArray());

    return true;
}

bool MTexture3D::SetGreyscaleData(const Array3D<unsigned char> & greyscaleData, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    if (IsPixelSizeGreyscale(newSize)) pixelSize = newSize;

    if (!IsGreyscaleTexture()) return false;

    width = greyscaleData.GetWidth();
    height = greyscaleData.GetHeight();
    depth = greyscaleData.GetDepth();

    Bind();
    glTexImage3D(GL_TEXTURE_3D, 0, ToGLenum(pixelSize), width, height, depth, 0,
                 GL_RED, GL_UNSIGNED_BYTE, greyscaleData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_3D);

    return true;
}
bool MTexture3D::SetGreyscaleData(const Array3D<float> & greyscaleData, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    if (IsPixelSizeGreyscale(newSize)) pixelSize = newSize;

    if (!IsGreyscaleTexture()) return false;

    width = greyscaleData.GetWidth();
    height = greyscaleData.GetHeight();
    depth = greyscaleData.GetDepth();

    Bind();
    glTexImage3D(GL_TEXTURE_3D, 0, ToGLenum(pixelSize), width, height, depth, 0,
                 GL_RED, GL_FLOAT, greyscaleData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_3D);

    return true;
}
bool MTexture3D::UpdateGreyscaleData(const Array3D<unsigned char> & pixelData, unsigned int offX, unsigned int offY, unsigned int offZ)
{
    if (offX + pixelData.GetWidth() > width ||
        offY + pixelData.GetHeight() > height ||
        offZ + pixelData.GetDepth() > depth)
    {
        return false;
    }

    Bind();
    glTexSubImage3D(GL_TEXTURE_3D, 0, offX, offY, offZ, pixelData.GetWidth(), pixelData.GetHeight(), pixelData.GetDepth(),
                    GL_RED, GL_UNSIGNED_BYTE, pixelData.GetArray());

    return true;
}
bool MTexture3D::UpdateGreyscaleData(const Array3D<float> & pixelData, unsigned int offX, unsigned int offY, unsigned int offZ)
{
    if (offX + pixelData.GetWidth() > width ||
        offY + pixelData.GetHeight() > height ||
        offZ + pixelData.GetDepth() > depth)
    {
        return false;
    }

    Bind();
    glTexSubImage3D(GL_TEXTURE_3D, 0, offX, offY, offZ, pixelData.GetWidth(), pixelData.GetHeight(), pixelData.GetDepth(),
                    GL_RED, GL_FLOAT, pixelData.GetArray());

    return true;
}

bool MTexture3D::SetDepthData(const Array3D<unsigned char> & depthData, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    if (IsPixelSizeDepth(newSize)) pixelSize = newSize;

    if (!IsDepthTexture()) return false;

    Bind();
    width = depthData.GetWidth();
    height = depthData.GetHeight();
    depth = depthData.GetDepth();
    glTexImage3D(GL_TEXTURE_3D, 0, ToGLenum(pixelSize), width, height, depth, 0,
                 GL_UNSIGNED_BYTE, GL_DEPTH_COMPONENT, depthData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_3D);

    return true;
}
bool MTexture3D::SetDepthData(const Array3D<float> & depthData, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    if (IsPixelSizeDepth(newSize)) pixelSize = newSize;

    if (!IsDepthTexture()) return false;

    Bind();
    width = depthData.GetWidth();
    height = depthData.GetHeight();
    depth = depthData.GetDepth();
    glTexImage3D(GL_TEXTURE_3D, 0, ToGLenum(pixelSize), width, height, depth, 0,
                 GL_FLOAT, GL_DEPTH_COMPONENT, depthData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_3D);

    return true;
}
bool MTexture3D::UpdateDepthData(const Array3D<unsigned char> & pixelData, unsigned int offX, unsigned int offY, unsigned int offZ)
{
    if (offX + pixelData.GetWidth() > width ||
        offY + pixelData.GetHeight() > height ||
        offZ + pixelData.GetDepth() > depth)
    {
        return false;
    }

    Bind();
    glTexSubImage3D(GL_TEXTURE_3D, 0, offX, offY, offZ, pixelData.GetWidth(), pixelData.GetHeight(), pixelData.GetDepth(),
                    GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, pixelData.GetArray());

    return true;
}
bool MTexture3D::UpdateDepthData(const Array3D<float> & pixelData, unsigned int offX, unsigned int offY, unsigned int offZ)
{
    if (offX + pixelData.GetWidth() > width ||
        offY + pixelData.GetHeight() > height ||
        offZ + pixelData.GetDepth() > depth)
    {
        return false;
    }

    Bind();
    glTexSubImage3D(GL_TEXTURE_3D, 0, offX, offY, offZ, pixelData.GetWidth(), pixelData.GetHeight(), pixelData.GetDepth(),
                    GL_DEPTH_COMPONENT, GL_FLOAT, pixelData.GetArray());

    return true;
}


bool MTexture3D::GetColorData(Array3D<Vector4b> & outData) const
{
    if (!IsValidTexture() || !IsColorTexture() ||
        outData.GetWidth() != width || outData.GetHeight() != height || outData.GetDepth() != depth)
        return false;

    Bind();
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_UNSIGNED_BYTE, outData.GetArray());

    return true;
}
bool MTexture3D::GetColorData(Array3D<Vector4f> & outData) const
{
    if (!IsValidTexture() || !IsColorTexture() ||
        outData.GetWidth() != width || outData.GetHeight() != height || outData.GetDepth() != depth)
        return false;

    Bind();
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_FLOAT, outData.GetArray());

    return true;
}

bool MTexture3D::GetGreyscaleData(Array3D<unsigned char> & outData) const
{
    if (!IsValidTexture() || !IsColorTexture() ||
        outData.GetWidth() != width || outData.GetHeight() != height || outData.GetDepth() != depth)
        return false;

    Bind();
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_UNSIGNED_BYTE, outData.GetArray());

    return true;
}
bool MTexture3D::GetGreyscaleData(Array3D<float> & outData) const
{
    if (!IsValidTexture() || !IsColorTexture() ||
        outData.GetWidth() != width || outData.GetHeight() != height || outData.GetDepth() != depth)
        return false;

    Bind();
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_FLOAT, outData.GetArray());

    return true;
}

bool MTexture3D::GetDepthData(Array3D<unsigned char> & outData) const
{
    if (!IsValidTexture() || !IsDepthTexture() ||
        outData.GetWidth() != width || outData.GetHeight() != height || outData.GetDepth() != depth)
        return false;

    Bind();
    glGetTexImage(GL_TEXTURE_3D, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, outData.GetArray());

    return true;
}
bool MTexture3D::GetDepthData(Array3D<float> & outData) const
{
    if (!IsValidTexture() || !IsDepthTexture() ||
        outData.GetWidth() != width || outData.GetHeight() != height || outData.GetDepth() != depth)
        return false;

    Bind();
    glGetTexImage(GL_TEXTURE_3D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, outData.GetArray());

    return true;
}

GLenum MTexture3D::GetCPUFormat(void) const
{
    switch (pixelSize)
    {
        case PS_8U:
        case PS_16U:
        case PS_16F:
        case PS_32F:
            return GL_RGBA;
        case PS_8U_GREYSCALE:
        case PS_16U_GREYSCALE:
        case PS_32F_GREYSCALE:
            return GL_RED;
        case PS_16U_DEPTH:
        case PS_24U_DEPTH:
        case PS_32F_DEPTH:
            return GL_DEPTH_COMPONENT;

        default: assert(false); return GL_INVALID_ENUM;
    }
}
GLenum MTexture3D::GetComponentType(void) const
{
    switch (pixelSize)
    {
        case PS_8U:
        case PS_16U:
        case PS_8U_GREYSCALE:
        case PS_16U_GREYSCALE:
        case PS_16U_DEPTH:
        case PS_24U_DEPTH:
            return GL_UNSIGNED_BYTE;

        case PS_16F:
        case PS_32F:
        case PS_32F_GREYSCALE:
        case PS_32F_DEPTH:
            return GL_FLOAT;

        default: assert(false); return GL_INVALID_ENUM;
    }
}