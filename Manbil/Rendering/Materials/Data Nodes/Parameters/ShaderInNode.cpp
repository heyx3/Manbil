#include "ShaderInNode.h"

#include "../../MaterialData.h"


MAKE_NODE_READABLE_CPP(ShaderInNode, 1)


unsigned int ShaderInNode::GetOutputSize(unsigned int outputIndex) const
{
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
    DataNode(std::vector<DataLine>(), name)
{

}

#pragma warning (disable: 4100)

void ShaderInNode::WriteMyOutputs(std::string & outCode) const
{
    //Don't write anything; this node outputs shader inputs.
}

#pragma warning (default: 4100)


bool ShaderInNode::WriteExtraData(DataWriter * writer, std::string & outError) const
{
    if (!writer->WriteUInt(outSize, "Output Size", outError))
    {
        outError = "Error writing the output size, " + ToString(outSize) + ": " + outError;
        return false;
    }

    if (!writer->WriteInt(vInputIndex, "Vertex Input Index", outError))
    {
        outError = "Error writing the vertex input index " + std::to_string(vInputIndex) + ": " + outError;
        return false;
    }

    if (!writer->WriteInt(fInputIndex, "Fragment Input Index", outError))
    {
        outError = "Error writing the fragment input index " + std::to_string(fInputIndex) + ": " + outError;
        return false;
    }

    if (!writer->WriteInt(gInputIndex, "Geometry Input Index", outError))
    {
        outError = "Error writing the geometry input index " + std::to_string(gInputIndex) + ": " + outError;
        return false;
    }

    if (!writer->WriteUInt(gInputArrayIndex, "Geometry Input Array Index", outError))
    {
        outError = "Error writing the geometry input array index " + ToString(gInputArrayIndex) + ": " + outError;
        return false;
    }

    return true;
}
bool ShaderInNode::ReadExtraData(DataReader * reader, std::string & outError)
{
    MaybeValue<unsigned int> tryOutSize = reader->ReadUInt(outError);
    if (!tryOutSize.HasValue())
    {
        outError = "Error reading output size: " + outError;
        return false;
    }
    outSize = tryOutSize.GetValue();

    MaybeValue<int> tryVertIn = reader->ReadInt(outError);
    if (!tryVertIn.HasValue())
    {
        outError = "Error reading vertex input index: " + outError;
        return false;
    }
    vInputIndex = tryVertIn.GetValue();

    MaybeValue<int> tryFragIn = reader->ReadInt(outError);
    if (!tryFragIn.HasValue())
    {
        outError = "Error reading fragment input index: " + outError;
        return false;
    }
    fInputIndex = tryFragIn.GetValue();

    MaybeValue<int> tryGeoValue = reader->ReadInt(outError);
    if (!tryGeoValue.HasValue())
    {
        outError = "Error reading geometry input index: " + outError;
        return false;
    }
    gInputIndex = tryGeoValue.GetValue();

    MaybeValue<unsigned int> tryGeoArrValue = reader->ReadUInt(outError);
    if (!tryGeoArrValue.HasValue())
    {
        outError = "Error reading geometry array input index: " + outError;
        return false;
    }
    gInputArrayIndex = tryGeoArrValue.GetValue();

    return true;
}