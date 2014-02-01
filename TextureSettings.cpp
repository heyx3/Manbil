#include "TextureSettings.h"

void TextureSettings::SetData(RenderObjHandle tex) const
{
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ToGLInt(MagFilter, false));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ToGLInt(MinFilter, true));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ToGLInt(HorWrap));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ToGLInt(VertWrap));

    if (GenerateMipmaps) glGenerateMipmap(GL_TEXTURE_2D);
}