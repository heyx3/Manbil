#pragma once

#include "../../IO/DataSerialization.h"
#include "../IEditable.h"
#include "DAGInput.h"
#include "../EditorObjects.h"



template<typename DAGNodeType, typename DAGInputType>
//A base class for the visitor pattern being applied to a DAG from a root node.
//Note that after visiting each DAG node, this visitor will remember and always skip that node.
class DAGVisitor
{
public:

    virtual ~DAGVisitor(void) { }
    
    //Clears this visitor's node history so that it forgets any nodes it's already visited.
    void ClearNodeHistory(void) { visitedAlready.clear(); }

    //Calls this instance's "ActOnNode" method on every node starting at the given root hierarchy.
    //At each node, this function acts on the node itself before acting on its inputs.
    void VisitNodeFirst(DAGNodeType * node)
    {
        if (std::find(visitedAlready.begin(), visitedAlready.end(), node.GetName()) == visitedAlready.end())
        {
            visitedAlready.insert(visitedAlready.end(), node.GetName());
            ActOnNode(node);

            const std::vector<DAGInputType> & children = node.GetChildren();
            for (unsigned int i = 0; i < children.size(); ++i)
                if (!children[i].IsConstant())
                    this->Visit((DAGNodeType*)children[i].GetDAGNode());
        }
    }
    //Calls this instance's "ActOnNode" method on every node starting at the given root hierarchy.
    //At each node, this function acts on that node's inputs before acting on the node itself.
    void VisitChildrenFirst(DAGNodeType * node)
    {
        if (std::find(visitedAlready.begin(), visitedAlready.end(), node.GetName()) == visitedAlready.end())
        {
            visitedAlready.insert(visitedAlready.end(), node.GetName());

            const std::vector<DAGInputType> & children = node.GetChildren();
            for (unsigned int i = 0; i < children.size(); ++i)
                if (!children[i].IsConstant())
                    this->Visit((DAGNodeType*)children[i].GetDAGNode());

            ActOnNode(node);
        }
    }

protected:

    //Visits the given node and does some action on it.
    virtual void ActOnNode(DAGNodeType * node) = 0;

private:

    std::vector<void*> visitedAlready;
};



//The type of DAGInput this node uses.
//It should be trivially copyable/assignable.
template<typename DAGInputType>
//A node in a DAG. Can generate a GUI to edit it and be serialized to a file.
//Each node has a name; if an empty string is passed for a node's name, then a unique name will be generated.
class DAGNode : public ISerializable, public IEditable
{
public:

    //An exception that's thrown when
    static int EXCEPTION_ASSERT_FAILED;


    //If an empty string is passed for the name, a unique name will be generated.
    DAGNode(const std::vector<DAGInputType> & _inputs, std::string _name = "")
        : inputs(_inputs), name(_name)
    {
        if (name.empty())
        {
            name = "node" + std::to_string(nextID);
            nextID += 1;
        }
    }


    //Gets the number of outputs this node offers.
    virtual unsigned int GetNumbOutputs(void) const = 0;
    //Gets the name of each output. Default behavior: outputs 'Out#", where # is the output index.
    virtual std::string GetOutputName(unsigned int outputIndex) { return "Out" + std::to_string(outputIndex); }

    //DO NOT MANUALLY OVERRIDE THIS.
    //It is automatically overridden with the "FINALIZE_DAG_NODE_H/_CPP" macros.
    virtual std::string GetTypeName(void) const = 0;

    //Gets some kind of helpful description for the given input.
    //Default behavior: returns "Input #".
    virtual std::string GetInputDescription(unsigned int index) const { return "Input " + std::to_string(index); }

    //Asserts that all this node's inputs are valid values for this node.
    //If something fails, this node's error message is set and EXCEPTION_ASSERT_FAILED is raised.
    //Default behavior: don't assert anything.
    virtual void AssertMyInputsAreValid(void) const { }


    //Gets whether this node has ever thrown an error.
    bool HasError(void) const { return !errorMsg.empty(); }
    //Gets the most recent error message that this instance generated.
    const std::string & GetErrorMsg(void) const { return errorMsg; }

    const std::vector<DAGInputType> & GetInputs(void) const { return inputs; }
    std::vector<DAGInputTypes> & GetInputs(void) const { return inputs; }


    virtual bool WriteData(DataWriter * writer, std::string & outError) const override
    {
        if (!writer->WriteString(name, "Name", outError))
        {
            outError = "Error writing name, '" + name + "': " + outError;
            return false;
        }

        bool tryWrite = writer->WriteCollection("Inputs",
                                                [](void* pCollection, unsigned int index,
                                                   DataWriter * writer, std::string & outError,
                                                   void* pData)
        {
            return writer->WriteDataStructure(((DAGInputType*)pCollection)[index],
                                              std::to_string(index), outError);
        }, inputs.data(), inputs.size(), outError)
        if (!tryWrite)
        {
            outError = "Error writing out the inputs: " + outError;
            return false;
        }

        return true;
    }
    virtual bool ReadData(DataReader * reader, std::string & outError) override
    {
        MaybeValue<std::string> tryStr = reader->ReadString(outError);
        if (!tryStr.HasValue())
        {
            outError = "Error reading name: " + outError;
            return false;
        }
        name = tryStr.GetValue();

        std::vector<unsigned char> binaryData;
        binaryData.reserve(sizeof(DAGInputType));
        bool tryRead = reader->ReadCollection([](void* pCollection, unsigned int index,
                                                 DataReader * reader, std::string & outError, void* pData)
        {
            ((DAGInputType*)pCollection)[index] = DAGInputType();
            return reader->ReadDataStructure(((DAGInputType*)pCollection)[index], outError);
        }, sizeof(DAGInputType), outError, binaryData);
        if (!tryRead)
        {
            outError = "Error reading in the inputs: " + outError;
            return false;
        }

        for (int i = 0; i < binaryData.size() / sizeof(DAGInputType); ++i)
            inputs[i] = ((DAGInputType*)binaryData.data())[i];
        return true;
    }

