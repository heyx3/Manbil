#include "MTexture2D.h"

#include <iostream>
#include "../../DebugAssist.h"


void MTexture2D::SetSettings(const TextureSampleSettings2D & newSettings)
{
    settings = newSettings;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyAllSettings(UsesMipmaps());
    }
}

void MTexture2D::SetMinFilterType(FilteringTypes newFiltering)
{
    settings.MinFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyMinFilter(UsesMipmaps());
    }
}
void MTexture2D::SetMagFilterType(FilteringTypes newFiltering)
{
    settings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyMagFilter(UsesMipmaps());
    }
}
void MTexture2D::SetFilterType(FilteringTypes newFiltering)
{
    settings.MinFilter = newFiltering;
    settings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyFilter(UsesMipmaps());
    }
}

void MTexture2D::SetHorzWrappingType(WrappingTypes wrapping)
{
    settings.HorzWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyHorzWrapping();
    }
}
void MTexture2D::SetVertWrappingType(WrappingTypes wrapping)
{
    settings.VertWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyVertWrapping();
    }
}
void MTexture2D::SetWrappingType(WrappingTypes wrapping)
{
    settings.HorzWrap = wrapping;
    settings.VertWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyWrapping();
    }
}


bool MTexture2D::WriteData(DataWriter * writer, std::string & outError) const
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
    std::vector<char> texData;
    Array2D<Vector4b> rgbaBytes(1, 1);
    Array2D<Vector4f> rgbaFloats(1, 1);
    Array2D<unsigned char> greyBytes(1, 1);
    Array2D<float> greyFloats(1, 1);
    switch (pixelSize)
    {
        case PS_8U:
            rgbaBytes.Reset(width, height);
            GetColorData(rgbaBytes);
            texData.resize(rgbaBytes.GetNumbElements() * sizeof(Vector4b));
            memcpy(texData.data(), rgbaBytes.GetArray(), texData.size());
            rgbaBytes.Reset(1, 1);
            break;

        case PS_16U:
        case PS_16F:
        case PS_32F:
            rgbaFloats.Reset(width, height);
            GetColorData(rgbaFloats);
            texData.resize(rgbaBytes.GetNumbElements() * sizeof(Vector4f));
            memcpy(texData.data(), rgbaFloats.GetArray(), texData.size());
            rgbaFloats.Reset(1, 1);
            break;

        case PS_8U_GREYSCALE:
            greyBytes.Reset(width, height);
            GetGreyscaleData(greyBytes);
            texData.resize(greyBytes.GetNumbElements() * sizeof(unsigned char));
            memcpy(texData.data(), greyBytes.GetArray(), texData.size());
            greyBytes.Reset(1, 1);
            break;

        case PS_16U_GREYSCALE:
        case PS_32F_GREYSCALE:
            greyFloats.Reset(width, height);
            GetGreyscaleData(greyFloats);
            texData.resize(greyFloats.GetNumbElements() * sizeof(float));
            memcpy(texData.data(), greyFloats.GetArray(), texData.size());
            greyFloats.Reset(1, 1);
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
bool MTexture2D::ReadData(DataReader * reader, std::string & outError)
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

    tryStr = reader->ReadString(outError);
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

    unsigned int area = width * height;

    switch (pixelSize)
    {
        case PS_8U:
            if ((area * sizeof(Vector4b)) != binaryData.size())
            {
                outError = "Texture data should be " + std::to_string(area * sizeof(Vector4b)) +
                               " bytes, but it was " + std::to_string(binaryData.size());
                return false;
            }
            rgbaBytes.Reset(width, height);
            memcpy(rgbaBytes.GetArray(), binaryData.data(), binaryData.size());
            binaryData.clear();
            SetColorData(rgbaBytes);
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
            rgbaFloats.Reset(width, height);
            memcpy(rgbaFloats.GetArray(), binaryData.data(), binaryData.size());
            binaryData.clear();
            SetColorData(rgbaFloats);
            break;

        case PS_8U_GREYSCALE:
            if ((area * sizeof(unsigned char)) != binaryData.size())
            {
                outError = "Texture data should be " + std::to_string(area * sizeof(unsigned char)) +
                               " bytes, but it was " + std::to_string(binaryData.size());
                return false;
            }
            greyBytes.Reset(width, height);
            memcpy(greyBytes.GetArray(), binaryData.data(), binaryData.size());
            binaryData.clear();
            SetGreyscaleData(greyBytes);
            break;

        case PS_16U_GREYSCALE:
        case PS_32F_GREYSCALE:
            if ((area * sizeof(float)) != binaryData.size())
            {
                outError = "Texture data should be " + std::to_string(area * sizeof(float)) +
                               " bytes, but it was " + std::to_string(binaryData.size());
                return false;
            }
            greyFloats.Reset(width, height);
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


void MTexture2D::Create(const TextureSampleSettings2D & texSettings, bool useMipmaps, PixelSizes _pixelSize)
{
    DeleteIfValid();

    settings = texSettings;
    usesMipmaps = useMipmaps;
    pixelSize = _pixelSize;
    width = 0;
    height = 0;

    glGenTextures(1, &texHandle);
    Bind();
    settings.ApplyAllSettings(usesMipmaps);

    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), 0, 0, 0, GetCPUFormat(), GetComponentType(), 0);
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_2D);
}

