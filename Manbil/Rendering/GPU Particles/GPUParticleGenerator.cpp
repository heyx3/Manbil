#include "GPUParticleGenerator.h"

#include "../Materials/Data Nodes/DataNodeIncludes.h"
#include "../../DebugAssist.h"
#include "../../Math/Array2D.h"


ShaderGenerator::GeneratedMaterial GPUParticleGenerator::GenerateGPUParticleMaterial(std::unordered_map<GPUPOutputs, DataLine> outputs, UniformDictionary & outUniforms, RenderingModes mode)
{
    VertexAttributes particleAttributes = ParticleVertex::GetAttributeData();
    DataNodePtr vertexInputNode = DataNodePtr(new VertexInputNode(particleAttributes));

    //First check for any missing outputs.
    if (outputs.find(GPUPOutputs::GPUP_WORLDPOSITION) == outputs.end())
        return ShaderGenerator::GeneratedMaterial("No 'GPUP_WORLDPOSITION' output");
    if (outputs.find(GPUPOutputs::GPUP_SIZE) == outputs.end())
        outputs[GPUPOutputs::GPUP_SIZE] = DataLine(VectorF(1.0f, 1.0f));
    if (outputs.find(GPUPOutputs::GPUP_QUADROTATION) == outputs.end())
        outputs[GPUPOutputs::GPUP_QUADROTATION] = DataLine(VectorF(0.0f));
    if (outputs.find(GPUPOutputs::GPUP_COLOR) == outputs.end())
        outputs[GPUPOutputs::GPUP_COLOR] = DataLine(VectorF(1.0f, 1.0f, 1.0f, 1.0f));

    //Make sure every output is a valid size.
    for (auto val = outputs.begin(); val != outputs.end(); ++val)
        if (!IsValidGPUPOutput(val->second, val->first))
            return ShaderGenerator::GeneratedMaterial("DataLine input for " + DebugAssist::ToString(val->first));


    std::unordered_map<RenderingChannels, DataLine> channels;


    //First, convert the particle inputs into vertex shader outputs.
    channels[RenderingChannels::RC_VertexPosOutput] = DataLine(DataNodePtr(new CombineVectorNode(outputs[GPUPOutputs::GPUP_WORLDPOSITION], DataLine(VectorF(1.0f)))), 0);
    channels[RenderingChannels::RC_VERTEX_OUT_0] = DataLine(vertexInputNode, 0);
    channels[RenderingChannels::RC_VERTEX_OUT_1] = DataLine(vertexInputNode, 1);
    channels[RenderingChannels::RC_VERTEX_OUT_2] = DataLine(vertexInputNode, 2);


    //Next, use the geometry shader to turn points into quads.
    //This shader uses the "size", "quad rotation", and "world position" outputs.

    DataNode::SetShaderType(DataNode::Shaders::SH_GeometryShader);
    GeoShaderData geoDat(GeoShaderOutput("particleID", 2, "randSeeds1", 4, "randSeeds2", 2, "uvs", 2),
                         MaterialUsageFlags(), 4, PrimitiveTypes::Points, PrimitiveTypes::TriangleStrip, UniformDictionary(), " ");
    DataNode::SetGeoData(&geoDat);

    geoDat.UsageFlags.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_FORWARD);
    geoDat.UsageFlags.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_SIDEWAYS);
    geoDat.UsageFlags.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_UPWARDS);
    geoDat.UsageFlags.EnableFlag(MaterialUsageFlags::DNF_USES_VIEWPROJ_MAT);

    std::vector<std::string> functionDecls;
    std::string finalOutputs;
    std::vector<unsigned int> usedNodesParams, usedNodesFuncs, writtenNodeIDs;
 
#pragma warning(disable: 4101)
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
    if (!outputs[GPUPOutputs::GPUP_WORLDPOSITION].IsConstant())
    {
        try
        {
            outputs[GPUPOutputs::GPUP_WORLDPOSITION].GetDataNodeValue()->SetFlags(geoDat.UsageFlags, outputs[GPUPOutputs::GPUP_WORLDPOSITION].GetDataNodeLineIndex());
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial(std::string() + "Error setting flags for channel 'GPUP_WORLDPOSITION': " + outputs[GPUPOutputs::GPUP_WORLDPOSITION].GetDataNodeValue()->GetError());
        }
        try
        {
            outputs[GPUPOutputs::GPUP_WORLDPOSITION].GetDataNodeValue()->GetParameterDeclarations(geoDat.Params, usedNodesParams);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial(std::string() + "Error getting params for channel 'GPUP_WORLDPOSITION': " + outputs[GPUPOutputs::GPUP_WORLDPOSITION].GetDataNodeValue()->GetError());
        }
        try
        {
            outputs[GPUPOutputs::GPUP_WORLDPOSITION].GetDataNodeValue()->GetFunctionDeclarations(functionDecls, usedNodesFuncs);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial(std::string() + "Error writing functions for channel 'GPUP_WORLDPOSITION': " + outputs[GPUPOutputs::GPUP_WORLDPOSITION].GetDataNodeValue()->GetError());
        }
        try
        {
            outputs[GPUPOutputs::GPUP_WORLDPOSITION].GetDataNodeValue()->WriteOutputs(finalOutputs, writtenNodeIDs);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial(std::string() + "Error writing outputs for channel 'GPUP_WORLDPOSITION': " + outputs[GPUPOutputs::GPUP_WORLDPOSITION].GetDataNodeValue()->GetError());
        }
    }
