#include "SerializedMaterial.h"

#include "DataNodes.hpp"
#include "../../IO/Serialization.h"
#include <stack>


//Handles reading/writing a DataNode.
struct SerializedNode : public ISerializable
{
public:

    //Unmanaged raw pointer. Needs to be managed by whoever called "ReadData" on this instance.
    DataNode* Node;
    SerializedNode(DataNode* node = 0) : Node(node) { }

    virtual void WriteData(DataWriter* writer) const override
    {
        writer->WriteString(Node->GetTypeName(), "Node Type");
        writer->WriteDataStructure(*Node, "Node Data");
    }
    virtual void ReadData(DataReader* reader) override
    {
        std::string nodeType;
        reader->ReadString(nodeType);

        Node = DataNode::CreateNode(nodeType);
        if (Node == 0)
        {
            reader->ErrorMessage = "Node type '" + nodeType +
                                        "' doesn't exist or doesn't have its " +
                                        "reflection data set properly";
            throw DataReader::EXCEPTION_FAILURE;
        }

        reader->ReadDataStructure(*Node);
    }
};


void ShaderOutput::WriteData(DataWriter* writer) const
{
    writer->WriteString(Name, "Output name");
    writer->WriteDataStructure(Value, "Output value");
}
void ShaderOutput::ReadData(DataReader* reader)
{
    reader->ReadString(Name);
    reader->ReadDataStructure(Value);
}


void MaterialOutputs::ClearData(void)
{
    VertexPosOutput = DataLine();
    VertexOutputs.clear();
    FragmentOutputs.clear();
}

#pragma warning(disable: 4100)
void MaterialOutputs::WriteData(DataWriter* writer) const
{
    writer->WriteDataStructure(VertexPosOutput, "Vertex position output");

    writer->WriteUInt(VertexOutputs.size(), "Number of vertex outputs");
    for (unsigned int i = 0; i < VertexOutputs.size(); ++i)
    {
        writer->WriteDataStructure(VertexOutputs[i], "Vertex output #" + std::to_string(i + 1));
    }

    writer->WriteUInt(FragmentOutputs.size(), "Number of fragment outputs");
    for (unsigned int i = 0; i < FragmentOutputs.size(); ++i)
    {
        writer->WriteDataStructure(FragmentOutputs[i], "Fragment output #" + std::to_string(i + 1));
    }
}
void MaterialOutputs::ReadData(DataReader* reader)
{
    reader->ReadDataStructure(VertexPosOutput);

    unsigned int nVerts;
    reader->ReadUInt(nVerts);
    VertexOutputs.resize(nVerts);
    for (unsigned int i = 0; i < nVerts; ++i)
    {
        reader->ReadDataStructure(VertexOutputs[i]);
    }

    unsigned int nFrags;
    reader->ReadUInt(nFrags);
    FragmentOutputs.resize(nFrags);
    for (unsigned int i = 0; i < nFrags; ++i)
    {
        reader->ReadDataStructure(FragmentOutputs[i]);
    }
}
#pragma warning(default: 4100)


