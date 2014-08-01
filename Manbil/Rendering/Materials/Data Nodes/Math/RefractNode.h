#pragma once

#include "../DataNode.h"


//Outputs a vector refracted around another vector.
class RefractNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "refract"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    RefractNode(const DataLine & toRefract, const DataLine & refractNormal, const DataLine & indexOfRefraction, std::string name = "");


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;
};