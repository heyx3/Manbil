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

void MTexture::Create(const ColorTextureSettings & texSettings)
{
    DeleteIfValid();

    settings = texSettings;
    RenderDataHandler::CreateTextureFromBytes(texHandle, texSettings, [](Vector2i loc, Vector4b * outCol) { *outCol = Vector4b((unsigned char)255, 255, 255, 255); });
}
bool MTexture::Create(std::string filePath, const ColorTextureSettings & texSettings)
{
    //Create the new empty texture.
    Create(texSettings);

    //Load the texture file using SFML.
    sf::Texture tex;
    if (!tex.loadFromFile(filePath))
    {
        return false;
    }

    //Get the pixel data from the SFML texture.
    Array2D<Vector4f> texCol(0, 0);
    TextureConverters::ToArray(tex, texCol);

    //Put the pixel data into the new texture.
    SetData(texCol);

    return true;
}


void MTexture::DeleteIfValid(void)
{
    if (IsValidTexture())
    {
        RenderDataHandler::DeleteTexture2D(texHandle);
    }
    texHandle = 0;
}

void MTexture::SetData(const Array2D<Vector4b> & inColor)
{
    settings.Width = inColor.GetWidth();
    settings.Height = inColor.GetHeight();
    TextureConverters::ToTexture(inColor, settings, texHandle);
}
void MTexture::SetData(const Array2D<Vector4f> & inColor)
{
    settings.Width = inColor.GetWidth();
    settings.Height = inColor.GetHeight();
    TextureConverters::ToTexture(inColor, settings, texHandle);
}
void MTexture::SetData(const Vector4b color, unsigned int width, unsigned int height)
{
    settings.Width = width;
    settings.Height = height;
    RenderDataHandler::BindTexture(TextureTypes::TT_2D, texHandle);
    RenderDataHandler::SetTextureFromByteFunc(settings, [color](Vector2i loc, Vector4b * outCol) { *outCol = color; });
}
void MTexture::SetData(const Vector4f color, unsigned int width, unsigned int height)
{
    settings.Width = width;
    settings.Height = height;
    RenderDataHandler::BindTexture(TextureTypes::TT_2D, texHandle);
    RenderDataHandler::SetTextureFromFloatFunc(settings, [color](Vector2i loc, Vector4f * outCol) { *outCol = color; });
}