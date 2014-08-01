#include "SerializedMaterial.h"


bool ShaderOutput::WriteData(DataWriter * writer, std::string & outError) const
{
    if (!writer->WriteString(Name, "Output Name", outError))
    {
        outError = "Error writing output name '" + Name + "': " + outError;
        return false;
    }
    if (!writer->WriteDataStructure(Value, "Output Value", outError))
    {
        outError = "Error writing output value: " + outError;
        return false;
    }

    return true;
}
bool ShaderOutput::ReadData(DataReader * reader, std::string & outError)
{
    MaybeValue<std::string> tryName = reader->ReadString(outError);
    if (!tryName.HasValue())
    {
        outError = "Error reading output name: " + outError;
        return false;
    }
    Name = tryName.GetValue();

    if (!reader->ReadDataStructure(Value, outError))
    {
        outError = "Error reading data line value for output '" + Name + "': " + outError;
        return false;
    }

    return true;
}


bool MaterialOutputs::WriteData(DataWriter * writer, std::string & outError) const
{
    if (!writer->WriteDataStructure(VertexPosOutput, "Vertex Position Output", outError))
    {
        outError = "Error writing out the vertex position output: " + outError;
        return false;
    }
    if (!writer->WriteCollection("Vertex Outputs",
                                 [](const void* coll, unsigned int index, DataWriter * write, std::string & outErr, void* d)
                                 {
                                     return write->WriteDataStructure(((ShaderOutput*)coll)[index].Value,
                                                                      std::to_string(index), outErr);
                                 }, VertexOutputs.data(), VertexOutputs.size(), outError))
    {
        outError = "Error writing out the vertex outputs: " + outError;
        return false;
    }
    if (!writer->WriteCollection("Fragment Outputs",
                                 [](const void* coll, unsigned int index, DataWriter * write, std::string & outErr, void* d)
                                 {
                                     return write->WriteDataStructure(((ShaderOutput*)coll)[index].Value,
                                                                      std::to_string(index), outErr);
                                 }, FragmentOutputs.data(), FragmentOutputs.size(), outError))
    {
        outError = "Error writing out the fragment outputs: " + outError;
        return false;
    }

    return true;
}
bool MaterialOutputs::ReadData(DataReader * reader, std::string & outError)
{
    if (!reader->ReadDataStructure(VertexPosOutput, outError))
    {
        outError = "Error reading in the vertex position output: " + outError;
        return false;
    }

    std::vector<unsigned char> tryCollData;
    if (!reader->ReadCollection([](void* coll, unsigned int index, DataReader * read, std::string & outErr, void* d)
                                {
                                    return read->ReadDataStructure(((ShaderOutput*)coll)[index].Value, outErr);
                                }, sizeof(ShaderOutput), outError, tryCollData))
    {
        outError = "Error reading in the vertex outputs: " + outError;
        return false;
    }
    VertexOutputs.clear();
    VertexOutputs.resize(tryCollData.size() / sizeof(ShaderOutput));
    for (unsigned i = 0; i < VertexOutputs.size(); ++i)
        VertexOutputs[i] = ((ShaderOutput*)tryCollData.data())[i];
                                
    tryCollData.clear();
    if (!reader->ReadCollection([](void* coll, unsigned int index, DataReader * read, std::string & outErr, void* d)
                                {
                                    return read->ReadDataStructure(((ShaderOutput*)coll)[index].Value, outErr);
                                }, sizeof(ShaderOutput), outError, tryCollData))
    {
        outError = "Error reading in the fragment outputs: " + outError;
        return false;
    }
    FragmentOutputs.clear();
    FragmentOutputs.resize(tryCollData.size() / sizeof(ShaderOutput));
    for (unsigned i = 0; i < FragmentOutputs.size(); ++i)
        FragmentOutputs[i] = ((ShaderOutput*)tryCollData.data())[i];
}


//PRIORITY: Finish.
bool SerializedMaterial::WriteData(DataWriter * writer, std::string & outError) const
{

}
bool SerializedMaterial::ReadData(DataReader * reader, std::string & outError)
{

}