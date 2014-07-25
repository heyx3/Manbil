#pragma once

#include "../DataNode.h"

#include "../../MaterialData.h"


#pragma warning(disable: 4100)

//Outputs the uniform for the elapsed time.
class TimeNode : public DataNode
{
public:

    static DataNodePtr GetInstance(void) { return instance; }
    static DataLine GetTime(void) { return DataLine(instance, 0); }

    virtual std::string GetName(void) const override { return "timeNode"; }

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

private:

    static DataNodePtr instance;

    TimeNode(void) : DataNode(std::vector<DataLine>(), MakeVector(1)) { }
};

#pragma warning(default: 4100)