#pragma once

#include <SFML/Graphics/Font.hpp>
#include "../Helper Classes/DrawingQuad.h"
#include "../../Material.h"


//Handles graphical text from some font.
class TextManager
{
public:

    TextManager(const sf::Font & _font, std::string text = "") : errorMsg(""), textValue(text), font(_font), isBold(false), size(16)
    {

    }
    TextManager(std::string fontFile, std::string text = "") : errorMsg(""), textValue(text), isBold(false), size(16)
    {
        font.loadFromFile(fontFile);
    }


    bool HasError(void) const { return !errorMsg.empty(); }
    const std::string & GetErrorMsg(void) const { return errorMsg; }
    void ClearErrorMsg(void) { errorMsg.clear(); }


    const DrawingQuad & GetQuad(void) const { return quad; }
    const Mesh & GetMesh(void) const { return quad.GetMesh(); }
    const TransformObject & GetTrans(void) const { return quad.GetMesh().Transform; }
    DrawingQuad & GetQuad(void) { return quad; }
    Mesh & GetMesh(void) { return quad.GetMesh(); }
    TransformObject & GetTrans(void) { return quad.GetMesh().Transform; }


    const std::string & GetTextValue(void) const { return textValue; }
    void SetTextValue(std::string value) { if (textValue == value) return; textValue = value; }
    void AppendText(std::string value) { if (value == "") return; textValue += value; }

    bool GetIsBold(void) const { return isBold; }
    void SetBold(bool bold) { if (isBold == bold) return; isBold = bold; }

    unsigned int GetFontSize(void) const { return size; }
    void SetFontSize(unsigned int newSize) { if (size == newSize) return; size = newSize; }

    const sf::Font & GetFont(void) const { return font; }
    sf::Font & GetFont(void) { return font; }
    void SetFont(const sf::Font & cpy) { font = cpy; }


    //Takes in which sampler to put the rasterized text into.
    bool Render(Material & material, const RenderInfo & info, unsigned int textValueSampler);


private:    

    sf::Font font;
    bool isBold;
    unsigned int size;
    std::string textValue;

    DrawingQuad quad;


    std::string errorMsg;
};