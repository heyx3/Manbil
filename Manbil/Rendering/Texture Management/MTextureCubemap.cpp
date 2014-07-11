#include "MTextureCubemap.h"



void MTextureCubemap::SetSettings(const TextureSettings & newSettings)
{
    settings.BaseSettings = newSettings;
    
    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyAllSettings(settings.GenerateMipmaps);
    }
}

void MTextureCubemap::SetMinFilterType(TextureSettings::FilteringTypes newFiltering)
{
    settings.BaseSettings.MinFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyMinFilter(settings.GenerateMipmaps);
    }
}
void MTextureCubemap::SetMagFilterType(TextureSettings::FilteringTypes newFiltering)
{
    settings.BaseSettings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyMagFilter(settings.GenerateMipmaps);
    }
}
void MTextureCubemap::SetFilterType(TextureSettings::FilteringTypes newFiltering)
{
    settings.BaseSettings.MinFilter = newFiltering;
    settings.BaseSettings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyFilter(settings.GenerateMipmaps);
    }
}

void MTextureCubemap::SetHorzWrappingType(TextureSettings::WrappingTypes wrapping)
{
    settings.BaseSettings.HorzWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyHorzWrapping();
    }
}
void MTextureCubemap::SetVertWrappingType(TextureSettings::WrappingTypes wrapping)
{
    settings.BaseSettings.VertWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyVertWrapping();
    }
}
void MTextureCubemap::SetWrappingType(TextureSettings::WrappingTypes wrapping)
{
    settings.BaseSettings.HorzWrap = wrapping;
    settings.BaseSettings.VertWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyWrapping();
    }
}


void MTextureCubemap::Create(const ColorTextureSettings & _settings,
                             const Array2D<Vector4f> & dataNegX, const Array2D<Vector4f> & dataNegY, const Array2D<Vector4f> & dataNegZ,
                             const Array2D<Vector4f> & dataPosX, const Array2D<Vector4f> & dataPosY, const Array2D<Vector4f> & dataPosZ)
{
    DeleteIfValid();
    RenderDataHandler::CreateTextureCubemap(texHandle);
    SetFaceData(_settings, dataNegX, dataNegY, dataNegZ, dataPosX, dataPosY, dataPosZ);
}
void MTextureCubemap::Create(const ColorTextureSettings & _settings,
                             const Array2D<Vector4b> & dataNegX, const Array2D<Vector4b> & dataNegY, const Array2D<Vector4b> & dataNegZ,
                             const Array2D<Vector4b> & dataPosX, const Array2D<Vector4b> & dataPosY, const Array2D<Vector4b> & dataPosZ)
{
    DeleteIfValid();
    RenderDataHandler::CreateTextureCubemap(texHandle);
    SetFaceData(_settings, dataNegX, dataNegY, dataNegZ, dataPosX, dataPosY, dataPosZ);
}

void MTextureCubemap::DeleteIfValid(void)
{
    if (texHandle != 0)
    {
        RenderDataHandler::DeleteTexture(texHandle);
        texHandle = 0;
    }
}


void MTextureCubemap::SetFaceData(const ColorTextureSettings & _settings,
                                  const Array2D<Vector4b> & dataNegX, const Array2D<Vector4b> & dataNegY, const Array2D<Vector4b> & dataNegZ,
                                  const Array2D<Vector4b> & dataPosX, const Array2D<Vector4b> & dataPosY, const Array2D<Vector4b> & dataPosZ)
{
    settings = _settings;
    settings.BaseSettings.TextureType = TextureTypes::TT_CUBE;


    assert(settings.Width == dataNegX.GetWidth() && settings.Height == dataNegX.GetHeight());
    assert(settings.Width == dataNegY.GetWidth() && settings.Height == dataNegY.GetHeight());
    assert(settings.Width == dataNegZ.GetWidth() && settings.Height == dataNegZ.GetHeight());
    assert(settings.Width == dataPosX.GetWidth() && settings.Height == dataPosX.GetHeight());
    assert(settings.Width == dataPosY.GetWidth() && settings.Height == dataPosY.GetHeight());
    assert(settings.Width == dataPosZ.GetWidth() && settings.Height == dataPosZ.GetHeight());


    Bind();
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_X_NEG, settings.PixelSize, dataNegX);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_Y_NEG, settings.PixelSize, dataNegY);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_Z_NEG, settings.PixelSize, dataNegZ);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_X_POS, settings.PixelSize, dataPosX);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_Y_POS, settings.PixelSize, dataPosY);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_Z_POS, settings.PixelSize, dataPosZ);


    if (settings.GenerateMipmaps)
        RenderDataHandler::GenerateTextureMipmaps(TextureTypes::TT_CUBE);
    settings.BaseSettings.ApplyAllSettings(settings.GenerateMipmaps);
}
void MTextureCubemap::SetFaceData(const ColorTextureSettings & _settings,
                                  const Array2D<Vector4f> & dataNegX, const Array2D<Vector4f> & dataNegY, const Array2D<Vector4f> & dataNegZ,
                                  const Array2D<Vector4f> & dataPosX, const Array2D<Vector4f> & dataPosY, const Array2D<Vector4f> & dataPosZ)
{
    settings = _settings;
    settings.BaseSettings.TextureType = TextureTypes::TT_CUBE;


    assert(settings.Width == dataNegX.GetWidth() && settings.Height == dataNegX.GetHeight());
    assert(settings.Width == dataNegY.GetWidth() && settings.Height == dataNegY.GetHeight());
    assert(settings.Width == dataNegZ.GetWidth() && settings.Height == dataNegZ.GetHeight());
    assert(settings.Width == dataPosX.GetWidth() && settings.Height == dataPosX.GetHeight());
    assert(settings.Width == dataPosY.GetWidth() && settings.Height == dataPosY.GetHeight());
    assert(settings.Width == dataPosZ.GetWidth() && settings.Height == dataPosZ.GetHeight());


    Bind();
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_X_NEG, settings.PixelSize, dataNegX);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_Y_NEG, settings.PixelSize, dataNegY);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_Z_NEG, settings.PixelSize, dataNegZ);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_X_POS, settings.PixelSize, dataPosX);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_Y_POS, settings.PixelSize, dataPosY);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_Z_POS, settings.PixelSize, dataPosZ);


    if (settings.GenerateMipmaps)
        RenderDataHandler::GenerateTextureMipmaps(TextureTypes::TT_CUBE);
    settings.BaseSettings.ApplyAllSettings(settings.GenerateMipmaps);
}

void MTextureCubemap::SetFaceData(CubeTextureTypes face, const Array2D<Vector4f> & pixels)
{
    assert(settings.Width == pixels.GetWidth() && settings.Height == pixels.GetHeight());

    Bind();
    RenderDataHandler::SetTextureCubemapFace(face, settings.PixelSize, pixels);
}
void MTextureCubemap::SetFaceData(CubeTextureTypes face, const Array2D<Vector4b> & pixels)
{
    assert(settings.Width == pixels.GetWidth() && settings.Height == pixels.GetHeight());

    Bind();
    RenderDataHandler::SetTextureCubemapFace(face, settings.PixelSize, pixels);
}

void MTextureCubemap::GetFaceData(CubeTextureTypes face, Array2D<Vector4b> & outData)
{
    Bind();
    RenderDataHandler::GetTextureData(face, outData);
}
void MTextureCubemap::GetFaceData(CubeTextureTypes face, Array2D<Vector4f> & outData)
{
    Bind();
    RenderDataHandler::GetTextureData(face, outData);
}