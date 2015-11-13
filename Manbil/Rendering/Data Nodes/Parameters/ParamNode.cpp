#include "ParamNode.h"


ADD_NODE_REFLECTION_DATA_CPP(ParamNode, 1, "badParam")


ParamNode::ParamNode(unsigned int vectorSize, std::string paramName, std::string name)
    : vSize(vectorSize), pName(paramName),
      DataNode(std::vector<DataLine>(), name)
{

}

#pragma warning(disable: 4100)
unsigned int ParamNode::GetOutputSize(unsigned int index) const
{
    return vSize;
}
std::string ParamNode::GetOutputName(unsigned int index) const
{
    return pName;
}
#pragma warning(default: 4100)

void ParamNode::AssertMyInputsValid(void) const
{
    //Make sure the param name is valid.
     Assert(MaterialConstants::IsValidGLSLName(pName),
            "Parameter name '" + pName + "' isn't a valid GLSL variable name!");
}

void ParamNode::WriteExtraData(DataWriter* writer) const
{
    writer->WriteString(pName, "Param name");
    writer->WriteUInt(vSize, "Number of components");
}
void ParamNode::ReadExtraData(DataReader* reader)
{
    reader->ReadString(pName);
    reader->ReadUInt(vSize);
}

void ParamNode::GetMyParameterDeclarations(UniformList& outUniforms) const
{
    outUniforms.push_back(Uniform(GetOutputName(0), UT_VALUE_F));

    float vals[] = { 0.0f, 0.0f, 0.0f, 0.0f};
    outUniforms[outUniforms.size() - 1].Float().SetValue(vSize, vals);
}

#pragma warning(disable: 4100)
void ParamNode::WriteMyOutputs(std::string& outCode) const
{
    //No need to write any outputs; the uniform variable is the output.
}
#pragma warning(default: 4100)