#pragma warning(default: 4101)
    
    geoDat.ShaderCode += "\n\n//Helper functions.\n";
    for (unsigned int i = 0; i < functionDecls.size(); ++i)
        geoDat.ShaderCode += functionDecls[i];
    //PRIORITY: Shouldn't GLSL uniforms be declared here?
    
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
                sizeX = outputs[GPUPOutputs::GPUP_SIZE].GetValue() + ".x",
                sizeY = outputs[GPUPOutputs::GPUP_SIZE].GetValue() + ".y",
                rotByQuat = "";
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
";
    if (!outputs[GPUPOutputs::GPUP_QUADROTATION].IsConstant(0.0f))
    {
        geoDat.ShaderCode += std::string() +
"    //Rotation calculations.                                                   \n\
    float halfRot = 0.5f * " +
                    outputs[GPUPOutputs::GPUP_QUADROTATION].GetValue() + ";     \n\
    vec4 rotQuat = vec4(" + MaterialConstants::CameraForwardName + " * sin(halfRot),\n\
                        cos(halfRot));                                          \n\
";
        rotByQuat = "    cornerPos = rotateByQuaternion_geoShader(cornerPos, rotQuat);\n";
    }
    
    geoDat.ShaderCode += std::string() +
"                                                                               \n\
    vec3 cornerPos = (up * " + sizeY + ") + (side * " + sizeX + ");             \n\
" + rotByQuat + "                                                               \n\
    gl_Position = " + vpTransf + "pos + cornerPos, 1.0));                       \n\
    uvs = vec2(1.0, 1.0);                                                       \n\
    particleID = " + MaterialConstants::VertexOutNameBase + "0[0];              \n\
    randSeeds1 = " + MaterialConstants::VertexOutNameBase + "1[0];              \n\
    randSeeds2 = " + MaterialConstants::VertexOutNameBase + "2[0];              \n\
    EmitVertex();                                                               \n\
                                                                                \n\
    cornerPos = (-up * " + sizeY + ") + (side * " + sizeX + ");                 \n\
" + rotByQuat + "                                                               \n\
    gl_Position = " + vpTransf + "pos + cornerPos, 1.0));                       \n\
    uvs = vec2(1.0, 0.0);                                                       \n\
    particleID = " + MaterialConstants::VertexOutNameBase + "0[0];              \n\
    randSeeds1 = " + MaterialConstants::VertexOutNameBase + "1[0];              \n\
    randSeeds2 = " + MaterialConstants::VertexOutNameBase + "2[0];              \n\
    EmitVertex();                                                               \n\
                                                                                \n\
    cornerPos = (up * " + sizeY + ") - (side * " + sizeX + ");                  \n\
" + rotByQuat + "                                                               \n\
    gl_Position = " + vpTransf + "pos + cornerPos, 1.0));                       \n\
    uvs = vec2(0.0, 1.0);                                                       \n\
    particleID = " + MaterialConstants::VertexOutNameBase + "0[0];              \n\
    randSeeds1 = " + MaterialConstants::VertexOutNameBase + "1[0];              \n\
    randSeeds2 = " + MaterialConstants::VertexOutNameBase + "2[0];              \n\
    EmitVertex();                                                               \n\
                                                                                \n\
    cornerPos = -((up * " + sizeY + ") + (side * " + sizeX + "));               \n\
" + rotByQuat + "                                                               \n\
    gl_Position = " + vpTransf + "pos + cornerPos, 1.0));                       \n\
    uvs = vec2(0.0, 0.0);                                                       \n\
    particleID = " + MaterialConstants::VertexOutNameBase + "0[0];              \n\
    randSeeds1 = " + MaterialConstants::VertexOutNameBase + "1[0];              \n\
    randSeeds2 = " + MaterialConstants::VertexOutNameBase + "2[0];              \n\
    EmitVertex();                                                               \n\
}";


    //Fragment shader uses the color output.
    channels[RenderingChannels::RC_Color] = DataLine(DataNodePtr(new SwizzleNode(outputs[GPUPOutputs::GPUP_COLOR], SwizzleNode::C_X, SwizzleNode::C_Y, SwizzleNode::C_Z)), 0);
    channels[RenderingChannels::RC_Opacity] = DataLine(DataNodePtr(new VectorComponentsNode(outputs[GPUPOutputs::GPUP_COLOR])), 3);


    return ShaderGenerator::GenerateMaterial(channels, outUniforms, particleAttributes, mode, false, LightSettings(false), geoDat);
}

RenderObjHandle GPUParticleGenerator::GenerateGPUPParticles(GPUParticleGenerator::NumberOfParticles numb, int randSeed)
{
    //Get the total number of particles and the number of particles in each row/column (in terms of particle ID).
    unsigned int n = GetNumbParticles(numb);
    unsigned int length = GetParticleDataLength(numb);

    //Generate the particle data.
    Array2D<ParticleVertex> particles(length, length);
    const float increment = 1.0f / (float)length;
    for (Vector2u loc; loc.y < length; ++loc.y)
    {
        float yID = increment * loc.y;

        for (loc.x = 0; loc.x < length; ++loc.x)
        {
            float xID = increment * loc.x;

            FastRand fr(Vector3u(loc.x, loc.y, randSeed).GetHashCode());
            fr.GetRandInt();
            fr.GetRandInt();

            particles[loc] = ParticleVertex(Vector2f(xID, yID),
                                            Vector4f(fr.GetZeroToOne(), fr.GetZeroToOne(), fr.GetZeroToOne(), fr.GetZeroToOne()),
                                            Vector2f(fr.GetZeroToOne(), fr.GetZeroToOne()));
        }
    }

    //Create the vertex buffer.
    RenderObjHandle vbo;
    RenderDataHandler::CreateVertexBuffer(vbo, particles.GetArray(), n, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
    return vbo;
}