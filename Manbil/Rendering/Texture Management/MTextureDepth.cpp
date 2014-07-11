#include "MTextureDepth.h"


void MTextureDepth::SetSettings(const TextureSettings & newSettings)
{
    settings.BaseSettings = newSettings;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyAllSettings(UsesMipmaps());
    }
}

void MTextureDepth::SetMinFilterType(TextureSettings::FilteringTypes newFiltering)
{
    settings.BaseSettings.MinFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyMinFilter(UsesMipmaps());
    }
}
void MTextureDepth::SetMagFilterType(TextureSettings::FilteringTypes newFiltering)
{
    settings.BaseSettings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyMagFilter(UsesMipmaps());
    }
}
void MTextureDepth::SetFilterType(TextureSettings::FilteringTypes newFiltering)
{
    settings.BaseSettings.MinFilter = newFiltering;
    settings.BaseSettings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyFilter(UsesMipmaps());
    }
}

void MTextureDepth::SetHorzWrappingType(TextureSettings::WrappingTypes wrapping)
{
    settings.BaseSettings.HorzWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyHorzWrapping();
    }
}
void MTextureDepth::SetVertWrappingType(TextureSettings::WrappingTypes wrapping)
{
    settings.BaseSettings.VertWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyVertWrapping();
    }
}
void MTextureDepth::SetWrappingType(TextureSettings::WrappingTypes wrapping)
{
    settings.BaseSettings.HorzWrap = wrapping;
    settings.BaseSettings.VertWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.BaseSettings.ApplyWrapping();
    }
}
void MTextureDepth::Create(const DepthTextureSettings & texSettings)
{
    DeleteIfValid();

    settings = texSettings;
    RenderDataHandler::CreateDepthTexture(texHandle, texSettings);
}

void MTextureDepth::DeleteIfValid(void)
{
    if (IsValidTexture())
    {
        RenderDataHandler::DeleteTexture(texHandle);
    }
    texHandle = 0;
}

void MTextureDepth::SetData(const Array2D<unsigned char> & pixelData)
{
    Bind();
    settings.Width = pixelData.GetWidth();
    settings.Height = pixelData.GetHeight();
    RenderDataHandler::SetDepthTexture(settings.GenerateMipmaps, settings.PixelSize, pixelData);
}
void MTextureDepth::SetData(const Array2D<float> & pixelData)
{
    Bind();
    settings.Width = pixelData.GetWidth();
    settings.Height = pixelData.GetHeight();
    RenderDataHandler::SetDepthTexture(settings.GenerateMipmaps, settings.PixelSize, pixelData);
}

void MTextureDepth::GetData(Array2D<unsigned char> & outData)
{
    outData.Reset(settings.Width, settings.Height);
    Bind();
    RenderDataHandler::GetDepthTextureData(outData);
}
void MTextureDepth::GetData(Array2D<float> & outData)
{
    outData.Reset(settings.Width, settings.Height);
    Bind();
    RenderDataHandler::GetDepthTextureData(outData);
}