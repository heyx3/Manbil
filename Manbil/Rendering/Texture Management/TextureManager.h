#pragma once

#include<unordered_map>

#include "ManbilTexture.h"


//Manages SFML textures.
class TextureManager
{
public:
    

    //An ID that will never be assigned to a texture. Is used as a "default" or "error" value.
    static const unsigned int UNUSED_ID = 0;


    TextureManager(void) : id(1) { }

    //Intentionally not implemented -- can't copy texture managers!
    TextureManager(const TextureManager & cpy);



    //Creates a new texture and returns its index.
    //Texture is not initialized, and this function always returns a new and valid index.
    unsigned int CreateTexture(void);
    //Creates a new initialized texture and returns its index.
    //Returns UNUSED_ID if the creation was a failure.
    unsigned int CreateTexture(unsigned int width, unsigned int height, Vector4b color = Vector4b((unsigned char)255, (unsigned char)255, (unsigned char)255, (unsigned char)255));
    //Creates a new texture from the given file.
    //Returns UNUSED_ID if the file wasn't loaded successfully.
    unsigned int CreateTexture(std::string filePath);
    //Puts the given texture into this texture manager.
    //NOTE: This manager now becomes responsible for the deletion of the texture --
    //   it will be deleted when this manager is deleted.
    unsigned int CreateTexture(const ManbilTexture1 & tex);

    //Removes this texture from this manager and deletes in from the OpenGL context.
    void DeleteTexture(unsigned int id);

    //Gets the texture with the given ID, or an invalid texture
    //   if the given id does not belong to a texture.
    ManbilTexture1 & operator[](unsigned int id);
    //Gets a copy of the texture with the given ID, or an invalid texture
    //   if the given id does not belong to a texture.
    ManbilTexture1 operator[](unsigned int id) const;


private:

    unsigned int id;

    std::unordered_map<unsigned int, ManbilTexture1> texturesByID;
};