#include "MTexture.h"

#include <iostream>
#include "../../DebugAssist.h"


const MTexture * MTexture::currentBound = 0;

void MTexture::SetSettings(const TextureSampleSettings & newSettings)
{
    settings = newSettings;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyAllSettings(TextureTypes::TT_2D, UsesMipmaps());
    }
}

void MTexture::SetMinFilterType(TextureSampleSettings::FilteringTypes newFiltering)
{
    settings.MinFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyMinFilter(TextureTypes::TT_2D, UsesMipmaps());
    }
}
void MTexture::SetMagFilterType(TextureSampleSettings::FilteringTypes newFiltering)
{
    settings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyMagFilter(TextureTypes::TT_2D, UsesMipmaps());
    }
}
void MTexture::SetFilterType(TextureSampleSettings::FilteringTypes newFiltering)
{
    settings.MinFilter = newFiltering;
    settings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyFilter(TextureTypes::TT_2D, UsesMipmaps());
    }
}

void MTexture::SetHorzWrappingType(TextureSampleSettings::WrappingTypes wrapping)
{
    settings.HorzWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyHorzWrapping(TextureTypes::TT_2D);
    }
}
void MTexture::SetVertWrappingType(TextureSampleSettings::WrappingTypes wrapping)
{
    settings.VertWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyVertWrapping(TextureTypes::TT_2D);
    }
}
void MTexture::SetWrappingType(TextureSampleSettings::WrappingTypes wrapping)
{
    settings.HorzWrap = wrapping;
    settings.VertWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyWrapping(TextureTypes::TT_2D);
    }
}


void MTexture::Create(const TextureSampleSettings & texSettings, bool useMipmaps, PixelSizes _pixelSize)
{
    DeleteIfValid();

    settings = texSettings;
    usesMipmaps = useMipmaps;
    pixelSize = _pixelSize;
    width = 0;
    height = 0;

    glGenTextures(1, &texHandle);
    Bind();
    settings.ApplyAllSettings(TextureTypes::TT_2D, usesMipmaps);

    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), 0, 0, 0, GetCPUFormat(), GetComponentType(), 0);
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_2D);
}

bool MTexture::DeleteIfValid(void)
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


void MTexture::ClearData(unsigned int newW, unsigned int newH)
{
    if (!IsValidTexture()) return;

    Bind();

    width = newW;
    height = newH;

    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), width, height, 0, GetCPUFormat(), GetComponentType(), 0);
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_2D);
}

bool MTexture::LoadImageFromFile(std::string filePath, Array2D<Vector4b> & outData)
{
    sf::Image img;
    if (!img.loadFromFile(filePath)) return false;

    outData.Reset(img.getSize().x, img.getSize().y);
    outData.Fill((Vector4b*)img.getPixelsPtr());

    return true;
}
bool MTexture::SetDataFromFile(std::string filePath, PixelSizes newSize, std::string & outError)
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
    sf::Image img;
    if (!img.loadFromFile(filePath))
    {
        outError = "Failed to load image from file.";
        return false;
    }


    //Update this texture's data.

    width = img.getSize().x;
    height = img.getSize().y;
    pixelSize = newSize;

    Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
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

bool MTexture::SetColorData(const Array2D<Vector4b> & pixelData, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    if (IsPixelSizeColor(newSize)) pixelSize = newSize;

    if (!IsColorTexture()) return false;

    width = pixelData.GetWidth();
    height = pixelData.GetHeight();
    pixelSize = newSize;

    Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}
bool MTexture::SetColorData(const Array2D<Vector4f> & pixelData, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    if (IsPixelSizeColor(newSize)) pixelSize = newSize;

    if (!IsColorTexture()) return false;

    width = pixelData.GetWidth();
    height = pixelData.GetHeight();
    pixelSize = newSize;

    Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), width, height, 0, GL_RGBA, GL_FLOAT, pixelData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}
bool MTexture::UpdateColorData(const Array2D<Vector4b> & pixelData, unsigned int offX, unsigned int offY)
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
bool MTexture::UpdateColorData(const Array2D<Vector4f> & pixelData, unsigned int offX, unsigned int offY)
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

bool MTexture::SetGreyscaleData(const Array2D<unsigned char> & greyscaleData, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    if (IsPixelSizeGreyscale(newSize)) pixelSize = newSize;

    if (!IsGreyscaleTexture()) return false;

    width = greyscaleData.GetWidth();
    height = greyscaleData.GetHeight();
    pixelSize = newSize;

    Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), width, height, 0, GL_RED, GL_UNSIGNED_BYTE, greyscaleData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}
bool MTexture::SetGreyscaleData(const Array2D<float> & greyscaleData, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    if (IsPixelSizeGreyscale(newSize)) pixelSize = newSize;

    if (!IsGreyscaleTexture()) return false;

    width = greyscaleData.GetWidth();
    height = greyscaleData.GetHeight();
    pixelSize = newSize;

    Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), width, height, 0, GL_RED, GL_FLOAT, greyscaleData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}
bool MTexture::UpdateGreyscaleData(const Array2D<unsigned char> & pixelData, unsigned int offX, unsigned int offY)
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
bool MTexture::UpdateGreyscaleData(const Array2D<float> & pixelData, unsigned int offX, unsigned int offY)
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

bool MTexture::SetDepthData(const Array2D<unsigned char> & depthData, PixelSizes newSize)
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
bool MTexture::SetDepthData(const Array2D<float> & depthData, PixelSizes newSize)
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
bool MTexture::UpdateDepthData(const Array2D<unsigned char> & pixelData, unsigned int offX, unsigned int offY)
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
bool MTexture::UpdateDepthData(const Array2D<float> & pixelData, unsigned int offX, unsigned int offY)
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


bool MTexture::GetColorData(Array2D<Vector4b> & outData)
{
    if (!IsValidTexture() || !IsColorTexture() || outData.GetWidth() != width || outData.GetHeight() != height)
        return false;

    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, outData.GetArray());

    return true;
}
bool MTexture::GetColorData(Array2D<Vector4f> & outData)
{
    if (!IsValidTexture() || !IsColorTexture() || outData.GetWidth() != width || outData.GetHeight() != height)
        return false;

    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, outData.GetArray());

    return true;
}

bool MTexture::GetGreyscaleData(Array2D<unsigned char> & outData)
{
    if (!IsValidTexture() || !IsColorTexture() || outData.GetWidth() != width || outData.GetHeight() != height)
        return false;

    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, outData.GetArray());

    return true;
}
bool MTexture::GetGreyscaleData(Array2D<float> & outData)
{
    if (!IsValidTexture() || !IsColorTexture() || outData.GetWidth() != width || outData.GetHeight() != height)
        return false;

    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, outData.GetArray());

    return true;
}

bool MTexture::GetDepthData(Array2D<unsigned char> & outData)
{
    if (!IsValidTexture() || !IsDepthTexture() || outData.GetWidth() != width || outData.GetHeight() != height)
        return false;

    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, outData.GetArray());

    return true;
}
bool MTexture::GetDepthData(Array2D<float> & outData)
{
    if (!IsValidTexture() || !IsDepthTexture() || outData.GetWidth() != width || outData.GetHeight() != height)
        return false;

    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, outData.GetArray());

    return true;
}

GLenum MTexture::GetCPUFormat(void) const
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
GLenum MTexture::GetComponentType(void) const
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