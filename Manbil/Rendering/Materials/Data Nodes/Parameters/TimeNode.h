#pragma once

#include "../DataNode.h"

#include "../../MaterialData.h"


//Outputs the uniform for the elapsed time.
class TimeNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "timeNode"; }

    TimeNode(void) : DataNode(std::vector<DataLine>(), MakeVector(1)) { }

    virtual std::string GetOutputName(unsigned int index, Shaders shaderType) const override { assert(index == 0); return MaterialConstants::ElapsedTimeName; }


protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override
    {
        //No outputting needed.
    }
};