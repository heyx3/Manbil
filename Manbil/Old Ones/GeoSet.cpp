#include "GeoSet.h"



void GeoSet::WriteData(DataWriter* writer) const
{
    writer->WriteString(MeshFile, "Mesh File");

    writer->WriteFloat(Specular, "Specular");
    writer->WriteFloat(SpecularIntensity, "Specular Intensity");

    writer->WriteBool(IsTransparent, "Is transparent?");
    writer->WriteBool(UseWorldPosUV, "Use world position as UV?");
    writer->WriteBool(UseNormalMap, "Uses normal map?");

    writer->WriteString(DiffuseTexFile, "Diffuse Tex File");
    writer->WriteDataStructure(Vector2f_Writable(DiffuseTexScale), "Diffuse Tex Scale");

    if (UseNormalMap)
    {
        writer->WriteString(NormalTexFile, "Normal Tex File");
        writer->WriteDataStructure(Vector2f_Writable(NormalTexScale), "Normal Tex Scale");
    }
}
void GeoSet::ReadData(DataReader* reader)
{
    reader->ReadString(MeshFile);

    reader->ReadFloat(Specular);
    reader->ReadFloat(SpecularIntensity);

    reader->ReadBool(IsTransparent);
    reader->ReadBool(UseWorldPosUV);
    reader->ReadBool(UseNormalMap);

    reader->ReadString(DiffuseTexFile);
    reader->ReadDataStructure(Vector2f_Readable(DiffuseTexScale));

    if (UseNormalMap)
    {
        reader->ReadString(NormalTexFile);
        reader->ReadDataStructure(Vector2f_Readable(NormalTexScale));
    }
}


void GeoSets::WriteData(DataWriter* writer) const
{
    writer->WriteCollection([](DataWriter* writer, const void* toWrite, unsigned int i, void* pData)
                            {
                                writer->WriteDataStructure(*(const GeoSet*)toWrite, std::to_string(i));
                            }, "Sets", sizeof(GeoSet), Sets.data(), Sets.size());
}
void GeoSets::ReadData(DataReader* reader)
{
    reader->ReadCollection([](void* collection, unsigned int index, void* pData)
                           {
                               std::vector<GeoSet>* setColl = (std::vector<GeoSet>*)collection;
                               setColl->push_back(GeoSet());
                           },
                           [](DataReader* reader, void* collection, unsigned int index, void* pData)
                           {
                               GeoSet* setColl = ((std::vector<GeoSet>*)collection)->data();
                               GeoSet& set = setColl[index];
                               reader->ReadDataStructure(set);
                           },
                           [](void* pCollection, unsigned int nElements)
                           {
                               std::vector<GeoSet>& set = *(std::vector<GeoSet>*)pCollection;
                               set.resize(nElements);
                           }, &Sets);
}