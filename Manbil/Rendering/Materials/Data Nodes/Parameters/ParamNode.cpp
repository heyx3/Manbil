#include "ParamNode.h"


ParamNode::ParamNode(unsigned int vectorSize, std::string paramName, std::string name)
    : vSize(vectorSize), pName(paramName),
    DataNode(std::vector<DataLine>(),
             []() { return DataNodePtr(new ParamNode(1, "ERROR_BAD_PARAM")); },
             name)
{

}

unsigned int ParamNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, std::string() + "Invalid output index " + ToString(index));
    return vSize;
}
std::string ParamNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, std::string() + "Invalid output index " + ToString(index));
    return pName;
}

bool ParamNode::WriteExtraData(DataWriter * writer, std::string & outError) const
{
    if (!writer->WriteString(pName, "paramName", outError))
    {
        outError = "Error writing out param name, '" +pName + "': " + outError;
        return false;
    }
    if (!writer->WriteUInt(vSize, "paramSize", outError))
    {
        outError = "Error writing out param size, " + ToString(vSize) + ": " + outError;
        return false;
    }

    return true;
}
bool ParamNode::ReadExtraData(DataReader * reader, std::string & outError)
{
    MaybeValue<std::string> tryParamName = reader->ReadString(outError);
    if (!tryParamName.HasValue())
    {
        outError = "Error reading param name: " + outError;
        return false;
    }
    pName = tryParamName.GetValue();

    MaybeValue<unsigned int> tryParamSize = reader->ReadUInt(outError);
    if (!tryParamSize.HasValue())
    {
        outError = "Error reading param size for param '" + pName + "': " + outError;
        return false;
    }
    vSize = tryParamSize.GetValue();

    return true;
}

void ParamNode::GetMyParameterDeclarations(UniformDictionary & outUniforms) const
{
    float data[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    outUniforms.FloatUniforms[GetOutputName(0)] = UniformValueF(data, vSize, GetOutputName(0));
}

#pragma warning(disable: 4100)
void ParamNode::WriteMyOutputs(std::string & outCode) const
{
    //No need to write any outputs; the uniform variable is the output.
}
#pragma warning(default: 4100)