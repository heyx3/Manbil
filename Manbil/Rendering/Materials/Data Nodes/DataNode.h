#pragma once

#include <vector>
#include <assert.h>
#include <memory>
#include "../../../Math/Vectors.h"
#include "DataLine.h"
#include "../../../ShaderHandler.h"
#include "../UniformCollections.h"


//Different kinds of data about what a material uses.
//TODO: Once this is all done, remove all behavior from ShaderGeneration and move it out to data nodes (e.x. computing world position for output).
struct MaterialUsageFlags
{
public:

    enum Flags : unsigned
    {
        //Uses object-space position vertex output.
        DNF_USES_OBJ_POS = 0x000001,
        //Uses world-space position vertex output.
        DNF_USES_WORLD_POS = 0x000002,
        //Uses object-space normal vertex output.
        DNF_USES_OBJ_NORMAL = 0x000004,
        //Uses world-space normal vertex output.
        DNF_USES_WORLD_NORMAL = 0x000008,

        //Uses vertex color output.
        DNF_USES_COLOR = 0x000010,
        //Uses UV coord output.
        DNF_USES_UV = 0x000020,

        //Uses elapsed time uniform.
        DNF_USES_TIME = 0x000040,

        //Uses world matrix uniform.
        DNF_USES_WORLD_MAT = 0x000080,
        //Uses view matrix uniform.
        DNF_USES_VIEW_MAT = 0x000100,
        //Uses projection matrix uniform.
        DNF_USES_PROJ_MAT = 0x000200,
        //Uses WVP matrix uniform.
        DNF_USES_WVP_MAT = 0x000400,

        //Uses camera position uniform.
        DNF_USES_CAM_POS = 0x000800,
        //Uses camera forward vector uniform.
        DNF_USES_CAM_FORWARD = 0x001000,
        //Uses camera upwards vector uniform.
        DNF_USES_CAM_UPWARDS = 0x002000,
        //Uses camera sideways vector uniform.
        DNF_USES_CAM_SIDEWAYS = 0x004000,

        //Uses screen width uniform.
        DNF_USES_WIDTH = 0x008000,
        //Uses screen height uniform.
        DNF_USES_HEIGHT = 0x010000,
        //Uses screen z-near uniform.
        DNF_USES_ZNEAR = 0x020000,
        //Uses screen z-far uniform.
        DNF_USES_ZFAR = 0x040000,
        //Uses FOV uniform.
        DNF_USES_FOV = 0x080000,
    };

    bool GetFlag(Flags flag) const { return (value & (unsigned int)flag) > 0; }
    void EnableFlag(Flags flag) { value |= (unsigned int)flag; }
    void DisableFlag(Flags flag) { value &= ~((unsigned int)flag); }

private:

    //TODO: Track the outputs taken from every input data node (an unordered_map indexed by pointer?), then use that information to remove any unnecessary lines of shader code in the data node shader code generation functions.

    unsigned int value = 0;
};


//Represents a basic, atomic operation in a shader.
//TOOD: Create protected function "bool IsInputUsed(unsigned int inputIndex)" so that unnecessary inputs for the given shader type/PPE pass aren't used.
//TODO: Get all child nodes that are only used once (using "IsInputUsed()" from above), and for those nodes, directly use the output instead of writing it to a temp variable.
//TODO: Error message system instead of asserts. Instead of returning bools or whatever, just throw an exception -- we're not concerned here about speed, and this removes the need to recursively check for child nodes returning false -- just put a try/catch block around the outside.
class DataNode
{
public:

    typedef ShaderHandler::Shaders Shaders;
    typedef std::shared_ptr<DataNode> DataNodePtr;

    static void SetShaderType(Shaders shadeType) { shaderType = shadeType; }


    DataNode(const std::vector<DataLine> & _inputs, const std::vector<unsigned int> & outputSizes)
        : id(GetNextID()), inputs(_inputs), outputs(outputSizes) { }
    DataNode(const DataNode & cpy); // Intentionally left blank.

