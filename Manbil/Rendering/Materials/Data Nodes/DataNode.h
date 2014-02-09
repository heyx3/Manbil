#pragma once

#include <vector>
#include <assert.h>
#include <memory>
#include "../../../Math/Vectors.h"
#include "DataLine.h"




//Represents a basic, atomic operation in a shader.
class DataNode
{
public:

    typedef std::shared_ptr<DataNode> DataNodePtr;

    //Gets the identifier unique for this dataNode.
    unsigned int GetUniqueID(void) const { return id; }

    //Gets the name of this data node. Only used for commenting the generated shader code.
    virtual std::string GetName(void) const { return "unknown DataNode"; }


    DataNode(const std::vector<DataLine> & _inputs, const std::vector<unsigned int> & outputSizes)
        : id(GetNextID()), inputs(_inputs), outputs(outputSizes) { }
    DataNode(const DataNode & cpy); // Intentionally left blank.


    //Gets any parameters/uniforms this node definesi.
    virtual void GetParameterDeclarations(std::vector<std::string> & outDecls) { }
    //Gets any GLSL helper function declarations this node needs to use.
    virtual void GetFunctionDeclarations(std::vector<std::string> & outDecls) { }
    //The bit of code that will actually generate the glsl code.
    //Appends the code to the end of "outCode". Takes in a list of all nodes that have already added their code to "outCode".
    WriteOutputs(std::string & outCode, std::vector<unsigned int> & writtenNodeIDs) const
    {
        //First write out all the child data nodes that haven't been written out yet.
        for (int i = 0; i < inputs.size(); ++i)
        {
            if (!inputs[i].IsConstant())
            {
                DataNodePtr input = inputs[i].GetDataNodeValue();

                if (std::find(writtenNodeIDs.begin(), writtenNodeIDs.end(), input->id) == writtenNodeIDs.end())
                {
                    writtenNodeIDs.insert(writtenNodeIDs.end(), input->id);

                    input->WriteOutputConnection()
                }
            }
        }

        outCode.append(WriteMyOutputConnections(outputNames));
    }


    //Gets this node's input lines.
    const std::vector<DataLine> & GetInputs(void) const { return inputs; }
    //Gets this node's output lines. Each element represents the size of that output line's Vector.
    const std::vector<unsigned int> & GetOutputs(void) const { return outputs; }

    //Gets the variable name for this node's given output.
    std::string GetOutputName(unsigned int outputIndex) const { return GetName() + "_" + std::to_string(id) + "_" + std::to_string(outputIndex); }


protected:

    static std::vector<DataLine> MakeVector(const DataLine & dat);
    static std::vector<DataLine> MakeVector(const DataLine & dat, const DataLine & dat2);
    static std::vector<DataLine> MakeVector(const DataLine & dat, const DataLine & dat2, const DataLine & dat3);

    static std::vector<unsigned int> MakeVector(unsigned int dat);
    static std::vector<unsigned int> MakeVector(unsigned int dat, unsigned int dat2);
    static std::vector<unsigned int> MakeVector(unsigned int dat, unsigned int dat2, unsigned int dat3);


    virtual std::string WriteMyOutputConnections(void) const = 0;


private:

    std::vector<DataLine> inputs;
    std::vector<unsigned int> outputs;


    static unsigned int nextID;
    static unsigned int GetNextID(void) { unsigned int id = nextID; nextID += 1; return id; }

    unsigned int id;
};