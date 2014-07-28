#pragma once

#include "../../../Vertices.h"
#include "../MaterialUsageFlags.h"
#include "../UniformCollections.h"


//Everything needed to construct a geometry shader.
struct GeoShaderData
{
public:

    ShaderInOutAttributes OutputTypes;
    PrimitiveTypes InputPrimitive, OutputPrimitive;
    unsigned int MaxVertices;
    MaterialUsageFlags UsageFlags;
    UniformDictionary Params;
    std::string ShaderCode;

    //Use the default parameters for "maxVertices" and/or "shaderCode" to create an invalid instance.
    GeoShaderData(ShaderInOutAttributes outputTypes = ShaderInOutAttributes(), MaterialUsageFlags flags = MaterialUsageFlags(), unsigned int maxVerts = 4,
                  PrimitiveTypes inPrimitive = PrimitiveTypes::Points, PrimitiveTypes outPrimitive = PrimitiveTypes::TriangleStrip,
                  UniformDictionary params = UniformDictionary(), std::string shaderCode = "")
                  : OutputTypes(outputTypes), InputPrimitive(inPrimitive), OutputPrimitive(outPrimitive), MaxVertices(maxVerts),
                  UsageFlags(flags), Params(params), ShaderCode(shaderCode)
    { }

    //Returns whether this instance has a non-empty shader code string and a MaxVertices value greater than 0.
    bool IsValidData(void) const { return !ShaderCode.empty() && MaxVertices > 0; }
};