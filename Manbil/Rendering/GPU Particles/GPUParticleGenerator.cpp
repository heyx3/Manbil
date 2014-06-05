#include "GPUParticleGenerator.h"

#include "../Materials/Data Nodes/DataNodeIncludes.h"
#include "../../DebugAssist.h"
#include "../../Math/Array2D.h"


ShaderGenerator::GeneratedMaterial GPUParticleGenerator::GenerateGPUParticleMaterial(std::unordered_map<GPUPOutputs, DataLine> outputs, UniformDictionary & outUniforms, RenderingModes mode)
{
    VertexAttributes particleAttributes = Vertex::GetAttributeData();
    DataNodePtr vertexInputNode = DataNodePtr(new VertexInputNode(particleAttributes));

    //First check for any missing outputs.
    if (outputs.find(GPUPOutputs::GPUP_WORLDPOSITION) == outputs.end())
        return ShaderGenerator::GeneratedMaterial("No 'GPUP_WORLDPOSITION' output");
    if (outputs.find(GPUPOutputs::GPUP_SIZE) == outputs.end())
        outputs[GPUPOutputs::GPUP_SIZE] = DataLine(VectorF(1.0f, 1.0f));

    //Make sure every output is a valid size.
    for (auto val = outputs.begin(); val != outputs.end(); ++val)
        if (!IsValidGPUPOutput(val->second, val->first))
            return ShaderGenerator::GeneratedMaterial("DataLine input for " + DebugAssist::ToString(val->first));


    std::unordered_map<RenderingChannels, DataLine> channels;


    //First, convert the particle inputs into vertex shader outputs.
    channels[RenderingChannels::RC_VertexPosOutput] = DataLine(DataNodePtr(new CombineVectorNode(outputs[GPUPOutputs::GPUP_WORLDPOSITION], DataLine(VectorF(1.0f)))), 0);
    channels[RenderingChannels::RC_VERTEX_OUT_1] = DataLine(vertexInputNode, 0);


    //Next, use the geometry shader to turn points into quads.
    //This shader uses the "size", "quad rotation", and "world position" outputs.

    DataNode::SetShaderType(DataNode::Shaders::SH_GeometryShader);
    GeoShaderData geoDat(GeoShaderOutput("particleID", 2, "uvs", 2), MaterialUsageFlags(), 4, PrimitiveTypes::Points, PrimitiveTypes::TriangleStrip, UniformDictionary(), "");
    DataNode::SetGeoData(&geoDat);

    geoDat.UsageFlags.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_FORWARD);
    geoDat.UsageFlags.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_SIDEWAYS);
    geoDat.UsageFlags.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_UPWARDS);
    geoDat.UsageFlags.EnableFlag(MaterialUsageFlags::DNF_USES_VIEWPROJ_MAT);

    std::vector<std::string> functionDecls;
    std::string finalOutputs;
    std::vector<unsigned int> usedNodesParams, usedNodesFuncs, writtenNodeIDs;
 
    if (!outputs[GPUPOutputs::GPUP_SIZE].IsConstant())
    {
        try
        {
            outputs[GPUPOutputs::GPUP_SIZE].GetDataNodeValue()->SetFlags(geoDat.UsageFlags, outputs[GPUPOutputs::GPUP_SIZE].GetDataNodeLineIndex());
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial(std::string() + "Error setting flags for channel 'GPUP_SIZE': " + outputs[GPUPOutputs::GPUP_SIZE].GetDataNodeValue()->GetError());
        }
        try
        {
            outputs[GPUPOutputs::GPUP_SIZE].GetDataNodeValue()->GetParameterDeclarations(geoDat.Params, usedNodesParams);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial(std::string() + "Error getting params for channel 'GPUP_SIZE': " + outputs[GPUPOutputs::GPUP_SIZE].GetDataNodeValue()->GetError());
        }
        try
        {
            outputs[GPUPOutputs::GPUP_SIZE].GetDataNodeValue()->GetFunctionDeclarations(functionDecls, usedNodesFuncs);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial(std::string() + "Error writing functions for channel 'GPUP_SIZE': " + outputs[GPUPOutputs::GPUP_SIZE].GetDataNodeValue()->GetError());
        }
        try
        {
            outputs[GPUPOutputs::GPUP_SIZE].GetDataNodeValue()->WriteOutputs(finalOutputs, writtenNodeIDs);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial(std::string() + "Error writing outputs for channel 'GPUP_SIZE': " + outputs[GPUPOutputs::GPUP_SIZE].GetDataNodeValue()->GetError());
        }
    }
    if (!outputs[GPUPOutputs::GPUP_QUADROTATION].IsConstant())
    {
        try
        {
            outputs[GPUPOutputs::GPUP_QUADROTATION].GetDataNodeValue()->SetFlags(geoDat.UsageFlags, outputs[GPUPOutputs::GPUP_QUADROTATION].GetDataNodeLineIndex());
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial(std::string() + "Error setting flags for channel 'GPUP_QUADROTATION': " + outputs[GPUPOutputs::GPUP_QUADROTATION].GetDataNodeValue()->GetError());
        }
        try
        {
            outputs[GPUPOutputs::GPUP_QUADROTATION].GetDataNodeValue()->GetParameterDeclarations(geoDat.Params, usedNodesParams);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial(std::string() + "Error getting params for channel 'GPUP_QUADROTATION': " + outputs[GPUPOutputs::GPUP_QUADROTATION].GetDataNodeValue()->GetError());
        }
        try
        {
            outputs[GPUPOutputs::GPUP_QUADROTATION].GetDataNodeValue()->GetFunctionDeclarations(functionDecls, usedNodesFuncs);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial(std::string() + "Error writing functions for channel 'GPUP_QUADROTATION': " + outputs[GPUPOutputs::GPUP_QUADROTATION].GetDataNodeValue()->GetError());
        }
        try
        {
            outputs[GPUPOutputs::GPUP_QUADROTATION].GetDataNodeValue()->WriteOutputs(finalOutputs, writtenNodeIDs);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial(std::string() + "Error writing outputs for channel 'GPUP_QUADROTATION': " + outputs[GPUPOutputs::GPUP_QUADROTATION].GetDataNodeValue()->GetError());
        }
    }

    geoDat.ShaderCode = MaterialConstants::GetGeometryHeader(std::string("out vec2 particleID;\nout vec2 uvs;\n"), PrimitiveTypes::Points, PrimitiveTypes::TriangleStrip, 4, geoDat.UsageFlags);

    geoDat.ShaderCode += "\n//Other uniforms.\n";
    for (auto loc = geoDat.Params.FloatUniforms.begin(); loc != geoDat.Params.FloatUniforms.end(); ++loc)
        geoDat.ShaderCode += loc->second.GetDeclaration() + "\n";
    for (auto loc = geoDat.Params.FloatArrayUniforms.begin(); loc != geoDat.Params.FloatArrayUniforms.end(); ++loc)
        geoDat.ShaderCode += loc->second.GetDeclaration() + "\n";
    for (auto loc = geoDat.Params.IntUniforms.begin(); loc != geoDat.Params.IntUniforms.end(); ++loc)
        geoDat.ShaderCode += loc->second.GetDeclaration() + "\n";
    for (auto loc = geoDat.Params.IntArrayUniforms.begin(); loc != geoDat.Params.IntArrayUniforms.end(); ++loc)
        geoDat.ShaderCode += loc->second.GetDeclaration() + "\n";
    for (auto loc = geoDat.Params.MatrixUniforms.begin(); loc != geoDat.Params.MatrixUniforms.end(); ++loc)
        geoDat.ShaderCode += loc->second.GetDeclaration() + "\n";
    for (auto loc = geoDat.Params.TextureUniforms.begin(); loc != geoDat.Params.TextureUniforms.end(); ++loc)
        geoDat.ShaderCode += loc->second.GetDeclaration() + "\n";
    
    geoDat.ShaderCode += "\n\n//Helper functions.\n";
    for (int i = 0; i < functionDecls.size(); ++i)
        geoDat.ShaderCode += functionDecls[i];
    
    //Define quaternion rotation.
    if (!outputs[GPUPOutputs::GPUP_QUADROTATION].IsConstant(0.0f))
    {
        geoDat.ShaderCode += "                                        \n\
vec3 rotateByQuaternion_geoShader(vec3 pos, vec4 rot)     \n\
{                                                         \n\
    return pos + (2.0 * cross(cross(pos, rot.xyz) + (rot.w * pos),\n\
                              rot.xyz));\n\
}";
    }
    //Define some other stuff for the shader.
    std::string vpTransf = "(" + MaterialConstants::ViewProjMatName + " * vec4(",
                size = outputs[GPUPOutputs::GPUP_SIZE].GetValue();
    geoDat.ShaderCode += std::string() +
