#include "ShaderInNode.h"

#include "../../MaterialData.h"


std::string ShaderInNode::GetOutputName(unsigned int outputIndex) const
{
    Assert(outputIndex == 0, std::string() + "Output index must be 0, but it was " + std::to_string(outputIndex));
    const GeoShaderData * dat = GetGeoShaderData();

    switch (GetShaderType())
    {
        case Shaders::SH_Vertex_Shader:
            Assert(HasVertexInput(), "Attempted to get value of a vertex input that doesn't exist!");
            return MaterialConstants::VertexInNameBase + std::to_string(vInputIndex);

        case Shaders::SH_GeometryShader:
            Assert(HasGeometryInput(), "Attempted to get value of a geometry input that doesn't exist!");
            Assert(dat != 0 && dat->IsValidData(), "Attempted to get value of a geometry input when there is no geometry shader!");
            return MaterialConstants::VertexOutNameBase + std::to_string(gInputIndex) + "[" + std::to_string(gInputArrayIndex) + "]";

        case Shaders::SH_Fragment_Shader:
            Assert(HasFragmentInput(), "Attempted to get value of a fragment input that doesn't exist!");
            if (dat != 0 && dat->IsValidData())
                return dat->OutputTypes.OutputNames[fInputIndex];
            else return MaterialConstants::VertexOutNameBase + std::to_string(fInputIndex);

        default:
            Assert(false, std::string() + "Unknown shader type " + ToString(GetShaderType()));
            return "ERROR_UNKNOWN_SHADER_TYPE";
    }
}