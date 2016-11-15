#pragma once

#include "../../IO/DataSerialization.h"
#include "DataLine.h"
#include "../Basic Rendering/RenderIOAttributes.h"
#include "MaterialOutputs.h"
#include "GeoShaderData.h"


class DataNode;


//Represents a material as a set of DataNodes and DataLines.
struct SerializedMaterial : public ISerializable
{
public:

    //The vertex inputs.
    RenderIOAttributes VertexInputs;
    //The material expression graph.
    MaterialOutputs MaterialOuts;

    //The optional geometry shader.
    GeoShaderData GeoShader;


    SerializedMaterial(RenderIOAttributes vertexIns = RenderIOAttributes(),
                       MaterialOutputs materialOuts = MaterialOutputs(),
                       GeoShaderData geoShader = GeoShaderData())
        : VertexInputs(vertexIns), MaterialOuts(materialOuts) { }

    
    //Gets the nodes created after calling "ReadData".
    const std::vector<std::shared_ptr<DataNode>>& GetNodesRead(void) const { return nodeStorage; }
    //Gets the nodes created after calling "ReadData".
    std::vector<std::shared_ptr<DataNode>>& GetNodesRead(void) { return nodeStorage; }

    virtual void WriteData(DataWriter* writer) const override;
    virtual void ReadData(DataReader* reader) override;

private:

    std::vector<std::shared_ptr<DataNode>> nodeStorage;
};