void SerializedMaterial::WriteData(DataWriter* writer) const
{
    writer->WriteDataStructure(RenderIOAttributes_Writable(VertexInputs), "Vertex inputs");


    //Get all the "root" nodes (i.e. all the outputs into the material).
    std::vector<DataNode*> rootNodes;
    if (!MaterialOuts.VertexPosOutput.IsConstant())
    {
        rootNodes.insert(rootNodes.end(), MaterialOuts.VertexPosOutput.GetNode());
    }
    for (unsigned int i = 0; i < MaterialOuts.VertexOutputs.size(); ++i)
    {
        if (!MaterialOuts.VertexOutputs[i].Value.IsConstant())
        {
            rootNodes.insert(rootNodes.end(), MaterialOuts.VertexOutputs[i].Value.GetNode());
        }
    }
    for (unsigned int i = 0; i < MaterialOuts.FragmentOutputs.size(); ++i)
    {
        if (!MaterialOuts.FragmentOutputs[i].Value.IsConstant())
        {
            rootNodes.insert(rootNodes.end(), MaterialOuts.FragmentOutputs[i].Value.GetNode());
        }
    }


    //Define each node's "max depth" as the length of the longest chain from the root node
    //   to that node.
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
            NodeAndDepth(DataNode* n = 0, unsigned int d = 0) : Node(n), Depth(d) { }
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
            bool operator==(const Traversal& other) const
            {
                return Start == other.Start && End == other.End;
            }
#pragma warning(disable: 4100)
            unsigned int operator()(const Traversal& v) const
            {
                return Vector2i((int)Start, (int)End).GetHashCode();
            }
#pragma warning(default: 4100)
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
            if (DataNode::IsSingletonType(toSearch.Node))
            {
                continue;
            }
            //If the node already exists, and doesn't have a greater depth here,
            //   then there is no need to traverse its children again.
            auto found = nodesAndDepth.find(toSearch.Node);
            if (found != nodesAndDepth.end() && found->second >= toSearch.Depth)
            {
                continue;
            }

            //Set/update the node's entry in the graph.
            nodesAndDepth[toSearch.Node] = toSearch.Depth;
            maxDepth = Mathf::Max(maxDepth, toSearch.Depth);

            //Add the node's DataNode children (not its constant VectorF children)
            //    to the search space.
            for (unsigned int i = 0; i < toSearch.Node->GetInputs().size(); ++i)
            {
                if (!toSearch.Node->GetInputs()[i].IsConstant())
                {
                    DataNode* child = toSearch.Node->GetInputs()[i].GetNode();

                    //Make sure the input actually exists.
                    if (child == 0)
                    {
                        writer->ErrorMessage = "Input node '" +
                                                    toSearch.Node->GetInputs()[i].GetNonConstantValue() +
                                                    "' of node '" + toSearch.Node->GetName() +
                                                    "' doens't exist!";
                        throw DataWriter::EXCEPTION_FAILURE;
                    }

                    //Make sure this path hasn't been traversed already.
                    Traversal trvs(toSearch.Node, child);
                    if (connectionsSoFar.find(trvs) != connectionsSoFar.end())
                    {
                        //If there are more connections between these nodes than there should be,
                        //   then this traversal has happened more than once -- i.e.,
                        //   there is an infinite loop.
                        if (connectionsSoFar[trvs] >= connectionsPerTraversal[trvs])
                        {
                            writer->ErrorMessage = "Infinite loop detected: The link between '" +
                                                        toSearch.Node->GetName() + "' and its input '" +
                                                        child->GetName() +
                                                        "' has been traversed more than once.";
                            throw DataWriter::EXCEPTION_FAILURE;
                        }
                        else
                        {
                            connectionsSoFar[trvs] += 1;
                        }
                    }
                    else
                    {
                        connectionsSoFar[trvs] = 1;

                        //Count how many connections there should be for this traversal.
                        unsigned int connPerTrav = 0;
                        for (unsigned int j = 0; j < toSearch.Node->GetInputs().size(); ++j)
                        {
                            DataLine& inp = toSearch.Node->GetInputs()[j];
                            if (!inp.IsConstant() && inp.GetNode() == trvs.End)
                            {
                                connPerTrav += 1;
                            }
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
    writer->WriteUInt(nodesAndDepth.size(), "Number of nodes");

    //Now that we have all nodes sorted by their depth, start with the lowest nodes
    //    (i.e. the fewest dependencies) and work up.
    for (int depth = (int)maxDepth; depth >= 0; --depth)
    {
        unsigned int depthU = (unsigned int)depth;

        //Get all nodes of this depth and write them.
        for (auto element = nodesAndDepth.begin(); element != nodesAndDepth.end(); ++element)
        {
            if (element->second == depthU)
            {
                //Generate a description of the outputs.
                std::string outputDescription = std::to_string(element->first->GetNumbOutputs()) +
                                                    " Outputs: ";
                for (unsigned int i = 0; i < element->first->GetNumbOutputs(); ++i)
                {
                    if (i > 0) outputDescription += ", ";
                    outputDescription += std::to_string(element->first->GetOutputSize(i));
                }

                //Write the node.
                writer->WriteDataStructure(SerializedNode(element->first), outputDescription);
            }
        }
    }

    //Now just write out the output declarations.
    writer->WriteDataStructure(MaterialOuts, "Material outputs");
}
void SerializedMaterial::ReadData(DataReader* reader)
{
    reader->ReadDataStructure(RenderIOAttributes_Readable(VertexInputs));

    unsigned int nNodes;
    reader->ReadUInt(nNodes);

    nodeStorage.clear();
    for (unsigned int i = 0; i < nNodes; ++i)
    {
        SerializedNode serNode;
        reader->ReadDataStructure(serNode);

        nodeStorage.push_back(DataNode::Ptr(serNode.Node));
    }

    //Try to read in the material outputs.
    reader->ReadDataStructure(MaterialOuts);
}