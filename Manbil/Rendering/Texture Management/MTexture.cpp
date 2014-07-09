#include "MTexture.h"


void MTexture::SetSettings(const TextureSettings & newSettings)
{
    settings.BaseSettings = newSettings;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyAllSettings(UsesMipmaps());
    }
}

void MTexture::SetMinFilterType(TextureSettings::FilteringTypes newFiltering)
{
    settings.BaseSettings.MinFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyMinFilter(UsesMipmaps());
    }
}
void MTexture::SetMagFilterType(TextureSettings::FilteringTypes newFiltering)
{
    settings.BaseSettings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyMagFilter(UsesMipmaps());
    }
}
void MTexture::SetFilterType(TextureSettings::FilteringTypes newFiltering)
{
    settings.BaseSettings.MinFilter = newFiltering;
    settings.BaseSettings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyFilter(UsesMipmaps());
    }
}

void MTexture::SetHorzWrappingType(TextureSettings::WrappingTypes wrapping)
{
    settings.BaseSettings.HorzWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyHorzWrapping();
    }
}
void MTexture::SetVertWrappingType(TextureSettings::WrappingTypes wrapping)
{
    settings.BaseSettings.VertWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyVertWrapping();
    }
}
void MTexture::SetWrappingType(TextureSettings::WrappingTypes wrapping)
{
    settings.BaseSettings.HorzWrap = wrapping;
    settings.BaseSettings.VertWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyWrapping();
    }
}
void MTexture::Create(const ColorTextureSettings & texSettings, const Array2D<Vector4b> & pixels)
{
    DeleteIfValid();

    settings = texSettings;
    settings.Width = pixels.GetWidth();
    settings.Height = pixels.GetHeight();
    RenderDataHandler::CreateTexture2D(texHandle, settings.GenerateMipmaps, settings.PixelSize, settings.BaseSettings, pixels);
}
void MTexture::Create(const ColorTextureSettings & texSettings, const Array2D<Vector4f> & pixels)
{
    DeleteIfValid();

    settings = texSettings;
    settings.Width = pixels.GetWidth();
    settings.Height = pixels.GetHeight();
    RenderDataHandler::CreateTexture2D(texHandle, settings.GenerateMipmaps, settings.PixelSize, settings.BaseSettings, pixels);
}

void MTexture::DeleteIfValid(void)
{
    if (IsValidTexture())
    {
        RenderDataHandler::DeleteTexture(texHandle);
    }
    texHandle = 0;
}

void MTexture::SetData(const Array2D<Vector4b> & pixelData)
{
    Bind();
    settings.Width = pixelData.GetWidth();
    settings.Height = pixelData.GetHeight();
    RenderDataHandler::SetTexture2D(settings.GenerateMipmaps, settings.PixelSize, settings.BaseSettings, pixelData);
}
void MTexture::SetData(const Array2D<Vector4f> & pixelData)
{
    Bind();
    settings.Width = pixelData.GetWidth();
    settings.Height = pixelData.GetHeight();
    RenderDataHandler::SetTexture2D(settings.GenerateMipmaps, settings.PixelSize, settings.BaseSettings, pixelData);
}

void MTexture::GetData(Array2D<Vector4b> & outData)
{
    outData.Reset(settings.Width, settings.Height);
    Bind();
    RenderDataHandler::GetTextureData(outData);
}
void MTexture::GetData(Array2D<Vector4f> & outData)
{
    outData.Reset(settings.Width, settings.Height);
    Bind();
    RenderDataHandler::GetTextureData(outData);
}