#include "MaterialOutputs.h"


void ShaderOutput::WriteData(DataWriter* writer) const
{
    writer->WriteString(Name, "Output name");
    writer->WriteDataStructure(Value, "Output value");
}
void ShaderOutput::ReadData(DataReader* reader)
{
    reader->ReadString(Name);
    reader->ReadDataStructure(Value);
}


void MaterialOutputs::ClearData(void)
{
    VertexPosOutput = DataLine();
    VertexOutputs.clear();
    FragmentOutputs.clear();
}

#pragma warning(disable: 4100)
void MaterialOutputs::WriteData(DataWriter* writer) const
{
    writer->WriteDataStructure(VertexPosOutput, "Vertex position output");

    writer->WriteUInt(VertexOutputs.size(), "Number of vertex outputs");
    for (unsigned int i = 0; i < VertexOutputs.size(); ++i)
    {
        writer->WriteDataStructure(VertexOutputs[i], "Vertex output #" + std::to_string(i + 1));
    }

    writer->WriteUInt(FragmentOutputs.size(), "Number of fragment outputs");
    for (unsigned int i = 0; i < FragmentOutputs.size(); ++i)
    {
        writer->WriteDataStructure(FragmentOutputs[i], "Fragment output #" + std::to_string(i + 1));
    }
}
void MaterialOutputs::ReadData(DataReader* reader)
{
    reader->ReadDataStructure(VertexPosOutput);

    unsigned int nVerts;
    reader->ReadUInt(nVerts);
    VertexOutputs.resize(nVerts);
    for (unsigned int i = 0; i < nVerts; ++i)
    {
        reader->ReadDataStructure(VertexOutputs[i]);
    }

    unsigned int nFrags;
    reader->ReadUInt(nFrags);
    FragmentOutputs.resize(nFrags);
    for (unsigned int i = 0; i < nFrags; ++i)
    {
        reader->ReadDataStructure(FragmentOutputs[i]);
    }
}
#pragma warning(default: 4100)