#include "TextManager.h"

bool TextManager::Render(Material & mat, const RenderInfo & info, unsigned int sampler)
{
    sf::Glyph glyph;
    sf::Texture tex;

    //Draw each letter, one at a time.
    for (int i = 0; i < textValue.size(); ++i)
    {
        glyph = font.getGlyph(textValue[i], size, isBold);
        tex = font.getTexture(size);

        //Set up the texture samplers.
        for (int pass = 0; pass < mat.GetNumbPasses(); ++pass)
        {
            if (GetMesh().TextureSamplers[pass][sampler] != 0)
            {
                //TODO: Set quad's sampler to use the font texture.
            }
        }
    }

    return true;
}