bool MTexture2D::DeleteIfValid(void)
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


void MTexture2D::ClearData(unsigned int newW, unsigned int newH)
{
    if (!IsValidTexture()) return;

    Bind();

    width = newW;
    height = newH;

    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), width, height, 0, GetCPUFormat(), GetComponentType(), 0);
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_2D);
}

bool MTexture2D::LoadImageFromFile(std::string filePath, Array2D<Vector4b> & outData)
{
    sf::Image img;
    if (!img.loadFromFile(filePath)) return false;

    //Flip the Y.
    outData.Reset(img.getSize().x, img.getSize().y);
    outData.FillFunc([&img](Vector2u loc, Vector4b* outCol)
    {
        sf::Color col = img.getPixel(loc.x, img.getSize().y - 1 - loc.y);
        *outCol = Vector4b(col.r, col.g, col.b, col.a);
    });

    return true;
}
bool MTexture2D::SetDataFromFile(std::string filePath, PixelSizes newSize, std::string & outError)
{
    ClearAllRenderingErrors();


    //Check for errors.
    if (!IsValidTexture())
    {
        outError = "Must call 'Create' before using this texture!";
        return false;
    }
    if (!IsPixelSizeColor(newSize))
    {
        outError = "Pixel size is '" + DebugAssist::ToString(newSize) + "' -- not a color type!";
        return false;
    }


    //Load the image.
    Array2D<Vector4b> outData(1, 1);
    if (!LoadImageFromFile(filePath, outData))
    {
        outError = "Failed to load the image from file.";
        return false;
    }


    //Update this texture's data.

    width = outData.GetWidth();
    height = outData.GetHeight();
    pixelSize = newSize;

    Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, outData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_2D);
    

    //Check for any unknown errors.
    outError = GetCurrentRenderingError();
    if (!outError.empty())
    {
        outError = "Unknown rendering error: " + outError;
        return false;
    }

    return true;
}

bool MTexture2D::SetColorData(const Array2D<Vector4b> & pixelData, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    if (IsPixelSizeColor(newSize)) pixelSize = newSize;

    if (!IsColorTexture()) return false;

    width = pixelData.GetWidth();
    height = pixelData.GetHeight();

    Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}
bool MTexture2D::SetColorData(const Array2D<Vector4f> & pixelData, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    if (IsPixelSizeColor(newSize)) pixelSize = newSize;

    if (!IsColorTexture()) return false;

    width = pixelData.GetWidth();
    height = pixelData.GetHeight();

    Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), width, height, 0, GL_RGBA, GL_FLOAT, pixelData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}
bool MTexture2D::UpdateColorData(const Array2D<Vector4b> & pixelData, unsigned int offX, unsigned int offY)
{
    if (offX + pixelData.GetWidth() > width ||
        offY + pixelData.GetHeight() > height)
    {
        return false;
    }

    Bind();
    glTexSubImage2D(GL_TEXTURE_2D, 0, offX, offY, pixelData.GetWidth(), pixelData.GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, pixelData.GetArray());

    return true;
}
bool MTexture2D::UpdateColorData(const Array2D<Vector4f> & pixelData, unsigned int offX, unsigned int offY)
{
    if (offX + pixelData.GetWidth() > width ||
        offY + pixelData.GetHeight() > height)
    {
        return false;
    }

    Bind();
    glTexSubImage2D(GL_TEXTURE_2D, 0, offX, offY, pixelData.GetWidth(), pixelData.GetHeight(), GL_RGBA, GL_FLOAT, pixelData.GetArray());

    return true;
}

bool MTexture2D::SetGreyscaleData(const Array2D<unsigned char> & greyscaleData, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    if (IsPixelSizeGreyscale(newSize)) pixelSize = newSize;

    if (!IsGreyscaleTexture()) return false;

    width = greyscaleData.GetWidth();
    height = greyscaleData.GetHeight();

    Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), width, height, 0, GL_RED, GL_UNSIGNED_BYTE, greyscaleData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}
bool MTexture2D::SetGreyscaleData(const Array2D<float> & greyscaleData, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    if (IsPixelSizeGreyscale(newSize)) pixelSize = newSize;

    if (!IsGreyscaleTexture()) return false;

    width = greyscaleData.GetWidth();
    height = greyscaleData.GetHeight();

    Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), width, height, 0, GL_RED, GL_FLOAT, greyscaleData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}
