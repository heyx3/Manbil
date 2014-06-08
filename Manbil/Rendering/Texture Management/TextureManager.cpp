#include "TextureManager.h"

#include <assert.h>


TextureManager::~TextureManager(void)
{
    for (auto iterator = texturesByID.begin(); iterator != texturesByID.end(); ++iterator)
        iterator->second.DeleteTexture();
}

unsigned int TextureManager::CreateSFMLTexture(void)
{
    unsigned int thisID = id;
    id += 1;

    texturesByID[thisID].SetData(new sf::Texture());

    return thisID;
}
unsigned int TextureManager::CreateGLTexture(void)
{
    unsigned int thisID = id;
    id += 1;

    RenderDataHandler::CreateTexture2D(texturesByID[thisID].GLTex);

    return thisID;
}
unsigned int TextureManager::CreateTexture(const ManbilTexture & tex)
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
    RenderObjHandle tex;
    RenderDataHandler::CreateTexture2D(tex);
    RenderDataHandler::SetTexture2DDataColor(tex, Vector2i(width, height), color);

    texturesByID[thisID].SetData(tex);

    if (strcmp(GetCurrentRenderingError(), "") != 0)
    {
        delete texturesByID[thisID].SFMLTex;
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

    texturesByID[thisID].SetData(new sf::Texture());

    if (!texturesByID[thisID].SFMLTex->loadFromFile(filePath))
    {
        delete texturesByID[thisID].SFMLTex;
        texturesByID.erase(texturesByID.find(thisID));

        id -= 1;
        return UNUSED_ID;
    }

    return thisID;
}

void TextureManager::DeleteTexture(unsigned int id)
{
    auto loc = texturesByID.find(id);
    if (loc != texturesByID.end())
    {
        loc->second.DeleteTexture();
        texturesByID.erase(loc);
    }
}

ManbilTexture & TextureManager::operator[](unsigned int id)
{
    assert(texturesByID.find(id) != texturesByID.end());
    return texturesByID[id];
}
ManbilTexture TextureManager::operator[](unsigned int id) const
{
    auto loc = texturesByID.find(id);
    if (loc == texturesByID.end()) return ManbilTexture();
    else return loc->second;
}