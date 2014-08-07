#pragma once

#include "../DataNode.h"


//Outputs a combination of an input's components.
class SwizzleNode : public DataNode
{
public:

    enum Components { C_X, C_Y, C_Z, C_W, };

    unsigned int GetNumbComponents(void) const { return nComps; }
    Components GetComponent(unsigned int index) const { return comps[index]; }


#pragma warning(disable: 4100)
    virtual unsigned int GetOutputSize(unsigned int index) const override { return nComps; }
#pragma warning(default: 4100)
    virtual std::string GetOutputName(unsigned int index) const override;


    SwizzleNode(const DataLine & in, Components newX, std::string name = "");
    SwizzleNode(const DataLine & in, Components newX, Components newY, std::string name = "");
    SwizzleNode(const DataLine & in, Components newX, Components newY, Components newZ, std::string name = "");
    SwizzleNode(const DataLine & in, Components newX, Components newY, Components newZ, Components newW, std::string name = "");

protected:

    virtual void WriteMyOutputs(std::string & outStr) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;

    virtual bool WriteExtraData(DataWriter * writer, std::string & outError) const override;
    virtual bool ReadExtraData(DataReader * reader, std::string & outError) override;


private:

    Components comps[4];
    unsigned int nComps;

    MAKE_NODE_READABLE_H(SwizzleNode)
};