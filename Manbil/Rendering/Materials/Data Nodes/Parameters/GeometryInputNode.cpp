#include "GeometryInputNode.h"



DataNodePtr GeometryInputNode::instance = DataNodePtr(new GeometryInputNode());


unsigned int GeometryInputNode::GetNumbOutputs(void) const
{
    AssertGeoShader();
    return MaterialOuts.VertexOutputs.size();
}

unsigned int GeometryInputNode::GetOutputSize(unsigned int outputIndex) const
{
    Assert(outputIndex < GetNumbOutputs(),
           std::string() + "Output index was " + ToString(outputIndex) +
           " but there are only " + ToString(GetNumbOutputs()) + " outputs!");
    return MaterialOuts.VertexOutputs[outputIndex].Value.GetSize();
}
std::string GeometryInputNode::GetOutputName(unsigned int outputIndex) const
{
    Assert(outputIndex < GetNumbOutputs(),
           std::string() + "Output index was " + ToString(outputIndex) +
           " but there are only " + ToString(GetNumbOutputs()) + " outputs!");
    return MaterialOuts.VertexOutputs[outputIndex].Name;
}


void GeometryInputNode::AssertGeoShader(void) const
{
    Assert(CurrentShader == ShaderHandler::SH_GeometryShader,
           "Invalid shader type (should be Geometry): " + ToString(CurrentShader));
}


#pragma warning(disable: 4100)
void GeometryInputNode::WriteMyOutputs(std::string & outCode) const
{
    AssertGeoShader();
    //Don't actually output anything, since the output name is an "in" variable.
}
#pragma warning(default: 4100)