#include "SerializedMaterial.h"

#include "DataNode.h"
#include <stack>


//Handles reading/writing a DataNode.
struct SerializedNode : public ISerializable
{
public:

    std::shared_ptr<DataNode> Node;
    SerializedNode(std::shared_ptr<DataNode> node = std::shared_ptr<DataNode>()) : Node(node) { }

    virtual bool WriteData(DataWriter * writer, std::string & outError) const override
    {
        if (!writer->WriteString(Node->GetTypeName(), "Node Type", outError))
        {
            outError = "Error writing node '" + Node->GetName() + "'s type name '" + Node->GetTypeName() + "': " + outError;
            return false;
        }
        if (!writer->WriteDataStructure(*Node, "Node Data", outError))
        {
            outError = "Error writing node '" + Node->GetName() + "'s data: " + outError;
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

        Node = DataNode::CreateNode(tryType.GetValue());
        if (Node.get() == 0)
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

#pragma warning(disable: 4100)
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
#pragma warning(default: 4100)

bool SerializedMaterial::WriteData(DataWriter * writer, std::string & outError) const
{
    //First write out the number of nodes that will be written.
    if (!writer->WriteUInt(Nodes.size(), "NumbNodes", outError))
    {
        outError = "Error writing out the number of nodes (" + std::to_string(Nodes.size()) + "): " + outError;
        return false;
    }


    //Get all the "root" nodes (i.e. all the outputs into the material).
    std::vector<DataNode*> rootNodes;
    if (!MaterialOuts.VertexPosOutput.IsConstant())
        rootNodes.insert(rootNodes.end(), MaterialOuts.VertexPosOutput.GetNode());
    for (unsigned int i = 0; i < MaterialOuts.VertexOutputs.size(); ++i)
        if (!MaterialOuts.VertexOutputs[i].Value.IsConstant())
            rootNodes.insert(rootNodes.end(), MaterialOuts.VertexOutputs[i].Value.GetNode());
    for (unsigned int i = 0; i < MaterialOuts.FragmentOutputs.size(); ++i)
        if (!MaterialOuts.FragmentOutputs[i].Value.IsConstant())
            rootNodes.insert(rootNodes.end(), MaterialOuts.FragmentOutputs[i].Value.GetNode());

    //Define each node's "max depth" as the length of the longest chain that starts at root node
    //   and ends at the specified node.
    //Sort all nodes by their max depth.
    std::unordered_map<DataNode*, unsigned int> nodesAndDepth;
    nodesAndDepth.reserve(Nodes.size());
    unsigned int maxDepth = 0;
    for (unsigned int i = 0; i < rootNodes.size(); ++i)
    {
        //Graph search starting at the root.
        struct NodeAndDepth
        {
            DataNode* Node;
            unsigned int Depth;
            NodeAndDepth(DataNode * n = 0, unsigned int d = 0) : Node(n), Depth(d) { }
        };
        std::stack<NodeAndDepth> searchSpace;
        searchSpace.push(NodeAndDepth(rootNodes[i], 0));

        //Prevent infinite loops by tracking which edges have already been traversed.
        struct Traversal
        {
            DataNode *Start, *End;
            Traversal(DataNode* start = 0, DataNode* end = 0) : Start(start), End(end) { }
        };
        std::vector<Traversal> traversed;

        //Iterate through the search space until all nodes have been traversed.
        while (!searchSpace.empty())
        {
            //Get the current node.
            NodeAndDepth toSearch = searchSpace.top();
            searchSpace.pop();

            //If the node already exists, and doesn't have a greater depth here,
            //   then there is no need to traverse its children again.
            auto found = nodesAndDepth.find(toSearch.Node);
            if (found != nodesAndDepth.end() && found->second >= toSearch.Depth)
                continue;

            //Set/update the node's entry in the graph.
            nodesAndDepth[toSearch.Node] = toSearch.Depth;
            maxDepth = BasicMath::Max(maxDepth, toSearch.Depth);

            //Add the node's DataNode children (as opposed to constant VectorF children) to the search space.
            for (unsigned int i = 0; i < toSearch.Node->GetInputs().size(); ++i)
            {
                if (!toSearch.Node->GetInputs()[i].IsConstant())
                {
                    DataNode* child = toSearch.Node->GetInputs()[i].GetNode();

                    //Make sure the input actually exists.
                    if (child == 0)
                    {
                        outError = "Input node '" + toSearch.Node->GetInputs()[i].GetNonConstantValue() +
                                        "' of node '" + toSearch.Node->GetName() + "' doens't exist!";
                        return false;
                    }

                    //Make sure this path hasn't been traversed already.
                    if (std::find(traversed.begin(), traversed.end(),
                                  Traversal(toSearch.Node, child)) != traversed.end())
                    {
                        outError = "Infinite loop detected: The link between '" + toSearch.Node->GetName() +
                                        "' and its input '" + child->GetName() + "' has been traversed more than once.";
                        return false;
                    }
                    traversed.insert(traversed.end(), Traversal(toSearch.Node, child));

                    //Put the child node into the search space.
                    searchSpace.push(NodeAndDepth(child, toSearch.Depth + 1));
                }
            }
        }
    }

    //Now that we have all nodes sorted by their depth, start with the lowest nodes (i.e. the fewest dependencies)
    //    and work up.
    for (int depth = (int)maxDepth; depth > 0; --depth)
    {
        unsigned int depthU = (unsigned int)depth;

        //Get all nodes of this depth and write them.
        for (auto element = nodesAndDepth.begin(); element != nodesAndDepth.end(); ++element)
        {
            if (element->second == depthU)
            {
                //Find the shared pointer that corresponds to this node.
                DataNode::Ptr elementN;
                for (unsigned int i = 0; i < Nodes.size(); ++i)
                {
                    if (Nodes[i].get() == element->first)
                    {
                        elementN = Nodes[i];
                        break;
                    }
                }

                //Generate a description of the outputs.
                std::string outputDescription = std::to_string(elementN->GetNumbOutputs()) + " Outputs: ";
                for (unsigned int i = 0; i < elementN->GetNumbOutputs(); ++i)
                {
                    if (i > 0) outputDescription += ", ";
                    outputDescription += std::to_string(elementN->GetOutputSize(i));
                }

                //Write the node.
                if (!writer->WriteDataStructure(SerializedNode(elementN), outputDescription, outError))
                {
                    outError = "Error writing declaration of node '" + elementN->GetName() + "': " + outError;
                    return false;
                }
            }
        }
    }

    //Now just write out the output declarations.
    if (!writer->WriteDataStructure(MaterialOuts, "Material Outputs", outError))
    {
        outError = "Error writing material outputs: " + outError;
        return false;
    }

    return true;
}
bool SerializedMaterial::ReadData(DataReader * reader, std::string & outError)
{
    //Try reading in the number of declared DataNodes.
    MaybeValue<unsigned int> tryNumbNodes = reader->ReadUInt(outError);
    if (!tryNumbNodes.HasValue())
    {
        outError = "Error reading the number of nodes: " + outError;
        return false;
    }

    //Try to read in each DataNode.
    for (unsigned int i = 0; i < tryNumbNodes.GetValue(); ++i)
    {
        SerializedNode serNode;
        if (!reader->ReadDataStructure(serNode, outError))
        {
            outError = "Error reading DataNode declaration #" + std::to_string(i + 1) + ": " + outError;
            return false;
        }

        Nodes.insert(Nodes.end(), serNode.Node);
    }

    //Try to read in the material outputs.
    if (!reader->ReadDataStructure(MaterialOuts, outError))
    {
        outError = "Error reading material outputs: " + outError;
        return false;
    }

    return true;
}