#include "ShaderInNode.h"

#include "../../Basic Rendering/MaterialConstants.h"


ADD_NODE_REFLECTION_DATA_CPP(ShaderInNode, 1)


#pragma warning(disable: 4100)
unsigned int ShaderInNode::GetOutputSize(unsigned int outputIndex) const
{
    switch (CurrentShader)
    {
        case SH_VERTEX:
            Assert(HasVertexInput(), "Attempted to get value of a vertex input that doesn't exist!");
            return GetMatData()->VertexInputs.GetAttribute(vInputIndex).Size;

        case SH_FRAGMENT:
            Assert(HasGeometryInput(), "Attempted to get value of a geometry input that doesn't exist!");
            Assert(GetMatData()->GeoShader.IsValidData(),
                   "Attempted to get value of a geometry input when there is no geometry shader!");
            return GetMatData()->MaterialOuts.VertexOutputs[gInputIndex].Value.GetSize();

        case SH_GEOMETRY:
            Assert(HasFragmentInput(), "Attempted to get value of a fragment input that doesn't exist!");
            if (GetMatData()->GeoShader.IsValidData())
                return GetMatData()->GeoShader.OutputTypes.GetAttribute(fInputIndex).Size;
            else return GetMatData()->MaterialOuts.VertexOutputs[fInputIndex].Value.GetSize();

        default:
            Assert(false, std::string("Unknown shader type ") + ToString(CurrentShader));
            return 0;
    }
}
std::string ShaderInNode::GetOutputName(unsigned int outputIndex) const
{
    switch (CurrentShader)
    {
        case SH_VERTEX:
            Assert(HasVertexInput(), "Attempted to get value of a vertex input that doesn't exist!");
            return GetMatData()->VertexInputs.GetAttribute(vInputIndex).Name;

        case SH_FRAGMENT:
            Assert(HasFragmentInput(), "Attempted to get value of a fragment input that doesn't exist!");
            if (GetMatData()->GeoShader.IsValidData())
                return GetMatData()->GeoShader.OutputTypes.GetAttribute(fInputIndex).Name;
            else return GetMatData()->MaterialOuts.VertexOutputs[fInputIndex].Name;

        case SH_GEOMETRY:
            Assert(HasGeometryInput(), "Attempted to get value of a geometry input that doesn't exist!");
            Assert(GetMatData()->GeoShader.IsValidData(),
                   "Attempted to get value of a geometry input when there is no geometry shader!");
            return GetMatData()->MaterialOuts.VertexOutputs[gInputIndex].Name + "[" + ToString(gInputArrayIndex) + "]";

        default:
            Assert(false, std::string() + "Unknown shader type " + ToString(CurrentShader));
            return "ERROR_UNKNOWN_SHADER_TYPE";
    }
}
#pragma warning(default: 4100)

ShaderInNode::ShaderInNode(unsigned int size, std::string name,
                           int vertIn, int fragIn, int geoIn, unsigned int geoArrIn)
    : vInputIndex(vertIn), gInputIndex(geoIn), fInputIndex(fragIn), gInputArrayIndex(geoArrIn), outSize(size),
      DataNode(std::vector<DataLine>(), name)
{

}

#pragma warning (disable: 4100)
void ShaderInNode::WriteMyOutputs(std::string& outCode) const
{
    //Don't write anything; this node outputs shader inputs.
}
#pragma warning (default: 4100)


void ShaderInNode::WriteExtraData(DataWriter* writer) const
{
    writer->WriteUInt(outSize, "Output size");

    writer->WriteInt(vInputIndex, "Vertex input index");
    writer->WriteInt(fInputIndex, "Fragment input index");
    writer->WriteInt(gInputIndex, "Geometry input index");
    writer->WriteUInt(gInputArrayIndex, "Geometry input array index");
}
void ShaderInNode::ReadExtraData(DataReader* reader)
{
    reader->ReadUInt(outSize);

    reader->ReadInt(vInputIndex);
    reader->ReadInt(fInputIndex);
    reader->ReadInt(gInputIndex);
    reader->ReadUInt(gInputArrayIndex);
}