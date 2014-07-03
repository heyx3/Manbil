#include "MTexture.h"

void MTexture::SetSettings(const TextureSettings & newSettings)
{
    settings = newSettings;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyAllSettings(hasMipmaps);
    }
}

void MTexture::SetMinFilterType(TextureSettings::FilteringTypes newFiltering)
{
    settings.MinFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyMinFilter(hasMipmaps);
    }
}
void MTexture::SetMagFilterType(TextureSettings::FilteringTypes newFiltering)
{
    settings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyMagFilter(hasMipmaps);
    }
}
void MTexture::SetFilterType(TextureSettings::FilteringTypes newFiltering)
{
    settings.MinFilter = newFiltering;
    settings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyFilter(hasMipmaps);
    }
}

void MTexture::SetHorzWrappingType(TextureSettings::WrappingTypes wrapping)
{
    settings.HorzWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyHorzWrapping();
    }
}
void MTexture::SetVertWrappingType(TextureSettings::WrappingTypes wrapping)
{
    settings.VertWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyVertWrapping();
    }
}
void MTexture::SetWrappingType(TextureSettings::WrappingTypes wrapping)
{
    settings.HorzWrap = wrapping;
    settings.VertWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settings.ApplyWrapping();
    }
}

void MTexture::GenerateMipmaps(void)
{
    if (IsValidTexture())
    {
        hasMipmaps = true;
        Bind();
        TextureSettings::GenerateMipmaps();
        settings.ApplyFilter(true);
    }
}

void MTexture::Create(void)
{
    DeleteIfValid();
    RenderDataHandler::CreateTexture2D(texHandle);
    tWidth = 0;
    tHeight = 0;

    Bind();
    settings.ApplyAllSettings(false);
}
bool MTexture::Create(std::string filePath)
{
    //Load the file using SFML.
    sf::Texture tex;
    if (!tex.loadFromFile(filePath))
    {
        return false;
    }

    //Copy the texture data into this texture.
    Create();
    Array2D<Vector4f> texCol(0, 0);
    TextureConverters::ToArray(tex, texCol);
    SetData(texCol);
}

//If this is a valid texture, deletes it from OpenGL.
//Also resets the mipmap settings to "no mipmaps".
void MTexture::DeleteIfValid(void)
{
    if (IsValidTexture())
    {
        RenderDataHandler::DeleteTexture2D(texHandle);
    }
    texHandle = 0;
    hasMipmaps = false;
}

void MTexture::SetData(const Array2D<Vector4b> & inColor)
{
    tWidth = inColor.GetWidth();
    tHeight = inColor.GetHeight();
    TextureConverters::ToTexture(inColor, texHandle);
    if (hasMipmaps) GenerateMipmaps();
}
void MTexture::SetData(const Array2D<Vector4f> & inColor)
{
    tWidth = inColor.GetWidth();
    tHeight = inColor.GetHeight();
    TextureConverters::ToTexture(inColor, texHandle);
    if (hasMipmaps) GenerateMipmaps();
}
void MTexture::SetData(const Vector4b color, unsigned int width, unsigned int height)
{
    tWidth = width;
    tHeight = height;
    RenderDataHandler::SetTexture2DDataColor(texHandle, Vector2i((int)tWidth, (int)tHeight), color);
    if (hasMipmaps) GenerateMipmaps();
}
void MTexture::SetData(const Vector4f color, unsigned int width, unsigned int height)
{
    tWidth = width;
    tHeight = height;
    RenderDataHandler::SetTexture2DDataColor(texHandle, Vector2i((int)tWidth, (int)tHeight), color);
    if (hasMipmaps) GenerateMipmaps();
}