#include "FragmentInputNode.h"


DataNodePtr FragmentInputNode::instance = DataNodePtr(new FragmentInputNode());

unsigned int FragmentInputNode::GetNumbOutputs(void) const
{
    if (GeometryShader.IsValidData())
        return GeometryShader.OutputTypes.GetNumbAttributes();
    else return MaterialOuts.VertexOutputs.size();
}

unsigned int FragmentInputNode::GetOutputSize(unsigned int index) const
{
    unsigned int max = (GeometryShader.IsValidData() ?
                        GeometryShader.OutputTypes.GetNumbAttributes() :
                        MaterialOuts.VertexOutputs.size());

    Assert(index < max, std::string() + "Invalid attribute number " + ToString(index) + ", must be less than " + ToString(max + 1));
    Assert(CurrentShader = ShaderHandler::SH_Fragment_Shader,
           std::string() + "Invalid shader type (must be Fragment): " + ToString(CurrentShader));

    return GeometryShader.IsValidData() ?
        GeometryShader.OutputTypes.GetAttributeSize(index) :
        MaterialOuts.VertexOutputs[index].Value.GetSize();
}
std::string FragmentInputNode::GetOutputName(unsigned int index) const
{
    unsigned int max = (GeometryShader.IsValidData() ?
                        GeometryShader.OutputTypes.GetNumbAttributes() :
                        MaterialOuts.VertexOutputs.size());

    Assert(index < max, std::string() + "Invalid attribute number " + ToString(index) + ", must be less than " + ToString(max + 1));
    Assert(CurrentShader = ShaderHandler::SH_Fragment_Shader,
           std::string() + "Invalid shader type (must be Fragment): " + ToString(CurrentShader));

    return GeometryShader.IsValidData() ?
        GeometryShader.OutputTypes.GetAttributeName(index) :
        MaterialOuts.VertexOutputs[index].Name;
}

void FragmentInputNode::WriteMyOutputs(std::string & outCode) const
{
    Assert(CurrentShader == ShaderHandler::SH_Fragment_Shader,
           std::string() + "Invalid shader type (must be Fragment): " + ToString(CurrentShader));
    //Dont' actually do anything, since the output is just an "in" variable.
}