#pragma once

#include <SFML/Graphics/Texture.hpp>
#include<unordered_map>
#include <assert.h>


//Manages SFML textures.
//TODO: Move the functions into a cpp file.
class TextureManager
{
public:

    //An ID that will never be assigned to a texture. Is used as a "default" or "error" value.
    static const unsigned int UNUSED_ID = 0;

    TextureManager(void) : id(1) { }
    //Intentionally not implemented -- can't copy texture managers!
    TextureManager(const TextureManager & cpy);

    //Deletes all textures.
    ~TextureManager(void) { for (auto iterator = texturesByID.begin(); iterator != texturesByID.end(); ++iterator) delete iterator->second; }


    //Creates a new texture and returns its index.
    //Texture is not initialized, and this function always returns a new and valid index.
    unsigned int CreateTexture(void)
    {
        unsigned int thisID = id;
        id += 1;

        texturesByID[thisID] = new sf::Texture();

        return thisID;
    }
    //Creates a new initialized texture and returns its index.
    //Returns UNUSED_ID if the creation was a failure.
    unsigned int CreateTexture(unsigned int width, unsigned int height)
    {
        unsigned int thisID = id;
        id += 1;

        texturesByID[thisID] = new sf::Texture();

        if (!texturesByID[thisID]->create(width, height))
        {
            delete texturesByID[thisID];
            texturesByID.erase(texturesByID.find(thisID));

            id -= 1;
            return UNUSED_ID;
        }

        return thisID;
    }
    //Creates a new texture from the given file.
    //Returns UNUSED_ID if the file wasn't loaded successfully.
    unsigned int CreateTexture(std::string filePath)
    {
        unsigned int thisID = id;
        id += 1;

        texturesByID[thisID] = new sf::Texture();

        if (!texturesByID[thisID]->loadFromFile(filePath))
        {
            delete texturesByID[thisID];
            texturesByID.erase(texturesByID.find(thisID));

            id -= 1;
            return UNUSED_ID;
        }

        return thisID;
    }

    //Gets the texture with the given ID, or 0 if the given id does not belong to a texture.
    sf::Texture * operator[](unsigned int id)
    {
        auto loc = texturesByID.find(id);
        if (loc == texturesByID.end())
            return 0;
        return loc->second;
    }
    //Gets the texture with the given ID, or 0 if the given id does not belong to a texture.
    const sf::Texture * operator[](unsigned int id) const
    {
        auto loc = texturesByID.find(id);
        if (loc == texturesByID.end())
            return 0;
        return loc->second;
    }

private:

    unsigned int id;

    std::unordered_map<unsigned int, sf::Texture*> texturesByID;
    std::unordered_map<sf::Texture*, unsigned int> texturesByPointer;
};