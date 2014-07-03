#include "TextureSettings.h"


void TextureSettings1::SetData(void) const
{
    SetWrappingData();
    SetFilteringData();
    SetMipmaps();
}

void TextureSettings1::SetWrappingData(TextureWrapping horizontal, TextureWrapping vertical)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ToGLInt(horizontal));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ToGLInt(vertical));
}
void TextureSettings1::SetFilteringData(TextureFiltering minFilter, TextureFiltering magFilter, bool mips)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ToGLInt(magFilter, false, mips));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ToGLInt(minFilter, true, mips));
}
void TextureSettings1::GenMipmaps(void)
{
    glGenerateMipmap(GL_TEXTURE_2D);
}