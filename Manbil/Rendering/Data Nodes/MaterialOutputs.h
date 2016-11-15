#pragma once

#include "../../IO/DataSerialization.h"
#include "DataLine.h"
#include "../Basic Rendering/RenderIOAttributes.h"


class DataNode;


//An output of a shader. Has a name and a value.
struct ShaderOutput : public ISerializable
{
public:

    std::string Name;
    DataLine Value;


    ShaderOutput(std::string name = "", DataLine value = DataLine()) : Name(name), Value(value) { }


    virtual void WriteData(DataWriter* writer) const override;
    virtual void ReadData(DataReader* reader) override;
};



//The set of outputs that together describes a vertex, and fragment shader.
struct MaterialOutputs : public ISerializable
{
public:

    //The position output of the vertex shader ("gl_Position"). Should be size 4.
    //Unless the material has a geometry shader, this DataLine should output NDC coordinates
    //    (i.e. homogenous screen-space coordinates before the Z divide).
    DataLine VertexPosOutput;
    //The outputs for the vertex shader. These values will go into the fragment shader
    //    (or geometry shader if it exists) as inputs.
    std::vector<ShaderOutput> VertexOutputs;
    //The outputs for the fragment shader.
    //These values will be output into the various color textures bound
    //    to the render target that's being rendered into.
    std::vector<ShaderOutput> FragmentOutputs;


    //Clears all data from this output.
    void ClearData(void);

    virtual void WriteData(DataWriter* writer) const override;
    virtual void ReadData(DataReader* reader) override;
};
