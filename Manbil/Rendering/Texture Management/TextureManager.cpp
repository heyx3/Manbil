#include "TextureManager.h"

#include <assert.h>


unsigned int TextureManager::CreateTexture(void)
{
    unsigned int thisID = id;
    id += 1;

    texturesByID[thisID].Create();

    return thisID;
}
unsigned int TextureManager::CreateTexture(const ManbilTexture1 & tex)
{
    unsigned int thisID = id;
    id += 1;

    texturesByID[thisID] = tex;

    return thisID;
}
unsigned int TextureManager::CreateTexture(unsigned int width, unsigned int height, Vector4b color)
{
    unsigned int thisID = id;
    id += 1;

    ClearAllRenderingErrors();
    texturesByID[thisID].Create();
    texturesByID[thisID].SetData(color, width, height);

    if (strcmp(GetCurrentRenderingError(), "") != 0)
    {
        texturesByID.erase(texturesByID.find(thisID));

        id -= 1;
        return UNUSED_ID;
    }

    return thisID;
}
unsigned int TextureManager::CreateTexture(std::string filePath)
{
    unsigned int thisID = id;
    id += 1;

    //Try loading the SFML texture.
    sf::Texture SFMLTex;
    if (!SFMLTex.loadFromFile(filePath))
    {
        id -= 1;
        return UNUSED_ID;
    }

    //Try creating the GL texture and copying the SFML texture data into it.
    ManbilTexture1 & tex = texturesByID[thisID];
    tex.Create();
    Array2D<Vector4b> texData(0, 0);
    tex.SetData(texData);

    return thisID;
}

void TextureManager::DeleteTexture(unsigned int id)
{
    auto loc = texturesByID.find(id);
    if (loc != texturesByID.end())
    {
        texturesByID.erase(loc);
    }
}

ManbilTexture1 & TextureManager::operator[](unsigned int id)
{
    assert(texturesByID.find(id) != texturesByID.end());
    return texturesByID[id];
}
ManbilTexture1 TextureManager::operator[](unsigned int id) const
{
    auto loc = texturesByID.find(id);
    if (loc == texturesByID.end()) return ManbilTexture1();
    else return loc->second;
}