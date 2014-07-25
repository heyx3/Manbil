#pragma once

#include "../DataNode.h"


//Outputs projection info (i.e. width/height, zNear/zFar, and FOV).
class ProjectionDataNode : public DataNode
{
public:

    static DataNodePtr GetInstance(void) { return instance; }
    static DataLine GetWidth(void) { return DataLine(GetInstance(), GetWidthOutputIndex()); }
    static DataLine GetHeight(void) { return DataLine(GetInstance(), GetHeightOutputIndex()); }
    static DataLine GetZNear(void) { return DataLine(GetInstance(), GetZNearOutputIndex()); }
    static DataLine GetZFar(void) { return DataLine(GetInstance(), GetZFarOutputIndex()); }
    static DataLine GetFov(void) { return DataLine(GetInstance(), GetFovOutputIndex()); }


    virtual std::string GetName(void) const override { return "projectionDataNode"; }
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

    static std::vector<unsigned int> MakeOutputs(void);

    static DataNodePtr instance;
    static unsigned int GetWidthOutputIndex(void) { return 0; }
    static unsigned int GetHeightOutputIndex(void) { return 1; }
    static unsigned int GetZNearOutputIndex(void) { return 2; }
    static unsigned int GetZFarOutputIndex(void) { return 3; }
    static unsigned int GetFovOutputIndex(void) { return 4; }

    ProjectionDataNode(void) : DataNode(std::vector<DataLine>(), MakeOutputs()) { }
};