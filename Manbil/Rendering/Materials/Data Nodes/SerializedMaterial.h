#pragma once

#include "../../../IO/DataSerialization.h"
#include "DataLine.h"



//An output of a shader. Has a name and a value.
struct ShaderOutput : public ISerializable
{
public:

    std::string Name;
    DataLine Value;

    ShaderOutput(std::string name = "", DataLine value = DataLine()) : Name(name), Value(value) { }

    virtual bool WriteData(DataWriter * writer, std::string & outError) const override;
    virtual bool ReadData(DataReader * reader, std::string & outError) override;
};



//The set of outputs that together describes a vertex, and fragment shader.
struct MaterialOutputs : public ISerializable
{
public:

    //Should be size 4. Unless the material has a geometry shader,
    //   this DataLine should output NDC coordinates (i.e. screen-space coordinates before the Z divide).
    DataLine VertexPosOutput;
    //The outputs for the vertex/fragment shaders. Fragment outputs should all be size 4.
    std::vector<ShaderOutput> VertexOutputs, FragmentOutputs;

    //Clears all data from this output.
    void ClearData(void) { VertexPosOutput = DataLine(); VertexOutputs.clear(); FragmentOutputs.clear(); }

    virtual bool WriteData(DataWriter * writer, std::string & outError) const override;
    virtual bool ReadData(DataReader * reader, std::string & outError) override;
};



//Represents a material as a set of DataNodes and DataLines.
struct SerializedMaterial : public ISerializable
{
public:

    //The actual nodes.
    std::vector<std::shared_ptr<DataNode>> Nodes;
    //The material expression graph.
    MaterialOutputs MaterialOuts;

    SerializedMaterial(std::vector<std::shared_ptr<DataNode>> nodes = std::vector<std::shared_ptr<DataNode>>(),
                       MaterialOutputs materialOuts = MaterialOutputs())
        : Nodes(nodes), MaterialOuts(materialOuts)
    {

    }

    virtual bool WriteData(DataWriter * writer, std::string & outError) const override;
    virtual bool ReadData(DataReader * reader, std::string & outError) override;
};