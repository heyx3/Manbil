#include "DataNode.h"

#include "../../Math/Lower Math/FastRand.h"
#include <time.h>


unsigned int GetDataNodeLastID(void)
{
    time_t now;
    time(&now);

    //'now' is type 'long', so just grab the first half of it and interpret that. 
    unsigned int hash = *(unsigned int*)(&now);

    return Mathf::Abs(FastRand((int)hash).GetRandInt());
}
unsigned int DataNode::lastID = GetDataNodeLastID();

int DataNode::EXCEPTION_ASSERT_FAILED = 1352;


MaterialOutputs DataNode::MaterialOuts = MaterialOutputs();
GeoShaderData DataNode::GeometryShader = GeoShaderData();
RenderIOAttributes DataNode::VertexIns = RenderIOAttributes();

Shaders DataNode::CurrentShader = SH_VERTEX;

std::unordered_map<std::string, DataNode*>* DataNode::DataNode_nameToNode = 0;
std::unordered_map<std::string, DataNode::NodeFactory>* DataNode::DataNode_factoriesByTypename = 0;


bool DataNode::IsSingletonType(std::string typeName)
{
    auto found = DataNode_factoriesByTypename->find(typeName);
    assert(found != DataNode_factoriesByTypename->end());

    //The node is a singleton if its factory is undefined.
    return found->second == 0;
}

DataNode* DataNode::GetNode(std::string name)
{
    auto found = DataNode_nameToNode->find(name);
    if (found == DataNode_nameToNode->end())
    {
        return 0;
    }
    return found->second;
}

DataNode* DataNode::CreateNode(std::string typeName)
{
    auto found = DataNode_factoriesByTypename->find(typeName);
    if (found == DataNode_factoriesByTypename->end())
    {
        return 0;
    }
    return found->second();
}


DataNode::DataNode(const std::vector<DataLine>& _inputs, std::string _name)
    : inputs(_inputs), name((_name.size() == 0) ? ("node" + std::to_string(GenerateUniqueID())) : _name)
{
    if (DataNode_nameToNode == 0)
    {
        DataNode_nameToNode = new std::unordered_map<std::string, DataNode*>();
    }

    Assert(DataNode_nameToNode->find(name) == DataNode_nameToNode->end(),
           "A node with the name '" + name + "' already exists!");
    DataNode_nameToNode->operator[](name) = this;
}
DataNode::~DataNode(void)
{
    DataNode_nameToNode->erase(name);
}

void DataNode::ReplaceInput(unsigned int inputIndex, const DataLine& replacement)
{
    assert(inputs.size() > inputIndex);
    inputs[inputIndex] = replacement;
}


void DataNode::AssertAllInputsValid(void) const
{
    for (unsigned int i = 0; i < inputs.size(); ++i)
    {
        if (!inputs[i].IsConstant())
        {
            std::string* outError;
            try
            {
                outError = &errorMsg;
                auto found = DataNode_nameToNode->find(inputs[i].GetNonConstantValue());
                Assert(found != DataNode_nameToNode->end(),
                       "The input named '" + inputs[i].GetNonConstantValue() + "' doesn't exist!");
                Assert(inputs[i].GetNonConstantOutputIndex() < found->second->GetNumbOutputs(),
                       "The input named '" + found->second->GetName() + "' only has " +
                           ToString(found->second->GetNumbOutputs()) +
                           " outputs, but the output index was " +
                           ToString(inputs[i].GetNonConstantOutputIndex()));

                outError = &found->second->errorMsg;
                found->second->AssertAllInputsValid();
            }
            catch (int ex)
            {
                assert(ex == EXCEPTION_ASSERT_FAILED);
                errorMsg = "Error with asserting inputs for input#" + ToString(i + 1) + ", '" +
                               inputs[i].GetNonConstantValue() + "':\n" + *outError;
                throw EXCEPTION_ASSERT_FAILED;
            }
        }
    }

    AssertMyInputsValid();
}

void DataNode::Assert(bool toAssert, const std::string& error) const
{
    if (!toAssert)
    {
        errorMsg = error;
        throw EXCEPTION_ASSERT_FAILED;
    }
}

