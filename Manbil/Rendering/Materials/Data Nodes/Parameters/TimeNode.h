#pragma once

#include "../DataNode.h"

#include "../../MaterialData.h"


#pragma warning(disable: 4100)

//Outputs the uniform for the elapsed time.
class TimeNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "timeNode"; }

    TimeNode(void) : DataNode(std::vector<DataLine>(), MakeVector(1)) { }

    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, std::string() + "Invalid output index " + ToString(index));
        return MaterialConstants::ElapsedTimeName;
    }


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_TIME);
    }

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        //No outputting needed.
    }
};

#pragma warning(default: 4100)