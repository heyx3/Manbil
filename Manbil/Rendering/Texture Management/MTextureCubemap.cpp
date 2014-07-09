#include "MTextureCubemap.h"


const unsigned int MTextureCubemap::NEG_X = 0,
                   MTextureCubemap::NEG_Y = 1,
                   MTextureCubemap::NEG_Z = 2,
                   MTextureCubemap::POS_X = 3,
                   MTextureCubemap::POS_Y = 4,
                   MTextureCubemap::POS_Z = 5;

unsigned int MTextureCubemap::GetIndex(CubeTextureTypes faceType)
{
    switch (faceType)
    {
        case CubeTextureTypes::CTT_X_NEG: return NEG_X;
        case CubeTextureTypes::CTT_Y_NEG: return NEG_Y;
        case CubeTextureTypes::CTT_Z_NEG: return NEG_Z;
        case CubeTextureTypes::CTT_X_POS: return POS_X;
        case CubeTextureTypes::CTT_Y_POS: return POS_Y;
        case CubeTextureTypes::CTT_Z_POS: return POS_Z;

        default: assert(false); return 6;
    }
}


void MTextureCubemap::SetSettings(CubeTextureTypes face, const TextureSettings & newSettings)
{
    ColorTextureSettings & settingsRef = settings[GetIndex(face)];
    settingsRef.BaseSettings = newSettings;
    
    if (IsValidTexture())
    {
        Bind();
        settingsRef.BaseSettings.ApplyAllSettings(settingsRef.GenerateMipmaps);
    }
}

void MTextureCubemap::SetMinFilterType(CubeTextureTypes face, TextureSettings::FilteringTypes newFiltering)
{
    ColorTextureSettings & settingsRef = settings[GetIndex(face)];
    settingsRef.BaseSettings.MinFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settingsRef.BaseSettings.ApplyMinFilter(settingsRef.GenerateMipmaps);
    }
}
void MTextureCubemap::SetMagFilterType(CubeTextureTypes face, TextureSettings::FilteringTypes newFiltering)
{
    ColorTextureSettings & settingsRef = settings[GetIndex(face)];
    settingsRef.BaseSettings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settingsRef.BaseSettings.ApplyMagFilter(settingsRef.GenerateMipmaps);
    }
}
void MTextureCubemap::SetFilterType(CubeTextureTypes face, TextureSettings::FilteringTypes newFiltering)
{
    ColorTextureSettings & settingsRef = settings[GetIndex(face)];
    settingsRef.BaseSettings.MinFilter = newFiltering;
    settingsRef.BaseSettings.MagFilter = newFiltering;

    if (IsValidTexture())
    {
        Bind();
        settingsRef.BaseSettings.ApplyFilter(settingsRef.GenerateMipmaps);
    }
}

void MTextureCubemap::SetHorzWrappingType(CubeTextureTypes face, TextureSettings::WrappingTypes wrapping)
{
    ColorTextureSettings & settingsRef = settings[GetIndex(face)];
    settingsRef.BaseSettings.HorzWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settingsRef.BaseSettings.ApplyHorzWrapping();
    }
}
void MTextureCubemap::SetVertWrappingType(CubeTextureTypes face, TextureSettings::WrappingTypes wrapping)
{
    ColorTextureSettings & settingsRef = settings[GetIndex(face)];
    settingsRef.BaseSettings.VertWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settingsRef.BaseSettings.ApplyVertWrapping();
    }
}
void MTextureCubemap::SetWrappingType(CubeTextureTypes face, TextureSettings::WrappingTypes wrapping)
{
    ColorTextureSettings & settingsRef = settings[GetIndex(face)];
    settingsRef.BaseSettings.HorzWrap = wrapping;
    settingsRef.BaseSettings.VertWrap = wrapping;

    if (IsValidTexture())
    {
        Bind();
        settingsRef.BaseSettings.ApplyWrapping();
    }
}


