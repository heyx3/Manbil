#pragma once

#include "../Basic Rendering/RenderIOAttributes.h"
#include "../Basic Rendering/MaterialUsageFlags.h"
#include "../Basic Rendering/UniformCollections.h"
#include "../Basic Rendering/MaterialUsageFlags.h"

#include "../../IO/DataSerialization.h"


//Everything needed to construct a geometry shader.
//Sits between the vertex and fragment shaders and modifies the primitive shapes in the vertex buffer.
struct GeoShaderData : public ISerializable
{
public:

    //The attributes output from each vertex into the fragment shader.
    RenderIOAttributes OutputTypes;

    //The type of primitive shapes going into this shader from the vertex shader
    //    and out from this shader into the fragment shader.
    PrimitiveTypes InputPrimitive, OutputPrimitive;

    //The maximum number of vertices that this shader may output.
    unsigned int MaxVertices;

    //The built-in uniform parameters this shader uses.
    MaterialUsageFlags UsageFlags;

    //The custom parameters this shader uses.
    UniformList Params;

    //The body of the shader code, including the entry point "main()" function.
    std::string ShaderCode;


    //Use the default parameters for "maxVertices" and/or "shaderCode" to create an invalid instance.
    GeoShaderData(RenderIOAttributes outputTypes = RenderIOAttributes(),
                  MaterialUsageFlags flags = MaterialUsageFlags(),
                  unsigned int maxVerts = 4,
                  PrimitiveTypes inPrimitive = PT_POINTS,
                  PrimitiveTypes outPrimitive = PT_TRIANGLE_STRIP,
                  UniformList params = UniformList(),
                  std::string shaderCode = "")
        : OutputTypes(outputTypes), InputPrimitive(inPrimitive), OutputPrimitive(outPrimitive),
          MaxVertices(maxVerts), UsageFlags(flags), Params(params), ShaderCode(shaderCode) { }


    //Returns whether this instance has a non-empty shader code string
    //    and a MaxVertices value greater than 0.
    bool IsValidData(void) const { return !ShaderCode.empty() && MaxVertices > 0; }

    virtual void WriteData(DataWriter* writer) const override;
    virtual void ReadData(DataReader* reader) override;
};