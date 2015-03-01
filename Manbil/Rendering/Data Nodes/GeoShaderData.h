#pragma once

#include "../Basic Rendering/RenderIOAttributes.h"
#include "../Basic Rendering/MaterialUsageFlags.h"
#include "../Basic Rendering/UniformCollections.h"
#include "../Basic Rendering/MaterialUsageFlags.h"


//Everything needed to construct a geometry shader.
struct GeoShaderData
{
public:

    RenderIOAttributes OutputTypes;
    PrimitiveTypes InputPrimitive, OutputPrimitive;
    unsigned int MaxVertices;
    MaterialUsageFlags UsageFlags;
    UniformDictionary Params;
    std::string ShaderCode;

    //Use the default parameters for "maxVertices" and/or "shaderCode" to create an invalid instance.
    GeoShaderData(RenderIOAttributes outputTypes = RenderIOAttributes(),
                  MaterialUsageFlags flags = MaterialUsageFlags(), unsigned int maxVerts = 4,
                  PrimitiveTypes inPrimitive = PT_POINTS, PrimitiveTypes outPrimitive = PT_TRIANGLE_STRIP,
                  UniformDictionary params = UniformDictionary(), std::string shaderCode = "")
        : OutputTypes(outputTypes), InputPrimitive(inPrimitive), OutputPrimitive(outPrimitive),
          MaxVertices(maxVerts), UsageFlags(flags), Params(params), ShaderCode(shaderCode) { }

    //Returns whether this instance has a non-empty shader code string
    //    and a MaxVertices value greater than 0.
    bool IsValidData(void) const { return !ShaderCode.empty() && MaxVertices > 0; }
};