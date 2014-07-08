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


bool MTextureCubemap::Create(const ColorTextureSettings & settingsNegX, Texture2DInitialization & initNegX,
                             const ColorTextureSettings & settingsNegY, Texture2DInitialization & initNegY,
                             const ColorTextureSettings & settingsNegZ, Texture2DInitialization & initNegZ,
                             const ColorTextureSettings & settingsPosX, Texture2DInitialization & initPosX,
                             const ColorTextureSettings & settingsPosY, Texture2DInitialization & initPosY,
                             const ColorTextureSettings & settingsPosZ, Texture2DInitialization & initPosZ)
{
    DeleteIfValid();

    Array2D<Vector4f> negX(1, 1), negY(1, 1), negZ(1, 1), posX(1, 1), posY(1, 1), posZ(1, 1);

    if (!initNegX.MakeTexture(settingsNegX, negX))
        return false;
    if (!initNegY.MakeTexture(settingsNegY, negY))
        return false;
    if (!initNegZ.MakeTexture(settingsNegZ, negZ))
        return false;
    if (!initPosX.MakeTexture(settingsPosX, posX))
        return false;
    if (!initPosY.MakeTexture(settingsPosY, posY))
        return false;
    if (!initPosZ.MakeTexture(settingsPosZ, posZ))
        return false;

    settings[NEG_X] = settingsNegX;
    settings[NEG_Y] = settingsNegY;
    settings[NEG_Z] = settingsNegZ;
    settings[POS_X] = settingsPosX;
    settings[POS_Y] = settingsPosY;
    settings[POS_Z] = settingsPosZ;

    settings[NEG_X].Width = negX.GetWidth();
    settings[NEG_X].Height = negX.GetHeight();
    settings[NEG_Y].Width = negY.GetWidth();
    settings[NEG_Y].Height = negY.GetHeight();
    settings[NEG_Z].Width = negZ.GetWidth();
    settings[NEG_Z].Height = negZ.GetHeight();
    settings[POS_X].Width = posX.GetWidth();
    settings[POS_X].Height = posX.GetHeight();
    settings[POS_Y].Width = posY.GetWidth();
    settings[POS_Y].Height = posY.GetHeight();
    settings[POS_Z].Width = posZ.GetWidth();
    settings[POS_Z].Height = posZ.GetHeight();

    RenderDataHandler::CreateTextureCubemap(texHandle,
                                            settingsPosX, &posX.GetArray()[0].x,
                                            settingsPosY, &posY.GetArray()[0].x,
                                            settingsPosZ, &posZ.GetArray()[0].x,
                                            settingsNegX, &negX.GetArray()[0].x,
                                            settingsNegY, &negY.GetArray()[0].x,
                                            settingsNegZ, &negZ.GetArray()[0].x);

    return true;
}

void MTextureCubemap::DeleteIfValid(void)
{
    if (texHandle != 0)
    {
        RenderDataHandler::DeleteTexture(texHandle);
        texHandle = 0;
    }
}

bool MTextureCubemap::SetFaceData(CubeTextureTypes face, const ColorTextureSettings & newSettings, Texture2DInitialization & init)
{
    Array2D<Vector4f> fce(1, 1);
    if (!init.MakeTexture(newSettings, fce))
    {
        return false;
    }

    unsigned int index = GetIndex(face);
    settings[index] = newSettings;
    settings[index].Width = fce.GetWidth();
    settings[index].Height = fce.GetHeight();

    Bind();
    RenderDataHandler::SetTextureCubemapFace(face, settings[index], &fce.GetArray()[0].x);

    return true;
}