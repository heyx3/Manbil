#include "ShaderInNodes.h"

std::string ShaderInNode::GetOutputName(unsigned int outputIndex) const
{
    assert(outputIndex == 0);

    switch (GetShaderType())
    {
        case Shaders::SH_Vertex_Shader: return vertIn;
        case Shaders::SH_Fragment_Shader: return fragIn;
        default: assert(false);
    }
}