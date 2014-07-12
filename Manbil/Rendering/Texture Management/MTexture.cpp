#include "MTexture.h"


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
    ClearData();
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

    GLenum dataFormat;
    if (IsDepthTexture()) dataFormat = GL_DEPTH_COMPONENT;
    else dataFormat = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), width, height, 0, GL_FLOAT, dataFormat, 0);
}

bool MTexture::LoadImageFromFile(std::string filePath, Array2D<Vector4b> & outData)
{
    sf::Image img;
    if (!img.loadFromFile(filePath)) return false;

    outData.Reset(img.getSize().x, img.getSize().y);
    outData.Fill((Vector4b*)img.getPixelsPtr());

    return true;
}
bool MTexture::SetDataFromFile(std::string filePath, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    sf::Image img;
    if (!img.loadFromFile(filePath)) return false;

    Bind();
    width = img.getSize().x;
    height = img.getSize().y;
    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), width, height, 0, GL_UNSIGNED_BYTE, GL_RGBA, img.getPixelsPtr());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}

bool MTexture::SetData(const Array2D<Vector4b> & pixelData, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    if (!IsPixelSizeDepth(newSize)) pixelSize = newSize;

    if (IsDepthTexture()) return false;

    Bind();
    width = pixelData.GetWidth();
    height = pixelData.GetHeight();
    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), width, height, 0, GL_UNSIGNED_BYTE, GL_RGBA, pixelData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}
bool MTexture::SetData(const Array2D<Vector4f> & pixelData, PixelSizes newSize)
{
    if (!IsValidTexture()) return false;

    if (!IsPixelSizeDepth(newSize)) pixelSize = newSize;

    if (IsDepthTexture()) return false;

    Bind();
    width = pixelData.GetWidth();
    height = pixelData.GetHeight();
    glTexImage2D(GL_TEXTURE_2D, 0, ToGLenum(pixelSize), width, height, 0, GL_FLOAT, GL_RGBA, pixelData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

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


bool MTexture::GetData(Array2D<Vector4b> & outData)
{
    if (!IsValidTexture() || IsDepthTexture()) return false;

    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, outData.GetArray());

    return true;
}
bool MTexture::GetData(Array2D<Vector4f> & outData)
{
    if (!IsValidTexture() || IsDepthTexture()) return false;

    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, outData.GetArray());

    return true;
}

bool MTexture::GetDepthData(Array2D<unsigned char> & outData)
{
    if (!IsValidTexture() || !IsDepthTexture()) return false;

    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, outData.GetArray());

    return true;
}
bool MTexture::GetDepthData(Array2D<float> & outData)
{
    if (!IsValidTexture() || !IsDepthTexture()) return false;

    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, outData.GetArray());

    return true;
}