void DataNode::SetFlags(MaterialUsageFlags& flags, unsigned int outputIndex) const
{
    for (unsigned int input = 0; input < inputs.size(); ++input)
    {
        if (!inputs[input].IsConstant() && UsesInput(input, outputIndex))
        {
            std::string inName = inputs[input].GetNonConstantValue();
            const std::string* outError;
            try
            {
                outError = &errorMsg;
                auto found = DataNode_nameToNode->find(inName);
                Assert(found != DataNode_nameToNode->end(),
                       "The input named '" + inName + "' doesn't exist!");

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

void DataNode::GetParameterDeclarations(UniformDictionary& outUniforms,
                                        std::vector<const DataNode*>& writtenNodes) const
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
            const std::string* outError;
            try
            {
                outError = &errorMsg;
                auto found = DataNode_nameToNode->find(inName);
                Assert(found != DataNode_nameToNode->end(),
                       "The input named '" + inName + "' doesn't exist!");

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
void DataNode::GetFunctionDeclarations(std::vector<std::string>& outDecls,
                                       std::vector<const DataNode*>& writtenNodes) const
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
            const std::string* outError;
            try
            {
                outError = &errorMsg;
                auto found = DataNode_nameToNode->find(inName);
                Assert(found != DataNode_nameToNode->end(),
                       "The input named '" + inName + "' doesn't exist!");

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
void DataNode::WriteOutputs(std::string& outCode, std::vector<const DataNode*>& writtenNodes) const
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
            const std::string* outError;
            try
            {
                outError = &errorMsg;
                auto found = DataNode_nameToNode->find(inName);
                Assert(found != DataNode_nameToNode->end(),
                       "The input named '" + inName + "' doesn't exist!");

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
    auto found = DataNode_nameToNode->find(name);
    Assert(found != DataNode_nameToNode->end(),
           "Somehow this node's name ('" + name + "') isn't in the static dictionary!");

    Assert(DataNode_nameToNode->find(newName) == DataNode_nameToNode->end(),
           "The name '" + newName + "' already exists!");

    name = newName;

    DataNode_nameToNode->erase(found);
    DataNode_nameToNode->operator[](name) = this;
}


void DataNode::WriteData(DataWriter* writer) const
{
    writer->WriteString(name, "Unique node name");
    
    writer->WriteUInt(inputs.size(), "Number of inputs");

    for (unsigned int i = 0; i < inputs.size(); ++i)
    {
        writer->WriteDataStructure(inputs[i], "Input: '" + GetInputDescription(i) + "'");
    }
    WriteExtraData(writer);
}
void DataNode::ReadData(DataReader* reader)
{
    reader->ReadString(name);

    //Make sure the name doesn't already belong to another node.
    if (DataNode_nameToNode->find(name) != DataNode_nameToNode->end())
    {
        reader->ErrorMessage = "A node already exists with the name '" + name + "'";
        throw DataReader::EXCEPTION_FAILURE;
    }

    unsigned int nInputs;
    reader->ReadUInt(nInputs);

    //Read in each input.
    inputs.clear();
    inputs.resize(nInputs);
    for (unsigned int inp = 0; inp < nInputs; ++inp)
    {
        reader->ReadDataStructure(inputs[inp]);
    }

    ReadExtraData(reader);
}

bool DataNode::UsesInput(unsigned int inputIndex) const
{
    Assert(inputIndex < inputs.size(),
           "Input index " + std::to_string(inputIndex) +
                " is too big! The largest index should be " + std::to_string(inputs.size() - 1));
    return true;
}
bool DataNode::UsesInput(unsigned int inputIndex, unsigned int outputIndex) const
{
    Assert(inputIndex < inputs.size() && outputIndex < GetNumbOutputs(),
           "Input index " + std::to_string(inputIndex) +
               " or output index " + std::to_string(outputIndex) + " are too big! " +
               "The input index should be at most " + std::to_string(inputs.size() - 1) +
               " and the output index should be at most " + std::to_string(GetNumbOutputs() - 1));
    return true;
}

std::string DataNode::GetOutputName(unsigned int outputIndex) const
{
    return name + "_out" + std::to_string(outputIndex);
}

std::string DataNode::GetInputDescription(unsigned int index) const
{
    return "in" + ToString(index + 1);
}

std::string DataNode::ToString(Shaders value)
{
    switch (value)
    {
        case SH_VERTEX: return "Vertex";
        case SH_FRAGMENT: return "Fragment";
        case SH_GEOMETRY: return "Geometry";
        default:
            assert(false);
            return "UNKONWN_SHADER_TYPE";
    }
}
std::string DataNode::ToString(unsigned int value)
{
    return std::to_string(value);
}

std::vector<DataLine> DataNode::MakeVector(const DataLine& dat)
{
    std::vector<DataLine> dats;
    dats.insert(dats.end(), dat);
    return dats;
}
std::vector<DataLine> DataNode::MakeVector(const DataLine& dat, const DataLine& dat2)
{
    std::vector<DataLine> dats;
    dats.insert(dats.end(), dat);
    dats.insert(dats.end(), dat2);
    return dats;
}
std::vector<DataLine> DataNode::MakeVector(const DataLine& dat, const DataLine& dat2,
                                           const DataLine& dat3)
{
    std::vector<DataLine> dats;
    dats.insert(dats.end(), dat);
    dats.insert(dats.end(), dat2);
    dats.insert(dats.end(), dat3);
    return dats;
}
std::vector<DataLine> DataNode::MakeVector(const DataLine& dat, const DataLine& dat2,
                                           const DataLine& dat3, const DataLine& dat4)
{
    std::vector<DataLine> dats;
    dats.insert(dats.end(), dat);
    dats.insert(dats.end(), dat2);
    dats.insert(dats.end(), dat3);
    dats.insert(dats.end(), dat4);
    return dats;
}
std::vector<DataLine> DataNode::MakeVector(const DataLine& dat, unsigned int wherePut,
                                           const std::vector<DataLine>& moreDats)
{
    std::vector<DataLine> cpy = moreDats;

    cpy.insert(cpy.begin() + wherePut, dat);

    return cpy;
}