    virtual void BuildEditorElements(std::vector<EditorObjectPtr> & outElements) override
    {
        //TODO: Make DAGInput editable and use that. Also, make EditorObjects for collapsible panels and collections.
    }


protected:
    
    mutable std::string errorMsg;

    //If the given value is false:
    //1) Sets this instance's "errorMsg" field to the given message.
    //2) Throws EXCEPTION_ASSERT_FAILED.
    void Assert(bool value, std::string failError) const
    {
        if (!value)
        {
            errorMsg = failError;
            throw EXCEPTION_ASSERT_FAILED;
        }
    }


private:

    std::vector<DAGInputType> inputs;
    std::string name;

    static unsigned int nextID;
};



//Should be used inside the base class's .h file (at the end of its declaration)
//    that itself inherits from a DAGNode template.
//Adds a constructor that takes in an std::vector<> of inputs and an optional name.
//Adds the ability to track and dynamically instantiate sub-classes of
//    the class that uses this macro.
#define FINALIZE_BASE_DAG_NODE_H(className, dagInputType) \
private: \
    typedef className * (*DAGNodeFactory)(void); \
    typedef std::shared_ptr<className> Ptr; \
    static std::unordered_map<std::string, className *> * global_NameToNode; \
    static std::unordered_map<std::string, DAGNodeFactory> * global_NameToFactory; \
    static className * CreateNode(std::string typeName) \
    { \
        auto found = global_NameToFactory->find(typeName); \
        if (found == global_NameToFactory->end()) return 0; \
        return found->second(); \
    } \
public: \
    className(const std::vector<dagInputType> & _inputs, std::string _name = "") \
        : DAGNode(_inputs, _name) \
    { \
        if (global_NameToNode == 0) \
        { \
            global_NameToNode = new std::unordered_map<std::string, std::string>(); \
        } \
        Assert(global_NameToNode->find(name) == global_NameToNode->end(), \
               "A node with the name '" + name + "' already exists!"); \
        global_NameToNode->operator[](name) = this; \
    }

//Should be used inside the base class's .cpp file that itself inherits from a DAGNode template.
//Implements the stuff declared by FINALIZE_BASE_DAG_NODE_H.
#define FINALIZE_BASE_DAG_NODE_CPP(className) \
    std::unordered_map<std::string, className *> className::global_NameToNode = 0; \
    std::unordered_map<std::string, DAGNodeFactory> * className::global_NameToFactory = 0;

//Should be used in each child class's .h file (at the end of its declaration) of the base class
//    that itself inherits from a DAGNode template.
//Adds the class to its base class's tracking/factory system.
#define FINALIZE_DAG_NODE_H(nodeClass) \
    public: \
        virtual std::string GetTypeName(void) const override { return #nodeClass; } \
    private: \
        struct Initializer \
        { \
        public: \
            Initializer(void); \
        }; \
        static Initializer IGNORE_MEEEEE;

//Should be used in each child class's .cpp file. Implements the stuff declared by FINALIZE_DAG_NODE_H.
//All arguments after the first one are the constructor arguments to construct a dummy version of this node.
#define FINALIZE_DAG_NODE_CPP(nodeClass, ...) \
    nodeClass::Initializer::Initializer(void) \
    { \
        if (global_NameToFactory == 0) \
            global_NameToFactory = new std::unordered_map<std::string, DAGNodeFactory>(); \
        if (global_NameToFactory->find(#nodeClass) == global_NameToFactory->end()) \
            global_NameToFactory->operator[](#nodeClass) = []() { return new nodeClass(__VA_ARGS__); }; \
    }


//The type of DAGNode being serialized. Should be a child of DAGNode<>.
template<typename DAGNodeType>
//Handles reading/writing a DAGNode.
struct SerializedDAGNode : public ISerializable
{
public:

    //Raw pointer to a DAGNode. If this struct's data was just read in from a DataReader,
    //    this node was just allocated on the heap and must be managed.
    DAGNodeType* Node;
    SerializedDAGNode(DAGNodeType* node = 0) : Node(node) { }

    virtual bool WriteData(DataWriter * writer, std::string & outError) const override
    {
        if (!writer->WriteString(Node->GetTypeName(), "Node type", outError))
        {
            outError  = "Error writing node '" + Node->GetName() + "'s type name '" +
                            Node->GetTypeName() + "': " + outError;
            return false;
        }
        if (!writer->WriteDataStructure(*Node, "Node value", outError))
        {
            outError = "Error writing node '" + Node->GetName() + "': " + outError;
            return false;
        }

        return true;
    }
    virtual bool ReadData(DataReader * reader, std::string & outError) override
    {
        MaybeValue<std::string> tryType = reader->ReadString(outError);
        if (!tryType.HasValue())
        {
            outError = "Error reading node's typename: " + outError;
            return false;
        }

        Node = DAGNodeType::CreateNode(tryType.GetValue());
        if (Node == 0)
        {
            outError = "Node type '" + tryType.GetValue() + "' is unknown!";
            return false;
        }
        if (!reader->ReadDataStructure(*Node, outError))
        {
            outError = "Error reading node '" + Node->GetName() + "'s data: " + outError;
            return false;
        }

        return true;
    }
};