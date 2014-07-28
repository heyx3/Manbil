#include "DataNode.h"


DataNode::Shaders DataNode::shaderType = DataNode::Shaders::SH_Vertex_Shader;
const GeoShaderData * DataNode::geoData = 0;
unsigned int DataNode::lastID = 0;

int DataNode::EXCEPTION_ASSERT_FAILED = 1352;


DataNode* DataNode::GetNode(std::string name)
{
    auto found = nameToNode.find(name);
    if (found == nameToNode.end()) return 0;
    return found->second;
}


DataNode::DataNode(const std::vector<DataLine> & _inputs, std::string _name)
    : inputs(_inputs), name((_name.size() == 0) ? ("node" + std::to_string(GenerateUniqueID())) : _name)
{
    Assert(nameToNode.find(name) == nameToNode.end(),
           "A node with the name '" + name + "' already exists!");
    nameToNode[name] = this;

    ResetOutputs(outputs);
}
DataNode::~DataNode(void)
{
    nameToNode.erase(name);
}

void DataNode::SetFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const
{
    Assert(outputIndex < outputs.size(),
           std::string() + "Output index " + std::to_string(outputIndex) +
             " is too big. The max output index value is " + std::to_string(outputs.size() - 1));

    for (unsigned int input = 0; input < inputs.size(); ++input)
    {
        if (!inputs[input].IsConstant() && UsesInput(input, outputIndex))
        {
            std::string inName = inputs[input].GetNonConstantValue();
            const std::string * outError;
            try
            {
                outError = &errorMsg;
                auto found = nameToNode.find(inName);
                Assert(found != nameToNode.end(), "The input named '" + inName + "' doesn't exist!");

                outError = &found->second->errorMsg;
                found->second->SetFlags(flags, inputs[input].GetNonConstantOutputIndex());
            }
            catch (int ex)
            {
                assert(ex == EXCEPTION_ASSERT_FAILED);
                errorMsg = "Error with setting flags for input #" + std::to_string(input + 1) +
                                ", '" + inName + "':\n  " + *outError;
                throw EXCEPTION_ASSERT_FAILED;
            }
        }
    }

    SetMyFlags(flags, outputIndex);
}

void DataNode::GetParameterDeclarations(UniformDictionary & outUniforms, std::vector<const DataNode*> & writtenNodes) const
{
    //Exit if this node has already been used.
    if (std::find(writtenNodes.begin(), writtenNodes.end(), this) != writtenNodes.end())
        return;

    //First get the parameter declarations for all the child nodes.
    for (unsigned int i = 0; i < inputs.size(); ++i)
    {
        if (!inputs[i].IsConstant() && UsesInput(i))
        {
            std::string inName = inputs[i].GetNonConstantValue();
            const std::string * outError;
            try
            {
                outError = &errorMsg;
                auto found = nameToNode.find(inName);
                Assert(found != nameToNode.end(), "The input named '" + inName + "' doesn't exist!");

                outError = &found->second->errorMsg;
                found->second->GetParameterDeclarations(outUniforms, writtenNodes);
            }
            catch (int ex)
            {
                assert(ex == EXCEPTION_ASSERT_FAILED);
                errorMsg = "Error with declaring uniforms for input #" + std::to_string(i + 1) +
                                ", '" + inName + "':\n  " + *outError;
                throw EXCEPTION_ASSERT_FAILED;
            }
        }
    }

    //Now get this node's own parameter declarations.
    writtenNodes.insert(writtenNodes.end(), this);
    GetMyParameterDeclarations(outUniforms);
}
void DataNode::GetFunctionDeclarations(std::vector<std::string> & outDecls, std::vector<const DataNode*> & writtenNodes) const
{
    //Exit if this node has already been used.
    if (std::find(writtenNodes.begin(), writtenNodes.end(), this) != writtenNodes.end())
        return;

    //First get the function declarations for all the child nodes.
    for (unsigned int i = 0; i < inputs.size(); ++i)
    {
        if (!inputs[i].IsConstant() && UsesInput(i))
        {
            std::string inName = inputs[i].GetNonConstantValue();
            const std::string * outError;
            try
            {
                outError = &errorMsg;
                auto found = nameToNode.find(inName);
                Assert(found != nameToNode.end(), "The input named '" + inName + "' doesn't exist!");

                outError = &found->second->errorMsg;
                found->second->GetFunctionDeclarations(outDecls, writtenNodes);
            }
            catch (int ex)
            {
                assert(ex == EXCEPTION_ASSERT_FAILED);
                errorMsg = "Error with declaring functions for input #" + std::to_string(i + 1) +
                                ", " + inName + "':\n  " + *outError;
                throw EXCEPTION_ASSERT_FAILED;
            }
        }
    }

    //Now get this node's own function declarations.
    writtenNodes.insert(writtenNodes.end(), this);
    GetMyFunctionDeclarations(outDecls);
}
void DataNode::WriteOutputs(std::string & outCode, std::vector<const DataNode*> & writtenNodes) const
{
    //Exit if this node has already been used.
    if (std::find(writtenNodes.begin(), writtenNodes.end(), this) != writtenNodes.end())
        return;

    //First get the outputs for all the child nodes.
    for (unsigned int i = 0; i < inputs.size(); ++i)
    {
        if (!inputs[i].IsConstant() && UsesInput(i))
        {
            std::string inName = inputs[i].GetNonConstantValue();
            const std::string * outError;
            try
            {
                outError = &errorMsg;
                auto found = nameToNode.find(inName);
                Assert(found != nameToNode.end(), "The input named '" + inName + "' doesn't exist!");

                outError = &found->second->errorMsg;
                found->second->WriteOutputs(outCode, writtenNodes);
            }
            catch (int ex)
            {
                assert(ex == EXCEPTION_ASSERT_FAILED);
                errorMsg = "Error with writing outputs for input #" + std::to_string(i + 1) +
                                ", '" + inName + "':\n  " + *outError;
                throw EXCEPTION_ASSERT_FAILED;
            }
        }
    }

    //Now get this node's own outputs.
    writtenNodes.insert(writtenNodes.end(), this);
    WriteMyOutputs(outCode);
}


