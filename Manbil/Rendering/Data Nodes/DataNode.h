#pragma once

#include "../Basic Rendering/UniformCollections.h"
#include "../Basic Rendering/MaterialUsageFlags.h"
#include "ShaderGenerator.h"
#include "SerializedMaterial.h"



#pragma warning(disable: 4100)

//An atomic operation in a shader mapping some kind of input into some kind of output.
//There is no inherent limit on the number of inputs any node must have,
//    but there should be at least one output.
//Some macros are provided for child classes to expose some simple reflection data
//    for serialization purposes; this macro is mandatory.
//All DataNodes have unique names. Specific nodes can be accessed globally by name through "GetNode()".
//TODO: Get all child nodes that are only used once, and for those nodes directly use the output instead of writing it to a temp variable.
//TODO: Pull out static data into a "DataNodeMaterialData" struct and have the DataNode contain an instance of it.
class DataNode : public ISerializable
{
public:

    #pragma region Macros for defining reflection data

    //Use this at the bottom of a DataNode child class's declaration in its .h file (INSIDE the class)
    //  to give it important reflection data. Must be used with ADD_NODE_REFLECTION_DATA_CPP.
    #define ADD_NODE_REFLECTION_DATA_H(nodeClass) \
        public: \
            virtual std::string GetTypeName(void) const override { return #nodeClass; } \
        private: \
            struct _Initializer \
            { \
            public: \
                _Initializer(void); \
            }; \
            static _Initializer IGNORE_MEEEE;
    //Put this in a DataNode class's .cpp file to give it important reflection data.
    //If the class should not be instantiated by anybody (e.x. if it's a singleton),
    //  use ADD_NODE_REFLECTION_DATA_CPP_SINGLETON instead.
    //Must be used with ADD_NODE_REFLECTION_DATA_H.
    //All arguments after the class name are inputs into a constructor for it.
    //These inputs don't have to be sane; they just have to compile.
    #define ADD_NODE_REFLECTION_DATA_CPP(nodeClass, ...) \
        nodeClass::_Initializer::_Initializer(void) \
        { \
            if (DataNode_factoriesByTypename == 0) \
            { \
                DataNode_factoriesByTypename = new std::unordered_map<std::string, NodeFactory>(); \
            } \
            if (DataNode_factoriesByTypename->find(#nodeClass) == DataNode_factoriesByTypename->end()) \
            { \
                DataNode_factoriesByTypename->operator[](#nodeClass) = []() \
                { \
                    return (DataNode*)new nodeClass(__VA_ARGS__); \
                }; \
            } \
        } \
        nodeClass::_Initializer nodeClass::IGNORE_MEEEE = nodeClass::_Initializer();
    //Put this in a singleton DataNode class's .cpp file to give it important reflection data.
    //Must be used with ADD_NODE_REFLECTION_DATA_H.
    #define ADD_NODE_REFLECTION_DATA_CPP_SINGLETON(nodeClass) \
        nodeClass::_Initializer::_Initializer(void) \
        { \
            if (DataNode_factoriesByTypename == 0) \
            { \
                DataNode_factoriesByTypename = new std::unordered_map<std::string, NodeFactory>(); \
            } \
            if (DataNode_factoriesByTypename->find(#nodeClass) == DataNode_factoriesByTypename->end()) \
            { \
                DataNode_factoriesByTypename->operator[](#nodeClass) = 0; \
            } \
        } \
        nodeClass::_Initializer nodeClass::IGNORE_MEEEE = nodeClass::_Initializer();

    #pragma endregion


    //Shared pointers to DataNodes are the standard way to use them.
    typedef std::shared_ptr<DataNode> Ptr;


    //Thrown when something about this DataNode is found to be invalid when generating shader code.
    static int EXCEPTION_ASSERT_FAILED;
    //Points to the last DataNode that threw an exception.
    static const DataNode* ExceptedNode;


    //The information about the material currently being built.
    static MaterialOutputs MaterialOuts;
    //The geometry shader being used.
    static GeoShaderData GeometryShader;
    //The vertex inputs.
    static RenderIOAttributes VertexIns;

    //The shader currently being generated.
    static Shaders CurrentShader;


    //Clears out the static data about the material currently being generated.
    static void ClearMaterialData(void)
    {
        MaterialOuts.ClearData();
        GeometryShader.MaxVertices = 0;
        GeometryShader.ShaderCode = "";
    }


    //Creates an unmanaged, heap-allocated node of the given class.
    //This node is not necessarily in a valid state yet; it has not been given any inputs.
    //The type name is just the class name as a string, e.x. "AddNode" or "NormalizeNode".
    //Returns 0 if the given type name doesn't correspond to a class.
    static DataNode* CreateNode(std::string typeName);

    //Gets the node with the given unique name.
    //Returns 0 if no node exists with the given name.
    static DataNode* GetNode(std::string name);

    static bool IsSingletonType(std::string typeName);
    static bool IsSingletonType(DataNode* node) { return IsSingletonType(node->GetTypeName()); }


    //Constructors/destructors.

    DataNode(const std::vector<DataLine>& _inputs, std::string _name = "");
    //DataNode(const DataNode& cpy) = delete;
    //PRIORITY: ^ Why was that commented out? Try uncommenting it.

    virtual ~DataNode(void);


    //Error-handling.

    bool HasError(void) const { return !errorMsg.empty(); }
    std::string GetError(void) const { return errorMsg; }


    //Data getters/setters.

    std::string GetName(void) const { return name; }
    void SetName(std::string newName);

    //Gets the type name of this node -- just the string version of its class.
    //DO NOT MANUALLY OVERRIDE THIS.
    //It is automatically overridden with the "ADD_NODE_REFLECTION_DATA_H/_CPP" macros.
    virtual std::string GetTypeName(void) const { return "ERROR_BAD_TYPE_NAME"; }


    const std::vector<DataLine>& GetInputs(void) const { return inputs; }
    std::vector<DataLine>& GetInputs(void) { return inputs; }

    //Note that the inputs should be in the same order they were specified in the constructor.
    void ReplaceInput(unsigned int inputIndex, const DataLine& replacement);

    
    //Functions to traverse down this node and its inputs.
    void AssertAllInputsValid(void) const;
    void SetFlags(MaterialUsageFlags& flags, unsigned int outputIndex) const;
    void GetParameterDeclarations(UniformDictionary& outUniforms,
                                  std::vector<const DataNode*>& writtenNodes) const;
    void GetFunctionDeclarations(std::vector<std::string>& outDecls,
                                 std::vector<const DataNode*>& writtenNodes) const;

    void WriteOutputs(std::string& outCode, std::vector<const DataNode*>& writtenNodeIDs) const;


    //Gets the number of outputs this node has.
    //Default behavior: returns 1 -- most nodes have a single output.
    virtual unsigned int GetNumbOutputs(void) const { return 1; }

    //Gets the variable name for this node's given output.
    //Default: returns "_out#", where # is the output index.
    virtual std::string GetOutputName(unsigned int outputIndex) const;
    //Gets the variable size for this node's given output.
    virtual unsigned int GetOutputSize(unsigned int outputIndex) const = 0;


    virtual void WriteData(DataWriter* writer) const final;
    virtual void ReadData(DataReader* reader) final;


protected:

    typedef DataNode*(*NodeFactory)(void);


    //Functions that simplify child classes' constructors.

    static std::vector<DataLine> MakeVector(const DataLine& dat);
    static std::vector<DataLine> MakeVector(const DataLine& dat, const DataLine& dat2);
    static std::vector<DataLine> MakeVector(const DataLine& dat, const DataLine& dat2,
                                            const DataLine& dat3);
    static std::vector<DataLine> MakeVector(const DataLine& dat, const DataLine& dat2,
                                            const DataLine& dat3, const DataLine& dat4);
    static std::vector<DataLine> MakeVector(const DataLine& dat, unsigned int wherePut,
                                            const std::vector<DataLine>& moreDats);

    static std::string ToString(Shaders shader);
    static std::string ToString(unsigned int value);


    mutable std::string errorMsg;

    //If the given value is false:
    //1) Sets this DataNode's error message to the given message.
    //2) Throws EXCEPTION_ASSERT_FAILED.
    void Assert(bool value, const std::string& error) const;


    //Gets whether the given input is used at all, given the current state
    //    (i.e. shader type and geometry shader data).
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

    virtual void WriteExtraData(DataWriter* writer) const { }
    virtual void ReadExtraData(DataReader* reader) { }

    //Gets a helpful description for the given input.
    //Default behavior: returns "in#", where # is index + 1.
    virtual std::string GetInputDescription(unsigned int index) const;


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


#pragma warning(default: 4100)