void MTextureCubemap::Create(const ColorTextureSettings & settingsNegX, const Array2D<Vector4f> & dataNegX,
                             const ColorTextureSettings & settingsNegY, const Array2D<Vector4f> & dataNegY,
                             const ColorTextureSettings & settingsNegZ, const Array2D<Vector4f> & dataNegZ,
                             const ColorTextureSettings & settingsPosX, const Array2D<Vector4f> & dataPosX,
                             const ColorTextureSettings & settingsPosY, const Array2D<Vector4f> & dataPosY,
                             const ColorTextureSettings & settingsPosZ, const Array2D<Vector4f> & dataPosZ)
{
    DeleteIfValid();

    settings[NEG_X] = settingsNegX;
    settings[NEG_Y] = settingsNegY;
    settings[NEG_Z] = settingsNegZ;
    settings[POS_X] = settingsPosX;
    settings[POS_Y] = settingsPosY;
    settings[POS_Z] = settingsPosZ;

    settings[NEG_X].Width = dataNegX.GetWidth();
    settings[NEG_X].Height = dataNegX.GetHeight();
    settings[NEG_Y].Width = dataNegY.GetWidth();
    settings[NEG_Y].Height = dataNegY.GetHeight();
    settings[NEG_Z].Width = dataNegZ.GetWidth();
    settings[NEG_Z].Height = dataNegZ.GetHeight();
    settings[POS_X].Width = dataPosX.GetWidth();
    settings[POS_X].Height = dataPosX.GetHeight();
    settings[POS_Y].Width = dataPosY.GetWidth();
    settings[POS_Y].Height = dataPosY.GetHeight();
    settings[POS_Z].Width = dataPosZ.GetWidth();
    settings[POS_Z].Height = dataPosZ.GetHeight();

    RenderDataHandler::CreateTextureCubemap(texHandle);
    Bind();
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_X_NEG, settings[NEG_X], &dataNegX.GetArray()[0].x);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_Y_NEG, settings[NEG_Y], &dataNegY.GetArray()[0].x);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_Z_NEG, settings[NEG_Z], &dataNegZ.GetArray()[0].x);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_X_POS, settings[POS_X], &dataPosX.GetArray()[0].x);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_Y_POS, settings[POS_Y], &dataPosY.GetArray()[0].x);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_Z_POS, settings[POS_Z], &dataPosZ.GetArray()[0].x);
}
void MTextureCubemap::Create(const ColorTextureSettings & settingsNegX, const Array2D<Vector4b> & dataNegX,
                             const ColorTextureSettings & settingsNegY, const Array2D<Vector4b> & dataNegY,
                             const ColorTextureSettings & settingsNegZ, const Array2D<Vector4b> & dataNegZ,
                             const ColorTextureSettings & settingsPosX, const Array2D<Vector4b> & dataPosX,
                             const ColorTextureSettings & settingsPosY, const Array2D<Vector4b> & dataPosY,
                             const ColorTextureSettings & settingsPosZ, const Array2D<Vector4b> & dataPosZ)
{
    DeleteIfValid();

    settings[NEG_X] = settingsNegX;
    settings[NEG_Y] = settingsNegY;
    settings[NEG_Z] = settingsNegZ;
    settings[POS_X] = settingsPosX;
    settings[POS_Y] = settingsPosY;
    settings[POS_Z] = settingsPosZ;

    settings[NEG_X].Width = dataNegX.GetWidth();
    settings[NEG_X].Height = dataNegX.GetHeight();
    settings[NEG_Y].Width = dataNegY.GetWidth();
    settings[NEG_Y].Height = dataNegY.GetHeight();
    settings[NEG_Z].Width = dataNegZ.GetWidth();
    settings[NEG_Z].Height = dataNegZ.GetHeight();
    settings[POS_X].Width = dataPosX.GetWidth();
    settings[POS_X].Height = dataPosX.GetHeight();
    settings[POS_Y].Width = dataPosY.GetWidth();
    settings[POS_Y].Height = dataPosY.GetHeight();
    settings[POS_Z].Width = dataPosZ.GetWidth();
    settings[POS_Z].Height = dataPosZ.GetHeight();

    RenderDataHandler::CreateTextureCubemap(texHandle);
    Bind();
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_X_NEG, settings[NEG_X], &dataNegX.GetArray()[0].x);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_Y_NEG, settings[NEG_Y], &dataNegY.GetArray()[0].x);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_Z_NEG, settings[NEG_Z], &dataNegZ.GetArray()[0].x);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_X_POS, settings[POS_X], &dataPosX.GetArray()[0].x);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_Y_POS, settings[POS_Y], &dataPosY.GetArray()[0].x);
    RenderDataHandler::SetTextureCubemapFace(CubeTextureTypes::CTT_Z_POS, settings[POS_Z], &dataPosZ.GetArray()[0].x);
}

void MTextureCubemap::DeleteIfValid(void)
{
    if (texHandle != 0)
    {
        RenderDataHandler::DeleteTexture(texHandle);
        texHandle = 0;
    }
}

void MTextureCubemap::SetFaceData(CubeTextureTypes face, const ColorTextureSettings & newSettings, const Array2D<Vector4b> & pixels)
{
    unsigned int index = GetIndex(face);
    settings[index] = newSettings;
    settings[index].Width = pixels.GetWidth();
    settings[index].Height = pixels.GetHeight();

    Bind();
    RenderDataHandler::SetTextureCubemapFace(face, settings[index], &pixels.GetArray()[0].x);
}
void MTextureCubemap::SetFaceData(CubeTextureTypes face, const ColorTextureSettings & newSettings, const Array2D<Vector4f> & pixels)
{
    unsigned int index = GetIndex(face);
    settings[index] = newSettings;
    settings[index].Width = pixels.GetWidth();
    settings[index].Height = pixels.GetHeight();

    Bind();
    RenderDataHandler::SetTextureCubemapFace(face, settings[index], &pixels.GetArray()[0].x);
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