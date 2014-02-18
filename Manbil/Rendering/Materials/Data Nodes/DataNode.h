#pragma once

#include <vector>
#include <assert.h>
#include <memory>
#include "../../../Math/Vectors.h"
#include "DataLine.h"
#include "../../../ShaderHandler.h"
#include "../UniformCollections.h"



//Represents a basic, atomic operation in a shader.
//TODO: Get all child nodes that are only used once, and for those nodes, directly use the output instead of writing it to a temp variable.
//TODO: Error message system instead of asserts.
class DataNode
{
public:

    typedef ShaderHandler::Shaders Shaders;
    typedef std::shared_ptr<DataNode> DataNodePtr;

    static void SetShaderType(Shaders shadeType) { shaderType = shadeType; }


    DataNode(const std::vector<DataLine> & _inputs, const std::vector<unsigned int> & outputSizes)
        : id(GetNextID()), inputs(_inputs), outputs(outputSizes) { }
    DataNode(const DataNode & cpy); // Intentionally left blank.


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
    //Gets this node's output lines. Each element represents the size of that output line's Vector.
    const std::vector<unsigned int> & GetOutputs(void) const { return outputs; }
    //Gets this node's output lines. Each element represents the size of that output line's Vector.
    std::vector<unsigned int> & GetOutputs(void) { return outputs; }

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