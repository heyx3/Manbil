#pragma once

#include <SFML/Graphics/Texture.hpp>
#include<unordered_map>
#include <assert.h>


class TextureManager
{
public:

    //Intentionally not implemented -- can't copy texture managers!
    TextureManager(const TextureManager & cpy);

    //Deletes all textures.
    ~TextureManager(void) { for (auto iterator = texturesByID.begin(); iterator != texturesByID.end(); ++iterator) delete iterator->second; }


    //Creates a new texture and returns its index.
    unsigned int CreateTexture(unsigned int width, unsigned int height);

    //Gets the texture with the given ID, or 0 if the given id does not belong to a texture.
    sf::Texture * GetTexture(unsigned int id) { auto loc = texturesByID.find(id); if (loc == texturesByID.end()) return 0; return loc->second; }
    //Gets the texture with the given ID, or 0 if the given id does not belong to a texture.
    const sf::Texture * GetTexture(unsigned int id) const { auto loc = texturesByID.find(id); if (loc == texturesByID.end()) return 0; return loc->second; }

private:

    std::unordered_map<unsigned int, sf::Texture*> texturesByID;
    std::unordered_map<sf::Texture*, unsigned int> texturesByPointer;
};