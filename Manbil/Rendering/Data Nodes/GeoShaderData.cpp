#include "GeoShaderData.h"

#include "../../IO/SerializationWrappers.h"


void GeoShaderData::WriteData(DataWriter* writer) const
{
    writer->WriteString(PrimitiveTypeToString(InputPrimitive), "Input primitive");
    writer->WriteString(PrimitiveTypeToString(OutputPrimitive), "Output primitive");

    writer->WriteUInt(MaxVertices, "Max output vertices");
    writer->WriteDataStructure(RenderIOAttributes_Writable(OutputTypes),
                               "Output attributes to fragment shader");

    static_assert(sizeof(MaterialUsageFlags::Flags) == sizeof(unsigned int),
                  "MaterialUsageFlags is currently assumed to use a uint!");
    writer->WriteUInt(UsageFlags.GetBitmaskValue(), "Built-in uniforms usage bitmask");

    writer->WriteCollection([](DataWriter* writer, const void* elementToWrite,
                            unsigned int elIndex, void* userData)
                            {
                                Uniform& u = *(Uniform*)elementToWrite;
                                writer->WriteDataStructure(Uniform_Writable(u), std::to_string(elIndex));
                            },
                            "Params", sizeof(Uniform), Params.data(), Params.size());

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

    reader->ReadCollection([](DataReader* reader, void* pCollection, unsigned int elIndex, void* pDat)
                           {
                               UniformList& uList = *(UniformList*)pCollection;
                               reader->ReadDataStructure(Uniform_Readable(uList[elIndex]));
                           },
                           [](void* pCollection, unsigned int nElements)
                           {
                               UniformList* uList = (UniformList*)pCollection;
                               uList->resize(nElements);
                           }, &Params);

    reader->ReadString(ShaderCode);
}