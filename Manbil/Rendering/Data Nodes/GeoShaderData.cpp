#include "GeoShaderData.h"

#include "../../IO/Serialization.h"


void GeoShaderData::WriteData(DataWriter* writer) const
{
    writer->WriteString(PrimitiveTypeToString(InputPrimitive), "Input primitive");
    writer->WriteString(PrimitiveTypeToString(OutputPrimitive), "Output primitive");

    writer->WriteUInt(MaxVertices, "Max output vertices");
    writer->WriteDataStructure(RenderIOAttributes_Writable(OutputTypes),
                               "Output attributes to fragment shader");

    static_assert(sizeof(MaterialUsageFlags::Flags) == sizeof(unsigned int),
                  "MaterialUsageFlags is assumed to use a uint so far!");
    writer->WriteUInt(UsageFlags.GetBitmaskValue(), "Built-in uniforms usage bitmask");

    writer->WriteDataStructure(UniformDictionary_Writable(Params), "Custom uniform params");

    writer->WriteString(ShaderCode, "Shader code");
}
void GeoShaderData::ReadData(DataReader* reader)
{
    std::string readStr;
    reader->ReadString(readStr);
    InputPrimitive = PrimitiveTypeFromString(readStr);

    readStr.clear();
    reader->ReadString(readStr);
    OutputPrimitive = PrimitiveTypeFromString(readStr);

    reader->ReadUInt(MaxVertices);
    reader->ReadDataStructure(RenderIOAttributes_Readable(OutputTypes));
    reader->ReadUInt(UsageFlags.GetBitmaskValue());
    reader->ReadDataStructure(UniformDictionary_Readable(Params));
    reader->ReadString(ShaderCode);
}