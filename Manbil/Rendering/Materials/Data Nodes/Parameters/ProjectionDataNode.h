#pragma once

#include "../DataNode.h"


//Outputs projection info (i.e. width/height, zNear/zFar, and FOV).
class ProjectionDataNode : public DataNode
{
public:

    static DataNodePtr GetInstance(void) { return instance; }
    static DataLine GetWidth(void) { return DataLine(instance->GetName(), GetWidthOutputIndex()); }
    static DataLine GetHeight(void) { return DataLine(instance->GetName(), GetHeightOutputIndex()); }
    static DataLine GetZNear(void) { return DataLine(instance->GetName(), GetZNearOutputIndex()); }
    static DataLine GetZFar(void) { return DataLine(instance->GetName(), GetZFarOutputIndex()); }
    static DataLine GetFov(void) { return DataLine(instance->GetName(), GetFovOutputIndex()); }


    virtual std::string GetTypeName(void) const override { return "projectionData"; }

    virtual unsigned int GetNumbOutputs(void) const override { return 5; }

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override;

#pragma warning(disable: 4100)
    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        //No output writing needed.
    }
#pragma warning(default: 4100)

private:

    static DataNodePtr instance;
    static unsigned int GetWidthOutputIndex(void) { return 0; }
    static unsigned int GetHeightOutputIndex(void) { return 1; }
    static unsigned int GetZNearOutputIndex(void) { return 2; }
    static unsigned int GetZFarOutputIndex(void) { return 3; }
    static unsigned int GetFovOutputIndex(void) { return 4; }

    ProjectionDataNode(void)
        : DataNode(std::vector<DataLine>(), [](std::vector<DataLine> & i, std::string n) { return instance; }, "projectionUniforms") { }
};