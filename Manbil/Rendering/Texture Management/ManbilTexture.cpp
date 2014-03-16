#include "ManbilTexture.h"

bool ManbilTexture::DeleteTexture(void)
{
    if (UsesSFMLTex())
    {
        delete SFMLTex;
        SFMLTex = 0;
        return true;
    }
    else if (UsesGLTex())
    {
        RenderDataHandler::DeleteTexture2D(GLTex);
        GLTex = 0;
        return true;
    }

    return false;
}