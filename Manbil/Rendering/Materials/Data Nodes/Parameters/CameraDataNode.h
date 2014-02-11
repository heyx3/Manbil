#pragma once

#include "../DataNode.h"


//Outputs different camera position, as well as forward/upwards/sideways vectors.
class CameraDataNode : public DataNode
{
public:

    static unsigned int GetCamPosOutput(void) { return 0; }
    static unsigned int GetCamForwardOutput(void) { return 1; }
    static unsigned int GetCamUpwardOutput(void) { return 2; }
    static unsigned int GetCamSidewaysOutput(void) { return 3; }

    virtual std::string GetName(void) const override { return "cameraDataNode"; }

    CameraDataNode(void) : DataNode(std::vector<DataLine>(), MakeVector(3, 3, 3, 3)) { }

    virtual std::string GetOutputName(unsigned int index, Shaders shaderType) const override;

protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderTypes) const override
    {
        //No output writing needed.
    }
};