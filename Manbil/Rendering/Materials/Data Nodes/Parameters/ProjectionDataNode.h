#pragma once

#include "../DataNode.h"


//Outputs projection info (i.e. width/height, zNear/zFar, and FOV).
class ProjectionDataNode : public DataNode
{
public:

    static unsigned int GetWidthOutputIndex(void) { return 0; }
    static unsigned int GetHeightOutputIndex(void) { return 1; }
    static unsigned int GetZNearOutputIndex(void) { return 2; }
    static unsigned int GetZFarOutputIndex(void) { return 3; }
    static unsigned int GetFovOutputIndex(void) { return 4; }

    ProjectionDataNode(void) : DataNode(std::vector<DataLine>(), MakeOutputs()) { }

    virtual std::string GetName(void) const override { return "projectionDataNode"; }
    virtual std::string GetOutputName(unsigned int index) const override;


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        //No output writing needed.
    }

private:

    static std::vector<unsigned int> MakeOutputs(void)
    {
        std::vector<unsigned int> ret;
        for (unsigned int i = 0; i < 5; ++i)
            ret.insert(ret.end(), 1);
        return ret;
    }
};