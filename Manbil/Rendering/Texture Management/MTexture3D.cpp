#include "MTexture3D.h"


void MTexture3D::SetSettings(const TextureSampleSettings3D& newSettings)
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

void MTexture3D::Create(const TextureSampleSettings3D& texSettings,
                        bool useMipmaps, PixelSizes _pixelSize)
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

    glTexImage3D(GL_TEXTURE_3D, 0, ToGLenum(pixelSize), 0, 0, 0, 0,
                 GetCPUFormat(), GetComponentType(), 0);
    if (usesMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_3D);
    }
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
    if (!IsValidTexture())
    {
        return;
    }

    Bind();

    width = newW;
    height = newH;
    depth = newD;

    glTexImage3D(GL_TEXTURE_3D, 0, ToGLenum(pixelSize), width, height, depth, 0,
                 GetCPUFormat(), GetComponentType(), 0);
    if (usesMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_3D);
    }
}

bool MTexture3D::SetColorData(const Array3D<Vector4b>& pixelData, PixelSizes newSize)
{
    if (!IsValidTexture())
    {
        return false;
    }
    if (IsPixelSizeColor(newSize))
    {
        pixelSize = newSize;
    }
    if (!IsColorTexture())
    {
        return false;
    }

    width = pixelData.GetWidth();
    height = pixelData.GetHeight();
    depth = pixelData.GetDepth();

    Bind();
    glTexImage3D(GL_TEXTURE_3D, 0, ToGLenum(pixelSize), width, height, depth, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixelData.GetArray());
    if (usesMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_3D);
    }

    return true;
}
bool MTexture3D::SetColorData(const Array3D<Vector4f>& pixelData, PixelSizes newSize)
{
    if (!IsValidTexture())
    {
        return false;
    }
    if (IsPixelSizeColor(newSize))
    {
        pixelSize = newSize;
    }
    if (!IsColorTexture())
    {
        return false;
    }

    width = pixelData.GetWidth();
    height = pixelData.GetHeight();
    depth = pixelData.GetDepth();

    Bind();
    glTexImage3D(GL_TEXTURE_3D, 0, ToGLenum(pixelSize), width, height, depth, 0,
                 GL_RGBA, GL_FLOAT, pixelData.GetArray());
    if (usesMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_3D);
    }

    return true;
}
bool MTexture3D::UpdateColorData(const Array3D<Vector4b>& pixelData,
                                 unsigned int offX, unsigned int offY, unsigned int offZ)
{
    if (offX + pixelData.GetWidth() > width ||
        offY + pixelData.GetHeight() > height ||
        offZ + pixelData.GetDepth() > depth)
    {
        return false;
    }

    Bind();
    glTexSubImage3D(GL_TEXTURE_3D, 0, offX, offY, offZ,
                    pixelData.GetWidth(), pixelData.GetHeight(), pixelData.GetDepth(),
                    GL_RGBA, GL_UNSIGNED_BYTE, pixelData.GetArray());
    if (usesMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_3D);
    }

    return true;
}
bool MTexture3D::UpdateColorData(const Array3D<Vector4f>& pixelData,
                                 unsigned int offX, unsigned int offY, unsigned int offZ)
{
    if (offX + pixelData.GetWidth() > width ||
        offY + pixelData.GetHeight() > height ||
        offZ + pixelData.GetDepth() > depth)
    {
        return false;
    }

    Bind();
    glTexSubImage3D(GL_TEXTURE_3D, 0, offX, offY, offZ,
                    pixelData.GetWidth(), pixelData.GetHeight(), pixelData.GetDepth(),
                    GL_RGBA, GL_FLOAT, pixelData.GetArray());
    if (usesMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_3D);
    }

    return true;
}

bool MTexture3D::SetGreyscaleData(const Array3D<unsigned char>& greyscaleData, PixelSizes newSize)
{
    if (!IsValidTexture())
    {
        return false;
    }
    if (IsPixelSizeGreyscale(newSize))
    {
        pixelSize = newSize;
    }
    if (!IsGreyscaleTexture())
    {
        return false;
    }

    width = greyscaleData.GetWidth();
    height = greyscaleData.GetHeight();
    depth = greyscaleData.GetDepth();

    Bind();
    glTexImage3D(GL_TEXTURE_3D, 0, ToGLenum(pixelSize), width, height, depth, 0,
                 GL_RED, GL_UNSIGNED_BYTE, greyscaleData.GetArray());
    if (usesMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_3D);
    }

    return true;
}
bool MTexture3D::SetGreyscaleData(const Array3D<float>& greyscaleData, PixelSizes newSize)
{
    if (!IsValidTexture())
    {
        return false;
    }
    if (IsPixelSizeGreyscale(newSize))
    {
        pixelSize = newSize;
    }
    if (!IsGreyscaleTexture())
    {
        return false;
    }

    width = greyscaleData.GetWidth();
    height = greyscaleData.GetHeight();
    depth = greyscaleData.GetDepth();

    Bind();
    glTexImage3D(GL_TEXTURE_3D, 0, ToGLenum(pixelSize), width, height, depth, 0,
                 GL_RED, GL_FLOAT, greyscaleData.GetArray());
    if (usesMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_3D);
    }

    return true;
}
bool MTexture3D::UpdateGreyscaleData(const Array3D<unsigned char>& pixelData,
                                     unsigned int offX, unsigned int offY, unsigned int offZ)
{
    if (offX + pixelData.GetWidth() > width ||
        offY + pixelData.GetHeight() > height ||
        offZ + pixelData.GetDepth() > depth)
    {
        return false;
    }

    Bind();
    glTexSubImage3D(GL_TEXTURE_3D, 0, offX, offY, offZ,
                    pixelData.GetWidth(), pixelData.GetHeight(), pixelData.GetDepth(),
                    GL_RED, GL_UNSIGNED_BYTE, pixelData.GetArray());
    if (usesMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_3D);
    }

    return true;
}
bool MTexture3D::UpdateGreyscaleData(const Array3D<float>& pixelData,
                                     unsigned int offX, unsigned int offY, unsigned int offZ)
{
    if (offX + pixelData.GetWidth() > width ||
        offY + pixelData.GetHeight() > height ||
        offZ + pixelData.GetDepth() > depth)
    {
        return false;
    }

    Bind();
    glTexSubImage3D(GL_TEXTURE_3D, 0, offX, offY, offZ,
                    pixelData.GetWidth(), pixelData.GetHeight(), pixelData.GetDepth(),
                    GL_RED, GL_FLOAT, pixelData.GetArray());
    if (usesMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_3D);
    }

    return true;
}

