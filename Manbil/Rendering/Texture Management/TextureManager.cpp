#include "TextureManager.h"


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
unsigned int TextureManager::CreateTexture(unsigned int width, unsigned int height)
{
    unsigned int thisID = id;
    id += 1;

    texturesByID[thisID].SetData(new sf::Texture());

    if (!texturesByID[thisID].SFMLTex->create(width, height))
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


ManbilTexture & TextureManager::operator[](unsigned int id)
{
    auto loc = texturesByID.find(id);
    if (loc == texturesByID.end())
        return ManbilTexture();
    return loc->second;
}
const ManbilTexture & TextureManager::operator[](unsigned int id) const
{
    auto loc = texturesByID.find(id);
    if (loc == texturesByID.end())
        return ManbilTexture();
    return loc->second;
}