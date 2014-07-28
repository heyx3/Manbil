#pragma once

#include "DataLine.h"
#include "../UniformCollections.h"
#include "../MaterialUsageFlags.h"
#include "GeometryShaderInfo.h"


#pragma warning(disable: 4100)

//Provides some kind of output in a shader, usually as the result of one or more inputs.
//Automatically tracks all nodes by their name. Use the static function "GetNode" to get the node with the given name.
//TODO: Get all child nodes that are only used once, and for those nodes directly use the output instead of writing it to a temp variable.
class DataNode : public ISerializable
{
public:

    //Thrown when something about this DataNode (or an attempt to input/output this DataNode)
    //    is found to be invalid.
    static int EXCEPTION_ASSERT_FAILED;

    //Returns 0 if the given name can't be found.
    static DataNode* GetNode(std::string name);


    //Some useful typedefs.

    typedef ShaderHandler::Shaders Shaders;

    
    //Sets data about the material being constructed for all nodes to use as a reference.

    static void SetShaderType(Shaders shadeType) { shaderType = shadeType; }
    static void SetGeoData(const GeoShaderData * dat) { geoData = dat; }



    //Constructors/destructors.

    DataNode(const std::vector<DataLine> & _inputs, std::string _name = "");
    DataNode(const DataNode & cpy); // Intentionally left blank.

    virtual ~DataNode(void);


    //Error-handling and name getter.

    bool HasError(void) const { return !errorMsg.empty(); }
    std::string GetError(void) const { return errorMsg; }

    std::string GetName(void) const { return name; }
    void SetName(std::string newName);


    //Getters/setters for input/output lists.

    const std::vector<DataLine> & GetInputs(void) const { return inputs; }
    const std::vector<unsigned int> & GetOutputs(void) const { return outputs; }

    //Note that the inputs should be in the same order they were specified in the constructor.
    void ReplaceInput(unsigned int inputIndex, const DataLine & replacement);

    
    //Functions to traverse down this node and its inputs to build data about these nodes.

    void SetFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const;
    void GetParameterDeclarations(UniformDictionary & outUniforms, std::vector<const DataNode*> & writtenNodes) const;
    void GetFunctionDeclarations(std::vector<std::string> & outDecls, std::vector<const DataNode*> & writtenNodes) const;

    void WriteOutputs(std::string & outCode, std::vector<const DataNode*> & writtenNodeIDs) const;


    //Gets the variable name for this node's given output.
    virtual std::string GetOutputName(unsigned int outputIndex) const { return name + "_out" + std::to_string(outputIndex); }


    virtual bool WriteData(DataWriter * writer, std::string & outError) const override sealed;
    virtual bool ReadData(DataReader * reader, std::string & outError) override sealed;


protected:


    //Functions that simplify child classes' constructors.

    static std::vector<DataLine> MakeVector(const DataLine & dat);
    static std::vector<DataLine> MakeVector(const DataLine & dat, const DataLine & dat2);
    static std::vector<DataLine> MakeVector(const DataLine & dat, const DataLine & dat2, const DataLine & dat3);
    static std::vector<DataLine> MakeVector(const DataLine & dat, const DataLine & dat2, const DataLine & dat3, const DataLine & dat4);
    static std::vector<DataLine> MakeVector(const DataLine & dat, unsigned int wherePut, const std::vector<DataLine> & moreDats);

    static Shaders GetShaderType(void) { return shaderType; }
    static const GeoShaderData * GetGeoShaderData(void) { return geoData; }

    static std::string ToString(Shaders shader)
    {
        switch (shader)
        {
            case Shaders::SH_Vertex_Shader: return "Vertex_Shader";
            case Shaders::SH_Fragment_Shader: return "Fragment_Shader";
            case Shaders::SH_GeometryShader: return "Geometry Shader";
            default: assert(false); return "UNKNOWN_SHADER_TYPE";
        }
    }
    static std::string ToString(unsigned int value)
    {
        return std::to_string(value);
    }


    mutable std::string errorMsg;

    //If the given value is false:
    //1) Sets this DataNode's error message to the given message.
    //2) Throws EXCEPTION_ASSERT_FAILED.
    void Assert(bool value, std::string error) const
    {
        if (!value)
        {
            errorMsg = error;
            throw EXCEPTION_ASSERT_FAILED;
        }
    }


    //Replaces all inputs and outputs.
    void ReplaceAll(std::vector<DataLine> & newInputs, std::vector<unsigned int> & newOutputs);


    //Gets whether the given input is used at all, given the current state (i.e. shader type and geometry shader data).
    //By default, returns true.
    virtual bool UsesInput(unsigned int inputIndex) const;
    //Gets whether the given input is used in calculating the given output.
    //By default, returns true.
    virtual bool UsesInput(unsigned int inputIndex, unsigned int outputIndex) const;

    //Outputs the new outputs for this node after the inputs have just been changed.
    virtual void ResetOutputs(std::vector<unsigned int> & newOuts) const = 0;

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const { }
    virtual void GetMyParameterDeclarations(UniformDictionary & outUniforms) const { }
    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const { }
    virtual void WriteMyOutputs(std::string & outCode) const = 0;

    virtual bool WriteExtraData(DataWriter * writer, std::string & outError) const { return true; }
    virtual bool ReadExtraData(DataReader * reader, std::string & outError) { return true; }

    virtual std::string GetInputDescription(unsigned int index) const { return "in" + std::to_string(index + 1); }


private:

    //Keeps track of what names correspond with what nodes.
    static std::unordered_map<std::string, DataNode*> nameToNode;

    static Shaders shaderType;
    static const GeoShaderData * geoData;

    static unsigned int lastID;
    static unsigned int GenerateUniqueID(void) { lastID += 1; return lastID; }


    std::vector<DataLine> inputs;
    std::vector<unsigned int> outputs;

    std::string name;
};

typedef std::shared_ptr<DataNode> DataNodePtr;

#pragma warning(default: 4100)