bool MTexture3D::SetDepthData(const Array3D<unsigned char>& depthData, PixelSizes newSize)
{
    if (!IsValidTexture())
    {
        return false;
    }
    if (IsPixelSizeDepth(newSize))
    {
        pixelSize = newSize;
    }
    if (!IsDepthTexture())
    {
        return false;
    }

    Bind();
    width = depthData.GetWidth();
    height = depthData.GetHeight();
    depth = depthData.GetDepth();
    glTexImage3D(GL_TEXTURE_3D, 0, ToGLenum(pixelSize), width, height, depth, 0,
                 GL_UNSIGNED_BYTE, GL_DEPTH_COMPONENT, depthData.GetArray());
    if (usesMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_3D);
    }

    return true;
}
bool MTexture3D::SetDepthData(const Array3D<float>& depthData, PixelSizes newSize)
{
    if (!IsValidTexture())
    {
        return false;
    }
    if (IsPixelSizeDepth(newSize))
    {
        pixelSize = newSize;
    }
    if (!IsDepthTexture())
    {
        return false;
    }

    Bind();
    width = depthData.GetWidth();
    height = depthData.GetHeight();
    depth = depthData.GetDepth();
    glTexImage3D(GL_TEXTURE_3D, 0, ToGLenum(pixelSize), width, height, depth, 0,
                 GL_FLOAT, GL_DEPTH_COMPONENT, depthData.GetArray());
    if (usesMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_3D);
    }

    return true;
}
bool MTexture3D::UpdateDepthData(const Array3D<unsigned char>& pixelData,
                                 unsigned int offX, unsigned int offY, unsigned int offZ)
{
    if (offX + pixelData.GetWidth() > width ||
        offY + pixelData.GetHeight() > height ||
        offZ + pixelData.GetDepth() > depth)
    {
        return false;
    }

    Bind();
    glTexSubImage3D(GL_TEXTURE_3D, 0, offX, offY, offZ,
                    pixelData.GetWidth(), pixelData.GetHeight(), pixelData.GetDepth(),
                    GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, pixelData.GetArray());
    if (usesMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_3D);
    }

    return true;
}
bool MTexture3D::UpdateDepthData(const Array3D<float>& pixelData,
                                 unsigned int offX, unsigned int offY, unsigned int offZ)
{
    if (offX + pixelData.GetWidth() > width ||
        offY + pixelData.GetHeight() > height ||
        offZ + pixelData.GetDepth() > depth)
    {
        return false;
    }

    Bind();
    glTexSubImage3D(GL_TEXTURE_3D, 0, offX, offY, offZ,
                    pixelData.GetWidth(), pixelData.GetHeight(), pixelData.GetDepth(),
                    GL_DEPTH_COMPONENT, GL_FLOAT, pixelData.GetArray());
    if (usesMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_3D);
    }

    return true;
}


bool MTexture3D::GetColorData(Array3D<Vector4b>& outData) const
{
    if (!IsValidTexture() || !IsColorTexture() ||
        outData.GetWidth() != width || outData.GetHeight() != height || outData.GetDepth() != depth)
    {
        return false;
    }

    Bind();
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_UNSIGNED_BYTE, outData.GetArray());

    return true;
}
bool MTexture3D::GetColorData(Array3D<Vector4f>& outData) const
{
    if (!IsValidTexture() || !IsColorTexture() ||
        outData.GetWidth() != width || outData.GetHeight() != height || outData.GetDepth() != depth)
    {
        return false;
    }

    Bind();
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_FLOAT, outData.GetArray());

    return true;
}

bool MTexture3D::GetGreyscaleData(Array3D<unsigned char>& outData) const
{
    if (!IsValidTexture() || !IsColorTexture() ||
        outData.GetWidth() != width || outData.GetHeight() != height || outData.GetDepth() != depth)
    {
        return false;
    }

    Bind();
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_UNSIGNED_BYTE, outData.GetArray());

    return true;
}
bool MTexture3D::GetGreyscaleData(Array3D<float>& outData) const
{
    if (!IsValidTexture() || !IsColorTexture() ||
        outData.GetWidth() != width || outData.GetHeight() != height || outData.GetDepth() != depth)
    {
        return false;
    }

    Bind();
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_FLOAT, outData.GetArray());

    return true;
}

bool MTexture3D::GetDepthData(Array3D<unsigned char>& outData) const
{
    if (!IsValidTexture() || !IsDepthTexture() ||
        outData.GetWidth() != width || outData.GetHeight() != height || outData.GetDepth() != depth)
    {
        return false;
    }

    Bind();
    glGetTexImage(GL_TEXTURE_3D, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, outData.GetArray());

    return true;
}
bool MTexture3D::GetDepthData(Array3D<float>& outData) const
{
    if (!IsValidTexture() || !IsDepthTexture() ||
        outData.GetWidth() != width || outData.GetHeight() != height || outData.GetDepth() != depth)
    {
        return false;
    }

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