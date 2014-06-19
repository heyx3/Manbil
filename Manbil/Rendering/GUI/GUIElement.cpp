#include "GUIElement.h"


namespace GUIElementSubroutineGenerator
{
    UniformSubroutineValue GenerateGUIElementSubroutineUniform(void)
    {


        return UniformSubroutineValue();
    }
}


const std::string GUIElement::RenderedTextSamplerUniformName = "u_GUITextSampler",
                  GUIElement::BackgroundImageSamplerUniformName = "u_GUIBackgroundSampler";
UniformSubroutineValue GUIElement::elementStateSubroutine = GUIElementSubroutineGenerator::GenerateGUIElementSubroutineUniform();