#include "TextureSettings.h"

void TextureSettings::SetData(void) const
{
    SetWrappingData();
    SetFilteringData();
    SetMipmaps();
}

void TextureSettings::SetWrappingData(TextureWrapping horizontal, TextureWrapping vertical)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ToGLInt(horizontal));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ToGLInt(vertical));
}
void TextureSettings::SetFilteringData(TextureFiltering minFilter, TextureFiltering magFilter, bool mips)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ToGLInt(magFilter, false, mips));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ToGLInt(minFilter, true, mips));
}
void TextureSettings::GenMipmaps(void)
{
    glGenerateMipmap(GL_TEXTURE_2D);
}