    //Marks any applicable info about this data node, given the output index.
    //By default, doesn't set anything and calls "SetFlags" for all inputs.
    void SetFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const;
    //Adds all needed parameter/uniform declarations (in the form, e.x. "vec3 myParam") to "outDecls",
    //    including all child nodes' declarations.
    //Takes in a list of all nodes that have already added their parameters to "outUniforms".
    void GetParameterDeclarations(UniformDictionary & outUniforms, std::vector<unsigned int> & writtenNodeIDs) const;
    //Adds all needed function declarations to "outDecls", including all child nodes' functions.
    //Takes in a list of all nodes that have already added their functions to "outDecls".
    void GetFunctionDeclarations(std::vector<std::string> & outDecls, std::vector<unsigned int> & writtenNodeIDs) const;
    //Appends generated GLSL code for this node (including all code for child nodes) to the end of "outCode".
    //Takes in a list of all nodes that have already added their code to "outCode".
    void WriteOutputs(std::string & outCode, std::vector<unsigned int> & writtenNodeIDs) const;


    //Gets the identifier unique for this dataNode.
    unsigned int GetUniqueID(void) const { return id; }

    //Gets the name of this data node. MUST BE A VALID GLSL VARIABLE/FUNCTION NAME.
    virtual std::string GetName(void) const { return "unknownNode"; }


    //Gets this node's input lines.
    const std::vector<DataLine> & GetInputs(void) const { return inputs; }
    //Gets this node's input lines.
    std::vector<DataLine> & GetInputs(void) { return inputs; }
    //Gets this node's output lines. Each element represents the size of that output line's VectorF.
    const std::vector<unsigned int> & GetOutputs(void) const { return outputs; }
    //Gets this node's output lines. Each element represents the size of that output line's VectorF.
    std::vector<unsigned int> & GetOutputs(void) { return outputs; }

    //Replaces the given input with the given line.
    void ReplaceInput(unsigned int inputIndex, const DataLine & replacement)
    {
        inputs[inputIndex] = replacement;
    }


    //Gets the variable name for this node's given output.
    virtual std::string GetOutputName(unsigned int outputIndex) const { assert(outputIndex < outputs.size()); return GetName() + std::to_string(id) + "_" + std::to_string(outputIndex); }


protected:


    //Functions that simplify child classes' constructors.

    static std::vector<DataLine> MakeVector(const DataLine & dat);
    static std::vector<DataLine> MakeVector(const DataLine & dat, const DataLine & dat2);
    static std::vector<DataLine> MakeVector(const DataLine & dat, const DataLine & dat2, const DataLine & dat3);
    static std::vector<DataLine> MakeVector(const DataLine & dat, const DataLine & dat2, const DataLine & dat3, const DataLine & dat4);
    static std::vector<DataLine> MakeVector(const DataLine & dat, unsigned int wherePut, const std::vector<DataLine> & moreDats);

    static std::vector<unsigned int> MakeVector(unsigned int dat);
    static std::vector<unsigned int> MakeVector(unsigned int dat, unsigned int dat2);
    static std::vector<unsigned int> MakeVector(unsigned int dat, unsigned int dat2, unsigned int dat3);
    static std::vector<unsigned int> MakeVector(unsigned int dat, unsigned int dat2, unsigned int dat3, unsigned int dat4);


    static Shaders GetShaderType(void) { return shaderType; }

    //Gets whether the given input is used when calculating the given output.
    //By default, returns true.
    virtual bool UsesInput(unsigned int inputIndex, unsigned int outputIndex) const { assert(inputIndex < inputs.size() && outputIndex < outputs.size()); return true; }

    //Sets any information about what this node does/uses.
    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const { }
    //Gets any uniforms this node defines.
    virtual void GetMyParameterDeclarations(UniformDictionary & outUniforms) const { }
    //Gets any GLSL helper function declarations this node needs to use.
    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const { }
    //Writes the output for this node, assuming all inputs have already written their output.
    virtual void WriteMyOutputs(std::string & outCode) const = 0;


private:

    static Shaders shaderType;

    static unsigned int nextID;
    static unsigned int GetNextID(void) { unsigned int id = nextID; nextID += 1; return id; }


    std::vector<DataLine> inputs;
    std::vector<unsigned int> outputs;

    unsigned int id;
};