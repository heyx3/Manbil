#include "SerializedMaterial.h"

#include "DataNodeIncludes.h"
#include <stack>


//Handles reading/writing a DataNode.
struct SerializedNode : public ISerializable
{
public:

    //Unmanaged raw pointer. Needs to be managed by whoever called "ReadData" on this instance.
    DataNode* Node;
    SerializedNode(DataNode * node = 0) : Node(node) { }

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

    if (!writer->WriteUInt(VertexOutputs.size(), "Numb Vertex Outputs", outError))
    {
        outError = "Error writing out the size of the vertex output collection: " + outError;
        return false;
    }
    for (unsigned int i = 0; i < VertexOutputs.size(); ++i)
    {
        if (!writer->WriteDataStructure(VertexOutputs[i], "Vertex Output #" + std::to_string(i + 1), outError))
        {
            outError = "Error writing out vertex output #" + std::to_string(i + 1) + ": " + outError;
            return false;
        }
    }

    if (!writer->WriteUInt(FragmentOutputs.size(), "Numb Fragment Outputs", outError))
    {
        outError = "Error writing out the size of the fragment output collection: " + outError;
        return false;
    }
    for (unsigned int i = 0; i < FragmentOutputs.size(); ++i)
    {
        if (!writer->WriteDataStructure(FragmentOutputs[i], "Fragment Output #" + std::to_string(i + 1), outError))
        {
            outError = "Error writing out fragment output #" + std::to_string(i + 1) + ": " + outError;
            return false;
        }
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

    MaybeValue<unsigned int> tryNumbVerts = reader->ReadUInt(outError);
    if (!tryNumbVerts.HasValue())
    {
        outError = "Error reading in the number of vertex outputs: " + outError;
        return false;
    }
    VertexOutputs.resize(tryNumbVerts.GetValue());
    for (unsigned int i = 0; i < tryNumbVerts.GetValue(); ++i)
    {
        if (!reader->ReadDataStructure(VertexOutputs[i], outError))
        {
            outError = "Error reading in the " + std::to_string(i + 1) + "-th vertex output: " + outError;
            return false;
        }
    }

    MaybeValue<unsigned int> tryNumbFrags = reader->ReadUInt(outError);
    if (!tryNumbFrags.HasValue())
    {
        outError = "Error reading in the number of fragment outputs: " + outError;
        return false;
    }
    FragmentOutputs.resize(tryNumbFrags.GetValue());
    for (unsigned int i = 0; i < tryNumbFrags.GetValue(); ++i)
    {
        if (!reader->ReadDataStructure(FragmentOutputs[i], outError))
        {
            outError = "Error reading in the " + std::to_string(i + 1) + "-th fragment output: " + outError;
            return false;
        }
    }

    return true;
}
#pragma warning(default: 4100)


//Gets whether the given node is a special singleton node that shouldn't be written out.
bool IsSingleton(DataNode* node)
{
    return node == VertexInputNode::GetInstance().get() ||
           node == GeometryInputNode::GetInstance().get() ||
           node == FragmentInputNode::GetInstance().get() ||
           node == TimeNode::GetInstance().get() ||
           node == CameraDataNode::GetInstance().get() ||
           node == ProjectionDataNode::GetInstance().get();
}

bool SerializedMaterial::WriteData(DataWriter * writer, std::string & outError) const
{
    //First, write out the expected vertex inputs.
    if (!writer->WriteDataStructure(VertexInputs, "Vertex Inputs", outError))
    {
        outError = "Error writing out the vertex inputs: " + outError;
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
        //Note that two nodes can be connected along more than one line,
        //    so we have to count how many connections there should be between each pair of nodes.
        struct Traversal
        {
            DataNode *Start, *End;
            Traversal(DataNode* start = 0, DataNode* end = 0) : Start(start), End(end) { }
            bool operator==(const Traversal & other) const { return Start == other.Start && End == other.End; }
            unsigned int operator()(const Traversal & v) const { return Vector2i((int)Start, (int)End).GetHashCode(); }
        };
        std::unordered_map<Traversal, unsigned int, Traversal> connectionsPerTraversal,
                                                               connectionsSoFar;

        //Iterate through the search space until all nodes have been traversed.
        while (!searchSpace.empty())
        {
            //Get the current node.
            NodeAndDepth toSearch = searchSpace.top();
            searchSpace.pop();

            //If the node is a special singleton, it shouldn't be written out.
            if (IsSingleton(toSearch.Node)) continue;
            //If the node already exists, and doesn't have a greater depth here,
            //   then there is no need to traverse its children again.
            auto found = nodesAndDepth.find(toSearch.Node);
            if (found != nodesAndDepth.end() && found->second >= toSearch.Depth)
                continue;

            //Set/update the node's entry in the graph.
            nodesAndDepth[toSearch.Node] = toSearch.Depth;
            maxDepth = Mathf::Max(maxDepth, toSearch.Depth);

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
                    Traversal trvs(toSearch.Node, child);
                    if (connectionsSoFar.find(trvs) != connectionsSoFar.end())
                    {
                        //If there are more connections between these nodes than there should be,
                        //   then this traversal has happened more than once -- i.e., there is an infinite loop.
                        if (connectionsSoFar[trvs] >= connectionsPerTraversal[trvs])
                        {
                            outError = "Infinite loop detected: The link between '" + toSearch.Node->GetName() +
                                "' and its input '" + child->GetName() + "' has been traversed more than once.";
                            return false;
                        }
                        else connectionsSoFar[trvs] += 1;
                    }
                    else
                    {
                        connectionsSoFar[trvs] = 1;

                        //Count how many connections there should be for this traversal.
                        unsigned int connPerTrav = 0;
                        for (unsigned int j = 0; j < toSearch.Node->GetInputs().size(); ++j)
                        {
                            DataLine & inp = toSearch.Node->GetInputs()[j];
                            if (!inp.IsConstant() && inp.GetNode() == trvs.End)
                                connPerTrav += 1;
                        }
                        connectionsPerTraversal[trvs] = connPerTrav;
                    }

                    //Put the child node into the search space.
                    searchSpace.push(NodeAndDepth(child, toSearch.Depth + 1));
                }
            }
        }
    }

    //Before writing any nodes, write out the number of nodes that will be written.
    if (!writer->WriteUInt(nodesAndDepth.size(), "NumbNodes", outError))
    {
        outError = "Error writing out the number of nodes (" + std::to_string(nodesAndDepth.size()) + "): " + outError;
        return false;
    }


    //Now that we have all nodes sorted by their depth, start with the lowest nodes (i.e. the fewest dependencies)
    //    and work up.
    for (int depth = (int)maxDepth; depth >= 0; --depth)
    {
        unsigned int depthU = (unsigned int)depth;

        //Get all nodes of this depth and write them.
        for (auto element = nodesAndDepth.begin(); element != nodesAndDepth.end(); ++element)
        {
            if (element->second == depthU)
            {
                //Generate a description of the outputs.
                std::string outputDescription = std::to_string(element->first->GetNumbOutputs()) + " Outputs: ";
                for (unsigned int i = 0; i < element->first->GetNumbOutputs(); ++i)
                {
                    if (i > 0) outputDescription += ", ";
                    outputDescription += std::to_string(element->first->GetOutputSize(i));
                }

                //Write the node.
                if (!writer->WriteDataStructure(SerializedNode(element->first), outputDescription, outError))
                {
                    outError = "Error writing declaration of node '" + element->first->GetName() + "': " + outError;
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
    //Try reading in the vertex inputs.
    if (!reader->ReadDataStructure(VertexInputs, outError))
    {
        outError = "Error reading in the vertex inputs: " + outError;
        return false;
    }

    //Try reading in the number of declared DataNodes.
    MaybeValue<unsigned int> tryNumbNodes = reader->ReadUInt(outError);
    if (!tryNumbNodes.HasValue())
    {
        outError = "Error reading the number of nodes: " + outError;
        return false;
    }

    //Try to read in each DataNode.
    nodeStorage.clear();
    for (unsigned int i = 0; i < tryNumbNodes.GetValue(); ++i)
    {
        SerializedNode serNode;
        if (!reader->ReadDataStructure(serNode, outError))
        {
            outError = "Error reading DataNode declaration #" + std::to_string(i + 1) + ": " + outError;
            return false;
        }

        nodeStorage.insert(nodeStorage.end(), DataNode::Ptr(serNode.Node));
    }

    //Try to read in the material outputs.
    if (!reader->ReadDataStructure(MaterialOuts, outError))
    {
        outError = "Error reading material outputs: " + outError;
        return false;
    }

    return true;
}