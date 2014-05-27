#pragma once

#include "../../../Vertices.h"
#include "../MaterialUsageFlags.h"
#include "../UniformCollections.h"



//Represents the geometry shader vertex output.
//Structured similarly to Vertex Attributes; a value of 0 for the output size indicates the end of the outputs.
struct GeoShaderOutput
{
public:
    std::string OutputNames[VertexAttributes::MAX_ATTRIBUTES];
    unsigned int OutputSizes[VertexAttributes::MAX_ATTRIBUTES];
    GeoShaderOutput(void) { OutputSizes[0] = 0; }
    GeoShaderOutput(std::string name, unsigned int outputSize) { OutputNames[0] = name; OutputSizes[0] = outputSize; OutputSizes[1] = 0; }
    GeoShaderOutput(std::string name1, unsigned int outputSize1, std::string name2, unsigned int outputSize2) { OutputNames[0] = name1; OutputNames[1] = name2; OutputSizes[0] = outputSize1; OutputSizes[1] = outputSize2; OutputSizes[2] = 0; }
    GeoShaderOutput(std::string name1, unsigned int outputSize1, std::string name2, unsigned int outputSize2, std::string name3, unsigned int outputSize3)
    {
        OutputNames[0] = name1;
        OutputNames[1] = name2;
        OutputNames[2] = name3;
        OutputSizes[0] = outputSize1;
        OutputSizes[1] = outputSize2;
        OutputSizes[2] = outputSize3;
        OutputSizes[3] = 0;
    }

    unsigned int GetNumbOutputs(void) const
    {
        unsigned int i;
        for (i = 0; i < VertexAttributes::MAX_ATTRIBUTES && OutputSizes[i] > 0; ++i)
            ;
        return i;
    }
};

//Everything needed to construct a geometry shader.
struct GeoShaderData
{
public:

    GeoShaderOutput OutputTypes;
    PrimitiveTypes InputPrimitive, OutputPrimitive;
    unsigned int MaxVertices;
    MaterialUsageFlags UsageFlags;
    UniformDictionary Params;
    std::string ShaderCode;

    //Use all the default parameters to create an "invalid" geometry shader.
    GeoShaderData(GeoShaderOutput outputTypes = GeoShaderOutput(), MaterialUsageFlags flags = MaterialUsageFlags(), unsigned int maxVerts = 4,
                  PrimitiveTypes inPrimitive = PrimitiveTypes::Points, PrimitiveTypes outPrimitive = PrimitiveTypes::TriangleStrip,
                  UniformDictionary params = UniformDictionary(), std::string shaderCode = "")
                  : OutputTypes(outputTypes), InputPrimitive(inPrimitive), OutputPrimitive(outPrimitive), MaxVertices(maxVerts),
                  UsageFlags(flags), Params(params), ShaderCode(shaderCode)
    { }

    //Returns whether this instance has a non-empty shader code string and a MaxVertices value greater than 0.
    bool IsValidData(void) const { return !ShaderCode.empty() && MaxVertices > 0; }
};