#pragma once

#include "../DataNode.h"

#include "../../Basic Rendering/MaterialData.h"


#pragma warning(disable: 4100)

//Outputs the uniform for the elapsed time.
class TimeNode : public DataNode
{
public:

    static std::string GetInstanceName(void) { return "timeParamNode"; }
    static std::shared_ptr<DataNode> GetInstance(void) { return instance; }
    static DataLine GetTime(void) { return DataLine(instance->GetName(), 0); }


    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override;
    virtual void WriteMyOutputs(std::string & outCode) const override;


private:

    static std::shared_ptr<DataNode> instance;

    TimeNode(void);

    ADD_NODE_REFLECTION_DATA_H(TimeNode)
};

#pragma warning(default: 4100)