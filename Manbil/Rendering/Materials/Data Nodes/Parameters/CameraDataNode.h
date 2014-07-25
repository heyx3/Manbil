#pragma once

#include "../DataNode.h"


//Outputs different camera position, as well as forward/upwards/sideways vectors.
class CameraDataNode : public DataNode
{
public:

    static DataNodePtr GetInstance(void) { return instance; }
    static DataLine GetCamPos(void) { return DataLine(GetInstance(), GetCamPosOutputIndex()); }
    static DataLine GetCamForward(void) { return DataLine(GetInstance(), GetCamForwardOutputIndex()); }
    static DataLine GetCamUp(void) { return DataLine(GetInstance(), GetCamUpwardOutputIndex()); }
    static DataLine GetCamSide(void) { return DataLine(GetInstance(), GetCamSidewaysOutputIndex()); }


    virtual std::string GetName(void) const override { return "cameraDataNode"; }
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
    static unsigned int GetCamPosOutputIndex(void) { return 0; }
    static unsigned int GetCamForwardOutputIndex(void) { return 1; }
    static unsigned int GetCamUpwardOutputIndex(void) { return 2; }
    static unsigned int GetCamSidewaysOutputIndex(void) { return 3; }

    CameraDataNode(void) : DataNode(std::vector<DataLine>(), MakeVector(3, 3, 3, 3)) { }
};