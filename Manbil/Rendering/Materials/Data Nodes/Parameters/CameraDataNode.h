#pragma once

#include "../DataNode.h"


//Outputs different camera position, as well as forward/upwards/sideways vectors.
class CameraDataNode : public DataNode
{
public:

    static std::shared_ptr<DataNode> GetInstance(void) { return instance; }
    static std::string GetInstanceName(void) { return "cameraData"; }

    static DataLine GetCamPos(void) { return DataLine(instance->GetName(), GetCamPosOutputIndex()); }
    static DataLine GetCamForward(void) { return DataLine(instance->GetName(), GetCamForwardOutputIndex()); }
    static DataLine GetCamUp(void) { return DataLine(instance->GetName(), GetCamUpwardOutputIndex()); }
    static DataLine GetCamSide(void) { return DataLine(instance->GetName(), GetCamSidewaysOutputIndex()); }


    //This constructor should not be called; it is only here to satisfy the reflection system.


    virtual unsigned int GetNumbOutputs(void) const override { return 4; }

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override;

    virtual void WriteMyOutputs(std::string & outCode) const override;


private:

    static std::shared_ptr<DataNode> instance;
    static unsigned int GetCamPosOutputIndex(void) { return 0; }
    static unsigned int GetCamForwardOutputIndex(void) { return 1; }
    static unsigned int GetCamUpwardOutputIndex(void) { return 2; }
    static unsigned int GetCamSidewaysOutputIndex(void) { return 3; }

    CameraDataNode(void);

    ADD_NODE_REFLECTION_DATA_H(CameraDataNode)
};