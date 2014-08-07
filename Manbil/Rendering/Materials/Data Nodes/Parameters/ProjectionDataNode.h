#pragma once

#include "../DataNode.h"


//Outputs projection info (i.e. width/height, zNear/zFar, and FOV).
class ProjectionDataNode : public DataNode
{
public:

    static std::string GetInstanceName(void) { return "projectionDataNode"; }
    static std::shared_ptr<DataNode> GetInstance(void) { return instance; }
    static DataLine GetWidth(void) { return DataLine(instance->GetName(), GetWidthOutputIndex()); }
    static DataLine GetHeight(void) { return DataLine(instance->GetName(), GetHeightOutputIndex()); }
    static DataLine GetZNear(void) { return DataLine(instance->GetName(), GetZNearOutputIndex()); }
    static DataLine GetZFar(void) { return DataLine(instance->GetName(), GetZFarOutputIndex()); }
    static DataLine GetFov(void) { return DataLine(instance->GetName(), GetFovOutputIndex()); }


    virtual unsigned int GetNumbOutputs(void) const override { return 5; }

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override;
    virtual void WriteMyOutputs(std::string & outCode) const override;

private:

    static std::shared_ptr<DataNode> instance;
    static unsigned int GetWidthOutputIndex(void) { return 0; }
    static unsigned int GetHeightOutputIndex(void) { return 1; }
    static unsigned int GetZNearOutputIndex(void) { return 2; }
    static unsigned int GetZFarOutputIndex(void) { return 3; }
    static unsigned int GetFovOutputIndex(void) { return 4; }

    ProjectionDataNode(void);


    MAKE_NODE_READABLE_H(ProjectionDataNode)
};