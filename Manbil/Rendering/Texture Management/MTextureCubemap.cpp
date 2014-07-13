#include "MTextureCubemap.h"


void MTextureCubemap::SetSettings(const TextureSampleSettings & newSettings)
{
    settings = newSettings;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyAllSettings(TextureTypes::TT_CUBE, UsesMipmaps());
    }
}

void MTextureCubemap::SetMinFilterType(TextureSampleSettings::FilteringTypes newFiltering)
{
    settings.MinFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyMinFilter(TextureTypes::TT_CUBE, UsesMipmaps());
    }
}
void MTextureCubemap::SetMagFilterType(TextureSampleSettings::FilteringTypes newFiltering)
{
    settings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyMagFilter(TextureTypes::TT_CUBE, UsesMipmaps());
    }
}
void MTextureCubemap::SetFilterType(TextureSampleSettings::FilteringTypes newFiltering)
{
    settings.MinFilter = newFiltering;
    settings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyFilter(TextureTypes::TT_CUBE, UsesMipmaps());
    }
}

void MTextureCubemap::SetHorzWrappingType(TextureSampleSettings::WrappingTypes wrapping)
{
    settings.HorzWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyHorzWrapping(TextureTypes::TT_CUBE);
    }
}
void MTextureCubemap::SetVertWrappingType(TextureSampleSettings::WrappingTypes wrapping)
{
    settings.VertWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyVertWrapping(TextureTypes::TT_CUBE);
    }
}
void MTextureCubemap::SetWrappingType(TextureSampleSettings::WrappingTypes wrapping)
{
    settings.HorzWrap = wrapping;
    settings.VertWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyWrapping(TextureTypes::TT_CUBE);
    }
}


void MTextureCubemap::Create(const TextureSampleSettings & texSettings, bool useMipmaps, PixelSizes _pixelSize)
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

bool MTextureCubemap::SetDataFromFile(CubeTextureTypes face, std::string filePath, bool shouldUpdateMipmaps)
{
    if (!IsValidTexture()) return false;

    sf::Image img;
    if (!img.loadFromFile(filePath)) return false;

    Bind();
    width = img.getSize().x;
    height = img.getSize().y;
    glTexImage2D(TextureTypeToGLEnum(face), 0, ToGLenum(pixelSize), width, height, 0, GL_UNSIGNED_BYTE, GL_RGBA, img.getPixelsPtr());
    if (usesMipmaps && shouldUpdateMipmaps)
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    return true;
}

bool MTextureCubemap::SetData(Array2D<Vector4b> & negXData, Array2D<Vector4b> & negYData, Array2D<Vector4b> & negZData,
                              Array2D<Vector4b> & posXData, Array2D<Vector4b> & posYData, Array2D<Vector4b> & posZData,
                              bool useMipmaps, PixelSizes _pixelSize)
{
    if (!IsValidTexture()) return false;

    if (!negXData.HasSameDimensions(negYData) || !negXData.HasSameDimensions(negZData) ||
        !negXData.HasSameDimensions(posXData) || !negXData.HasSameDimensions(posYData) ||
        !negXData.HasSameDimensions(posZData))
    {
        return false;
    }

    if (!IsPixelSizeDepth(pixelSize)) pixelSize = _pixelSize;

    width = negXData.GetWidth();
    height = negXData.GetHeight();
    usesMipmaps = useMipmaps;

    Bind();
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, ToGLenum(pixelSize),
                 width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, posXData.GetArray());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, ToGLenum(pixelSize),
                 width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, posYData.GetArray());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, ToGLenum(pixelSize),
                 width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, posZData.GetArray());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, ToGLenum(pixelSize),
                 width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, negXData.GetArray());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, ToGLenum(pixelSize),
                 width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, negYData.GetArray());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, ToGLenum(pixelSize),
                 width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, negZData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    return true;
}
bool MTextureCubemap::SetData(Array2D<Vector4f> & negXData, Array2D<Vector4f> & negYData, Array2D<Vector4f> & negZData,
                              Array2D<Vector4f> & posXData, Array2D<Vector4f> & posYData, Array2D<Vector4f> & posZData,
                              bool useMipmaps, PixelSizes _pixelSize)
{
    if (!IsValidTexture()) return false;

    if (!negXData.HasSameDimensions(negYData) || !negXData.HasSameDimensions(negZData) ||
        !negXData.HasSameDimensions(posXData) || !negXData.HasSameDimensions(posYData) ||
        !negXData.HasSameDimensions(posZData))
    {
        return false;
    }

    if (!IsPixelSizeDepth(pixelSize)) pixelSize = _pixelSize;

    width = negXData.GetWidth();
    height = negXData.GetHeight();
    usesMipmaps = useMipmaps;

    Bind();
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, ToGLenum(pixelSize),
                 width, height, 0, GL_RGBA, GL_FLOAT, posXData.GetArray());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, ToGLenum(pixelSize),
                 width, height, 0, GL_RGBA, GL_FLOAT, posYData.GetArray());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, ToGLenum(pixelSize),
                 width, height, 0, GL_RGBA, GL_FLOAT, posZData.GetArray());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, ToGLenum(pixelSize),
                 width, height, 0, GL_RGBA, GL_FLOAT, negXData.GetArray());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, ToGLenum(pixelSize),
                 width, height, 0, GL_RGBA, GL_FLOAT, negYData.GetArray());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, ToGLenum(pixelSize),
                 width, height, 0, GL_RGBA, GL_FLOAT, negZData.GetArray());
    if (usesMipmaps) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    return true;
}

bool MTextureCubemap::SetData(CubeTextureTypes face, const Array2D<Vector4b> & pixelData, bool shouldUpdateMipmaps)
{
    if (pixelData.GetWidth() != width || pixelData.GetHeight() != height)
        return false;

    Bind();
    width = pixelData.GetWidth();
    height = pixelData.GetHeight();
    glTexImage2D(TextureTypeToGLEnum(face), 0, ToGLenum(pixelSize), width, height, 0, GL_UNSIGNED_BYTE, GL_RGBA, pixelData.GetArray());
    if (usesMipmaps && shouldUpdateMipmaps)
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    return true;
}
bool MTextureCubemap::SetData(CubeTextureTypes face, const Array2D<Vector4f> & pixelData, bool shouldUpdateMipmaps)
{
    if (pixelData.GetWidth() != width || pixelData.GetHeight() != height)
        return false;

    Bind();
    width = pixelData.GetWidth();
    height = pixelData.GetHeight();
    glTexImage2D(TextureTypeToGLEnum(face), 0, ToGLenum(pixelSize), width, height, 0, GL_FLOAT, GL_RGBA, pixelData.GetArray());
    if (usesMipmaps && shouldUpdateMipmaps)
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    return true;
}


void MTextureCubemap::GetData(CubeTextureTypes face, Array2D<Vector4b> & outData)
{
    Bind();
    glGetTexImage(TextureTypeToGLEnum(face), 0, GL_RGBA, GL_UNSIGNED_BYTE, outData.GetArray());
}
void MTextureCubemap::GetData(CubeTextureTypes face, Array2D<Vector4f> & outData)
{
    Bind();
    glGetTexImage(TextureTypeToGLEnum(face), 0, GL_RGBA, GL_FLOAT, outData.GetArray());
}