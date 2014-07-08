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

bool MTexture::Create(const ColorTextureSettings & texSettings, Texture2DInitialization & initer)
{
    DeleteIfValid();

    settings = texSettings;
    Array2D<Vector4f> pixels(1, 1);
    if (!initer.MakeTexture(settings, pixels))
    {
        return false;
    }

    settings.Width = pixels.GetWidth();
    settings.Height = pixels.GetHeight();
    RenderDataHandler::CreateTextureFromFloats(texHandle, settings, [&pixels](Vector2i loc, Vector4f * outCol) { *outCol = pixels[loc]; });
    
    return true;
}


void MTexture::DeleteIfValid(void)
{
    if (IsValidTexture())
    {
        RenderDataHandler::DeleteTexture(texHandle);
    }
    texHandle = 0;
}


bool MTexture::SetData(Texture2DInitialization & init)
{
    Array2D<Vector4f> pixels(1, 1);
    if (!init.MakeTexture(settings, pixels))
    {
        return false;
    }

    settings.Width = pixels.GetWidth();
    settings.Height = pixels.GetHeight();
    TextureConverters::ToTexture(pixels, settings, texHandle);

    return true;
}
bool MTexture::SetData(unsigned int width, unsigned int height, Texture2DInitialization & init)
{
    settings.Width = width;
    settings.Height = height;
    return SetData(init);
}