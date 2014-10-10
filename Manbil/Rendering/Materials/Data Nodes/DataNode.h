#pragma once

#include "../UniformCollections.h"
#include "../MaterialUsageFlags.h"
#include "ShaderGenerator.h"
#include "SerializedMaterial.h"



#pragma warning(disable: 4100)

//Provides some kind of output in a shader, usually as the result of one or more inputs.
//Automatically tracks all nodes by their name. Use the static function "GetNode" to get the node with the given name.
//TODO: Get all child nodes that are only used once, and for those nodes directly use the output instead of writing it to a temp variable.
class DataNode : public ISerializable
{
public:

    //The information about the material currently being built.
    static MaterialOutputs MaterialOuts;
    //The geometry shader being used.
    static GeoShaderData GeometryShader;
    //The vertex inputs.
    static ShaderInOutAttributes VertexIns;

    static void ClearMaterialData(void)
    {
        MaterialOuts.ClearData();
        GeometryShader.MaxVertices = 0;
        GeometryShader.ShaderCode = "";
    }

    //The shader currently being generated.
    static ShaderHandler::Shaders CurrentShader;

    //Put this at the bottom of a DataNode class's declaration in the .h file (inside the declaration)
    //  to make it readable by a DataSerializer. Must be used with MAKE_NODE_READABLE_CPP.
    #define MAKE_NODE_READABLE_H(nodeClass) \
        public: \
            virtual std::string GetTypeName(void) const override { return #nodeClass; } \
        private: \
            struct Initializer \
            { \
            public: \
                Initializer(void); \
            }; \
            static Initializer IGNORE_MEEEE;
    //Put this in a DataNode class's .cpp file to make it readable by a DataSerializer.
    //Must be used with MAKE_NODE_READABLE_H.
    #define MAKE_NODE_READABLE_CPP(nodeClass, ...) \
        nodeClass::Initializer::Initializer(void) \
        { \
            if (DataNode_factoriesByTypename == 0) \
            { \
                DataNode_factoriesByTypename = new std::unordered_map<std::string, NodeFactory>(); \
            } \
            if (DataNode_factoriesByTypename->find(#nodeClass) == DataNode_factoriesByTypename->end()) \
                DataNode_factoriesByTypename->operator[](#nodeClass) = []() { return (DataNode*)new nodeClass(__VA_ARGS__); }; \
        } \
        nodeClass::Initializer nodeClass::IGNORE_MEEEE = nodeClass::Initializer();


    typedef DataNode*(*NodeFactory)(void);
    typedef std::shared_ptr<DataNode> Ptr;

    //Creates a node of the given type name with no useful data yet.
    //The type name is just the stringify-ed class name, e.x. "AddNode" or "LightingNode".
    //Creates a raw unmanaged pointer.
    //Returns 0 if the given type name doesn't correspond to a node.
    static DataNode* CreateNode(std::string typeName);


    //Thrown when something about this DataNode (or an attempt to input/output this DataNode)
    //    is found to be invalid.
    static int EXCEPTION_ASSERT_FAILED;

    //Returns 0 if the given name can't be found.
    static DataNode* GetNode(std::string name);


    //Constructors/destructors.

    DataNode(const std::vector<DataLine> & _inputs, std::string _name = "");
    //DataNode(const DataNode & cpy) = delete; // Intentionally left blank.

    virtual ~DataNode(void);


    //Error-handling.

    bool HasError(void) const { return !errorMsg.empty(); }
    std::string GetError(void) const { return errorMsg; }


    //Data getters/setters.

    std::string GetName(void) const { return name; }
    void SetName(std::string newName);

    //DO NOT MANUALLY OVERRIDE THIS. It is automatically overridden with the "MAKE_NODE_READABLE_H/_CPP" macros.
    virtual std::string GetTypeName(void) const { return "ERROR_BAD_TYPE_NAME"; }


    const std::vector<DataLine> & GetInputs(void) const { return inputs; }
    std::vector<DataLine> & GetInputs(void) { return inputs; }

    //Note that the inputs should be in the same order they were specified in the constructor.
    void ReplaceInput(unsigned int inputIndex, const DataLine & replacement);

    
    //Functions to traverse down this node and its inputs.
    void AssertAllInputsValid(void) const;
    void SetFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const;
    void GetParameterDeclarations(UniformDictionary & outUniforms, std::vector<const DataNode*> & writtenNodes) const;
    void GetFunctionDeclarations(std::vector<std::string> & outDecls, std::vector<const DataNode*> & writtenNodes) const;

    void WriteOutputs(std::string & outCode, std::vector<const DataNode*> & writtenNodeIDs) const;


    //Gets the variable name for this node's given output.
    virtual std::string GetOutputName(unsigned int outputIndex) const { return name + "_out" + std::to_string(outputIndex); }
    //Gets the variable size for this node's given output.
    virtual unsigned int GetOutputSize(unsigned int outputIndex) const = 0;
    //Gets the number of outputs this node has.
    //Default behavior: returns 1 -- most nodes have a single output.
    virtual unsigned int GetNumbOutputs(void) const { return 1; }


    virtual bool WriteData(DataWriter * writer, std::string & outError) const final;
    virtual bool ReadData(DataReader * reader, std::string & outError) final;


protected:


    //Functions that simplify child classes' constructors.

    static std::vector<DataLine> MakeVector(const DataLine & dat);
    static std::vector<DataLine> MakeVector(const DataLine & dat, const DataLine & dat2);
    static std::vector<DataLine> MakeVector(const DataLine & dat, const DataLine & dat2, const DataLine & dat3);
    static std::vector<DataLine> MakeVector(const DataLine & dat, const DataLine & dat2, const DataLine & dat3, const DataLine & dat4);
    static std::vector<DataLine> MakeVector(const DataLine & dat, unsigned int wherePut, const std::vector<DataLine> & moreDats);

    static std::string ToString(ShaderHandler::Shaders shader)
    {
        switch (shader)
        {
            case ShaderHandler::Shaders::SH_Vertex_Shader: return "Vertex_Shader";
            case ShaderHandler::Shaders::SH_Fragment_Shader: return "Fragment_Shader";
            case ShaderHandler::Shaders::SH_GeometryShader: return "Geometry Shader";
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


    //Gets whether the given input is used at all, given the current state (i.e. shader type and geometry shader data).
    //By default, returns true.
    virtual bool UsesInput(unsigned int inputIndex) const;
    //Gets whether the given input is used in calculating the given output.
    //By default, returns true.
    virtual bool UsesInput(unsigned int inputIndex, unsigned int outputIndex) const;

    virtual void AssertMyInputsValid(void) const { }

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const { }
    virtual void GetMyParameterDeclarations(UniformDictionary & outUniforms) const { }
    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const { }
    virtual void WriteMyOutputs(std::string & outCode) const = 0;

    virtual bool WriteExtraData(DataWriter * writer, std::string & outError) const { return true; }
    virtual bool ReadExtraData(DataReader * reader, std::string & outError) { return true; }

    virtual std::string GetInputDescription(unsigned int index) const { return "in" + ToString(index + 1); }


    static unsigned int GenerateUniqueID(void) { lastID += 1; return lastID; }

    //Keeps track of what names correspond with what nodes.
    static std::unordered_map<std::string, DataNode*> * DataNode_nameToNode;
    //Holds how to create each kind of DataNode.
    static std::unordered_map<std::string, DataNode::NodeFactory> * DataNode_factoriesByTypename;


private:

    static unsigned int lastID;


    std::vector<DataLine> inputs;
    std::string name;
};

//typedef std::shared_ptr<DataNode> Ptr;

#pragma warning(default: 4100)