"                                                                               \n\
                                                                                \n\
void main()                                                                     \n\
{                                                                               \n\
    //Data node computations.                                                   \n\
" + finalOutputs + "                                                            \n\
                                                                                \n\
    //Position/orientation calculations.                                        \n\
    vec3 pos = " + outputs[GPUPOutputs::GPUP_WORLDPOSITION].GetValue() + ";     \n\
    vec3 up = " + MaterialConstants::CameraUpName + ";                          \n\
    vec3 side = " + MaterialConstants::CameraSideName + ";                      \n\
    up = cross(" + MaterialConstants::CameraForwardName + ", side);             \n\
                                                                                \n\
    //Rotation calculations.                                                    \n\
    float halfRot = 0.5f * " +
                    outputs[GPUPOutputs::GPUP_QUADROTATION].GetValue() + ";     \n\
    vec4 rotQuat = vec4(" + MaterialConstants::CameraForwardName + " * sin(halfRot),\n\
                        cos(halfRot));                                          \n\
                                                                                \n\
    vec3 cornerPos = " + size + " * (up + side);                                \n\
    cornerPos = rotateByQuaternion_geoShader(cornerPos, rotQuat);               \n\
    gl_Position = " + vpTransf + "pos + cornerPos, 1.0));                       \n\
    uvs = vec2(1.0, 1.0);                                                       \n\
    particleID = " + MaterialConstants::VertexOutNameBase + "1;                 \n\
    EmitVertex();                                                               \n\
                                                                                \n\
    cornerPos = " + size + " * (-up + side);                                    \n\
    cornerPos = rotateByQuaternion_geoShader(cornerPos, rotQuat);               \n\
    gl_Position = " + vpTransf + "pos + cornerPos, 1.0));                       \n\
    uvs = vec2(1.0, 0.0);                                                       \n\
    particleID = " + MaterialConstants::VertexOutNameBase + "1;                 \n\
    EmitVertex();                                                               \n\
                                                                                \n\
    cornerPos = " + size + " * (up - side);                                     \n\
    cornerPos = rotateByQuaternion_geoShader(cornerPos, rotQuat);               \n\
    gl_Position = " + vpTransf + "pos + cornerPos, 1.0));                       \n\
    uvs = vec2(0.0, 1.0);                                                       \n\
    particleID = " + MaterialConstants::VertexOutNameBase + "1;                 \n\
    EmitVertex();                                                               \n\
                                                                                \n\
    cornerPos = " + size + " * -(up + side);                                    \n\
    cornerPos = rotateByQuaternion_geoShader(cornerPos, rotQuat);               \n\
    gl_Position = " + vpTransf + "pos + cornerPos, 1.0));                       \n\
    uvs = vec2(0.0, 0.0);                                                       \n\
    particleID = " + MaterialConstants::VertexOutNameBase + "1;                 \n\
    EmitVertex();                                                               \n\
}";


    //Fragment shader uses the color output.
    channels[RenderingChannels::RC_Color] = DataLine(DataNodePtr(new SwizzleNode(outputs[GPUPOutputs::GPUP_COLOR], SwizzleNode::C_X, SwizzleNode::C_Y, SwizzleNode::C_Z)), 0);
    channels[RenderingChannels::RC_Opacity] = DataLine(DataNodePtr(new VectorComponentsNode(outputs[GPUPOutputs::GPUP_COLOR])), 3);


    return ShaderGenerator::GenerateMaterial(channels, outUniforms, particleAttributes, mode, false, LightSettings(false), geoDat);
}

RenderObjHandle GPUParticleGenerator::GenerateGPUPParticles(GPUParticleGenerator::NumberOfParticles numb)
{
    //Get the total number of particles and the number of particles in each row/column (in terms of particle ID).
    unsigned int n = GetNumbParticles(numb);
    unsigned int length = GetParticleDataLength(numb);

    //Generate the particle data.
    Array2D<Vertex> particles(length, length);
    const float increment = 1.0f / length;
    for (unsigned int y = 0; y < length; ++y)
    {
        float yID = increment * y;

        for (unsigned int x = 0; x < length; ++x)
        {
            float xID = increment * x;

            particles[Vector2i(x, y)].ParticleID = Vector2f(xID, yID);
        }
    }

    //Create the vertex buffer.
    RenderObjHandle vbo;
    RenderDataHandler::CreateVertexBuffer(vbo, particles.GetArray(), n, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
    return vbo;
}