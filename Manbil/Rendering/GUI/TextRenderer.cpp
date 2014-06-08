#include "TextRenderer.h"


unsigned int TextRenderer::CreateTextRenderSlot(std::string fontPath, TextureSettings settings, unsigned int pWidth, unsigned int pHeight)
{
    //Create texture and set its settings.
    unsigned int texID = TexManager.CreateSFMLTexture();
    sf::Texture * tex = TexManager[texID].SFMLTex;
    sf::Texture::bind(tex);
    settings.SetData();

    //Try to create the font.
    unsigned int fontID = FTHandler.LoadFont(fontPath, FontSizeData(1, 0, 0, 0), 0);
    if (fontID == FreeTypeHandler::ERROR_ID)
    {
        errorMsg = "Error loading font from " + fontPath + ": " + FTHandler.GetError();
        return FreeTypeHandler::ERROR_ID;
    }

    fontIDToTexID[fontID] = texID;

    //Try to set the font's size.
    if (!FTHandler.SetFontSize(fontID, pWidth, pHeight))
    {
        errorMsg = "Error setting font size to " + std::to_string(pWidth) + "x" + std::to_string(pHeight) + ": " + FTHandler.GetError();
        return FreeTypeHandler::ERROR_ID;
    }
}