bool MTexture2D::UpdateGreyscaleData(const Array2D<unsigned char> & pixelData, unsigned int offX, unsigned int offY)
{
    if (offX + pixelData.GetWidth() > width ||
        offY + pixelData.GetHeight() > height)
    {
        return false;
    }

    Bind();
    glTexSubImage2D(GL_TEXTURE_2D, 0, offX, offY, pixelData.GetWidth(), pixelData.GetHeight(), GL_RED, GL_UNSIGNED_BYTE, pixelData.GetArray());

    return true;
}
bool MTexture2D::UpdateGreyscaleData(const Array2D<float> & pixelData, unsigned int offX, unsigned int offY)
{
    if (offX + pixelData.GetWidth() > width ||
        offY + pixelData.GetHeight() > height)
    {
        return false;
    }

    Bind();
    glTexSubImage2D(GL_TEXTURE_2D, 0, offX, offY, pixelData.GetWidth(), pixelData.GetHeight(), GL_RED, GL_FLOAT, pixelData.GetArray());

    return true;
}

bool MTexture2D::SetDepthData(const Array2D<unsigned char> & depthData, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    if (IsPixelSizeDepth(newSize)) pixelSize = newSize;

    if (!IsDepthTexture()) return false;

    Bind();
    width = depthData.GetWidth();
    height = depthData.GetHeight();
    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), width, height, 0, GL_UNSIGNED_BYTE, GL_DEPTH_COMPONENT, depthData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}
bool MTexture2D::SetDepthData(const Array2D<float> & depthData, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    if (IsPixelSizeDepth(newSize)) pixelSize = newSize;

    if (!IsDepthTexture()) return false;

    Bind();
    width = depthData.GetWidth();
    height = depthData.GetHeight();
    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), width, height, 0, GL_FLOAT, GL_DEPTH_COMPONENT, depthData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}
bool MTexture2D::UpdateDepthData(const Array2D<unsigned char> & pixelData, unsigned int offX, unsigned int offY)
{
    if (offX + pixelData.GetWidth() > width ||
        offY + pixelData.GetHeight() > height)
    {
        return false;
    }

    Bind();
    glTexSubImage2D(GL_TEXTURE_2D, 0, offX, offY, pixelData.GetWidth(), pixelData.GetHeight(), GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, pixelData.GetArray());

    return true;
}
bool MTexture2D::UpdateDepthData(const Array2D<float> & pixelData, unsigned int offX, unsigned int offY)
{
    if (offX + pixelData.GetWidth() > width ||
        offY + pixelData.GetHeight() > height)
    {
        return false;
    }

    Bind();
    glTexSubImage2D(GL_TEXTURE_2D, 0, offX, offY, pixelData.GetWidth(), pixelData.GetHeight(), GL_DEPTH_COMPONENT, GL_FLOAT, pixelData.GetArray());

    return true;
}


bool MTexture2D::GetColorData(Array2D<Vector4b> & outData) const
{
    if (!IsValidTexture() || !IsColorTexture() || outData.GetWidth() != width || outData.GetHeight() != height)
        return false;

    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, outData.GetArray());

    return true;
}
bool MTexture2D::GetColorData(Array2D<Vector4f> & outData) const
{
    if (!IsValidTexture() || !IsColorTexture() || outData.GetWidth() != width || outData.GetHeight() != height)
        return false;

    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, outData.GetArray());

    return true;
}

bool MTexture2D::GetGreyscaleData(Array2D<unsigned char> & outData) const
{
    if (!IsValidTexture() || !IsColorTexture() || outData.GetWidth() != width || outData.GetHeight() != height)
        return false;

    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, outData.GetArray());

    return true;
}
bool MTexture2D::GetGreyscaleData(Array2D<float> & outData) const
{
    if (!IsValidTexture() || !IsColorTexture() || outData.GetWidth() != width || outData.GetHeight() != height)
        return false;

    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, outData.GetArray());

    return true;
}

bool MTexture2D::GetDepthData(Array2D<unsigned char> & outData) const
{
    if (!IsValidTexture() || !IsDepthTexture() || outData.GetWidth() != width || outData.GetHeight() != height)
        return false;

    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, outData.GetArray());

    return true;
}
bool MTexture2D::GetDepthData(Array2D<float> & outData) const
{
    if (!IsValidTexture() || !IsDepthTexture() || outData.GetWidth() != width || outData.GetHeight() != height)
        return false;

    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, outData.GetArray());

    return true;
}

GLenum MTexture2D::GetCPUFormat(void) const
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
GLenum MTexture2D::GetComponentType(void) const
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