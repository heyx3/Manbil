#include "ShaderInNode.h"

#include "../../MaterialData.h"



unsigned int ShaderInNode::GetOutputSize(unsigned int outputIndex) const
{
    Assert(outputIndex == 0, std::string() + "Output index must be 0, but it was " + ToString(outputIndex));

    switch (CurrentShader)
    {
        case ShaderHandler::SH_Vertex_Shader:
            Assert(HasVertexInput(), "Attempted to get value of a vertex input that doesn't exist!");
            return VertexIns.GetAttributeSize(vInputIndex);

        case ShaderHandler::SH_GeometryShader:
            Assert(HasGeometryInput(), "Attempted to get value of a geometry input that doesn't exist!");
            Assert(GeometryShader.IsValidData(), "Attempted to get value of a geometry input when there is no geometry shader!");
            return MaterialOuts.VertexOutputs[gInputIndex].Value.GetSize();

        case ShaderHandler::SH_Fragment_Shader:
            Assert(HasFragmentInput(), "Attempted to get value of a fragment input that doesn't exist!");
            if (GeometryShader.IsValidData())
                return GeometryShader.OutputTypes.GetAttributeSize(fInputIndex);
            else return MaterialOuts.VertexOutputs[fInputIndex].Value.GetSize();

        default:
            Assert(false, std::string() + "Unknown shader type " + ToString(CurrentShader));
            return 0;
    }
}
std::string ShaderInNode::GetOutputName(unsigned int outputIndex) const
{
    Assert(outputIndex == 0, std::string() + "Output index must be 0, but it was " + ToString(outputIndex));

    switch (CurrentShader)
    {
        case ShaderHandler::SH_Vertex_Shader:
            Assert(HasVertexInput(), "Attempted to get value of a vertex input that doesn't exist!");
            return VertexIns.GetAttributeName(vInputIndex);

        case ShaderHandler::SH_GeometryShader:
            Assert(HasGeometryInput(), "Attempted to get value of a geometry input that doesn't exist!");
            Assert(GeometryShader.IsValidData(), "Attempted to get value of a geometry input when there is no geometry shader!");
            return MaterialOuts.VertexOutputs[gInputIndex].Name + "[" + ToString(gInputArrayIndex) + "]";

        case ShaderHandler::SH_Fragment_Shader:
            Assert(HasFragmentInput(), "Attempted to get value of a fragment input that doesn't exist!");
            if (GeometryShader.IsValidData())
                return GeometryShader.OutputTypes.GetAttributeName(fInputIndex);
            else return MaterialOuts.VertexOutputs[fInputIndex].Name;

        default:
            Assert(false, std::string() + "Unknown shader type " + ToString(CurrentShader));
            return "ERROR_UNKNOWN_SHADER_TYPE";
    }
}

ShaderInNode::ShaderInNode(unsigned int size, std::string name, int vertIn, int fragIn, int geoIn, unsigned int geoArrIn)
    : vInputIndex(vertIn), gInputIndex(geoIn), fInputIndex(fragIn), gInputArrayIndex(geoArrIn), outSize(size),
    DataNode(std::vector<DataLine>(),
             [](std::vector<DataLine> & inputs, std::string _name) { return DataNodePtr(new ShaderInNode(0, _name)); },
             name)
{

}

#pragma warning (disable: 4100)

void ShaderInNode::WriteMyOutputs(std::string & outCode) const
{
    //Don't write anything; this node outputs shader inputs.
}

#pragma warning (default: 4100)