void DataNode::SetName(std::string newName)
{
    auto found = nameToNode.find(name);
    Assert(found != nameToNode.end(), "Somehow this node's name ('" + name + "') isn't in the static dictionary!");

    Assert(nameToNode.find(newName) == nameToNode.end(),
           "The name '" + newName + "' already exists!");

    name = newName;

    nameToNode.erase(found);
    nameToNode[name] = this;
}


bool DataNode::WriteData(DataWriter * writer, std::string & outError) const
{
    //Write the name of this node.
    if (!writer->WriteString(name, "nodeName", outError))
    {
        outError = "Error writing out this node's name, '" + name + "': " + outError;
        return false;
    }

    //Write the number of inputs.
    if (!writer->WriteUInt(inputs.size(), "numberOfInputs", outError))
    {
        outError = "Error writing out the number of inputs (" + std::to_string(inputs.size()) + ": " + outError;
        return false;
    }

    //Write each input value.
    for (unsigned int i = 0; i < inputs.size(); ++i)
    {
        std::string desc = GetInputDescription(i);
        if (!writer->WriteDataStructure(inputs[i], desc, outError))
        {
            outError = "Error reading input '" + desc + "': " + outError;
            return false;
        }
    }

    //Write any extra data specific to each type of node.
    if (!WriteExtraData(writer, outError)) return false;

    return true;
}
bool DataNode::ReadData(DataReader * reader, std::string & outError)
{
    //Get the name of this node.
    MaybeValue<std::string> tryName = reader->ReadString(outError);
    if (!tryName.HasValue())
    {
        outError = "Error reading in this node's name: " + outError;
        return false;
    }
    //Make sure the name doesn't already belong to another node.
    if (nameToNode.find(tryName.GetValue()) != nameToNode.end())
    {
        outError = "A node already exists with the name '" + tryName.GetValue() + "'";
        return false;
    }

    SetName(tryName.GetValue());

    //Read in the total number of inputs.
    MaybeValue<unsigned int> tryNumbIns = reader->ReadUInt(outError);
    if (!tryNumbIns.HasValue())
    {
        outError = "Error reading in the number of inputs: " + outError;
        return false;
    }

    //Read in each input.
    inputs.clear();
    inputs.resize(tryNumbIns.GetValue());
    for (unsigned int inp = 0; inp < tryNumbIns.GetValue(); ++inp)
    {
        if (!reader->ReadDataStructure(inputs[inp], outError))
        {
            outError = "Error reading input '" + GetInputDescription(inp) + "': " + outError;
            return false;
        }
    }

    if (!ReadExtraData(reader, outError)) return false;

    outputs.clear();
    ResetOutputs(outputs);

    return true;
}



std::vector<DataLine> DataNode::MakeVector(const DataLine & dat)
{
    std::vector<DataLine> dats;
    dats.insert(dats.end(), dat);
    return dats;
}
std::vector<DataLine> DataNode::MakeVector(const DataLine & dat, const DataLine & dat2)
{
    std::vector<DataLine> dats;
    dats.insert(dats.end(), dat);
    dats.insert(dats.end(), dat2);
    return dats;
}
std::vector<DataLine> DataNode::MakeVector(const DataLine & dat, const DataLine & dat2, const DataLine & dat3)
{
    std::vector<DataLine> dats;
    dats.insert(dats.end(), dat);
    dats.insert(dats.end(), dat2);
    dats.insert(dats.end(), dat3);
    return dats;
}
std::vector<DataLine> DataNode::MakeVector(const DataLine & dat, const DataLine & dat2, const DataLine & dat3, const DataLine & dat4)
{
    std::vector<DataLine> dats;
    dats.insert(dats.end(), dat);
    dats.insert(dats.end(), dat2);
    dats.insert(dats.end(), dat3);
    dats.insert(dats.end(), dat4);
    return dats;
}
std::vector<DataLine> DataNode::MakeVector(const DataLine & dat, unsigned int wherePut, const std::vector<DataLine> & moreDats)
{
    std::vector<DataLine> cpy = moreDats;

    cpy.insert(cpy.begin() + wherePut, dat);

    return cpy;
}