#include "GPUParticleGenerator.h"

#include "../Materials/Data Nodes/DataNodeIncludes.h"
#include "../../DebugAssist.h"
#include "../../Math/Lower Math/Array2D.h"


ShaderGenerator::GeneratedMaterial GPUParticleGenerator::GenerateGPUParticleMaterial(std::unordered_map<GPUPOutputs, DataLine> outputs, UniformDictionary & outUniforms, RenderingModes mode)
{
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

    //Prepare material data to be created.
    DataNode::ClearMaterialData();
    DataNode::VertexIns = ParticleVertex::GetAttributeData();
    std::vector<ShaderOutput> & vertexOuts = DataNode::MaterialOuts.VertexOutputs,
                              & fragmentOuts = DataNode::MaterialOuts.FragmentOutputs;


    //First, convert the particle inputs into vertex shader outputs.
    DataNode::CurrentShader = ShaderHandler::SH_Vertex_Shader;
    DataNode::Ptr worldPos(new CombineVectorNode(outputs[GPUPOutputs::GPUP_WORLDPOSITION], DataLine(VectorF(1.0f)), "worldPos"));
    DataNode::MaterialOuts.VertexPosOutput = std::string("worldPos");
    vertexOuts.insert(vertexOuts.end(), ShaderOutput("vOut_particleID", DataLine(VertexInputNode::GetInstanceName(), 0)));
    vertexOuts.insert(vertexOuts.end(), ShaderOutput("vOut_randSeeds1", DataLine(VertexInputNode::GetInstanceName(), 1)));
    vertexOuts.insert(vertexOuts.end(), ShaderOutput("vOut_randSeeds2", DataLine(VertexInputNode::GetInstanceName(), 2)));


    //Next, use the geometry shader to turn points into quads.
    //This shader uses the "size", "quad rotation", and "world position" outputs.

    DataNode::CurrentShader = ShaderHandler::SH_GeometryShader;
    DataNode::GeometryShader = GeoShaderData(ShaderInOutAttributes(2, 4, 2, 2, false, false, false, false, "particleID", "randSeeds1", "randSeeds2", "uvs"),
                                             MaterialUsageFlags(), 4, PrimitiveTypes::Points, PrimitiveTypes::TriangleStrip, UniformDictionary(), " ");
    DataNode::GeometryShader.UsageFlags.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_FORWARD);
    DataNode::GeometryShader.UsageFlags.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_SIDEWAYS);
    DataNode::GeometryShader.UsageFlags.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_UPWARDS);
    DataNode::GeometryShader.UsageFlags.EnableFlag(MaterialUsageFlags::DNF_USES_VIEWPROJ_MAT);

    std::vector<std::string> functionDecls;
    std::string finalOutputs;
    std::vector<const DataNode*> usedNodesParams, usedNodesFuncs, writtenNodeIDs;
 
    //Generate info/code for the geometry shader inputs.
#pragma warning(disable: 4101)
    for (unsigned int i = 0; i < 3; ++i)
    {
        std::string typeName;
        GPUPOutputs typeT;
        switch (i)
        {
            case 0:
                typeT = GPUPOutputs::GPUP_SIZE;
                typeName = "GPUP_SIZE";
                break;
            case 1:
                typeName = "GPUP_QUADROTATION";
                break;
            case 2:
                typeName = "GPUP_WORLDPOSITION";
                break;

            default: assert(false);
        }

        if (outputs[typeT].IsConstant()) continue;
        DataNode* pNode = outputs[typeT].GetNode();
        std::string action;
        try
        {
            action = "Invalid input chain";
            pNode->AssertAllInputsValid();
            action = "Error setting flags";
            pNode->SetFlags(DataNode::GeometryShader.UsageFlags, outputs[typeT].GetSize());
            action = "Error getting params";
            pNode->GetParameterDeclarations(DataNode::GeometryShader.Params, usedNodesParams);
            action = "Error writing functions";
            pNode->GetFunctionDeclarations(functionDecls, usedNodesFuncs);
            action = "Error writing outputs";
            pNode->WriteOutputs(finalOutputs, writtenNodeIDs);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial(action + " for channel '" + typeName + "': " + pNode->GetError());
        }
    }
#pragma warning(default: 4101)
    
    DataNode::GeometryShader.ShaderCode += "\n\n//Helper functions.\n";
    for (unsigned int i = 0; i < functionDecls.size(); ++i)
        DataNode::GeometryShader.ShaderCode += functionDecls[i];
    
    //Define quaternion rotation.
    if (!outputs[GPUPOutputs::GPUP_QUADROTATION].IsConstant(0.0f))
    {
        DataNode::GeometryShader.ShaderCode += "                                        \n\
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
    DataNode::GeometryShader.ShaderCode += std::string() +
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
        DataNode::GeometryShader.ShaderCode += std::string() +
"    //Rotation calculations.                                                   \n\
    float halfRot = 0.5f * " +
                    outputs[GPUPOutputs::GPUP_QUADROTATION].GetValue() + ";     \n\
    vec4 rotQuat = vec4(" + MaterialConstants::CameraForwardName + " * sin(halfRot),\n\
                        cos(halfRot));                                          \n\
";
        rotByQuat = "    cornerPos = rotateByQuaternion_geoShader(cornerPos, rotQuat);\n";
    }
    
    DataNode::GeometryShader.ShaderCode += std::string() +
"                                                                               \n\
    vec3 cornerPos = (up * " + sizeY + ") + (side * " + sizeX + ");             \n\
" + rotByQuat + "                                                               \n\
    gl_Position = " + vpTransf + "pos + cornerPos, 1.0));                       \n\
    uvs = vec2(1.0, 1.0);                                                       \n\
    particleID = " + DataNode::MaterialOuts.VertexOutputs[0].Name + "[0];       \n\
    randSeeds1 = " + DataNode::MaterialOuts.VertexOutputs[1].Name + "[0];       \n\
    randSeeds2 = " + DataNode::MaterialOuts.VertexOutputs[2].Name + "[0];       \n\
    EmitVertex();                                                               \n\
                                                                                \n\
    cornerPos = (-up * " + sizeY + ") + (side * " + sizeX + ");                 \n\
" + rotByQuat + "                                                               \n\
    gl_Position = " + vpTransf + "pos + cornerPos, 1.0));                       \n\
    uvs = vec2(1.0, 0.0);                                                       \n\
    particleID = " + DataNode::MaterialOuts.VertexOutputs[0].Name + "[0];       \n\
    randSeeds1 = " + DataNode::MaterialOuts.VertexOutputs[1].Name + "[0];       \n\
    randSeeds2 = " + DataNode::MaterialOuts.VertexOutputs[2].Name + "[0];       \n\
    EmitVertex();                                                               \n\
                                                                                \n\
    cornerPos = (up * " + sizeY + ") - (side * " + sizeX + ");                  \n\
" + rotByQuat + "                                                               \n\
    gl_Position = " + vpTransf + "pos + cornerPos, 1.0));                       \n\
    uvs = vec2(0.0, 1.0);                                                       \n\
    particleID = " + DataNode::MaterialOuts.VertexOutputs[0].Name + "[0];       \n\
    randSeeds1 = " + DataNode::MaterialOuts.VertexOutputs[1].Name + "[0];       \n\
    randSeeds2 = " + DataNode::MaterialOuts.VertexOutputs[2].Name + "[0];       \n\
    EmitVertex();                                                               \n\
                                                                                \n\
    cornerPos = -((up * " + sizeY + ") + (side * " + sizeX + "));               \n\
" + rotByQuat + "                                                               \n\
    gl_Position = " + vpTransf + "pos + cornerPos, 1.0));                       \n\
    uvs = vec2(0.0, 0.0);                                                       \n\
    particleID = " + DataNode::MaterialOuts.VertexOutputs[0].Name + "[0];       \n\
    randSeeds1 = " + DataNode::MaterialOuts.VertexOutputs[1].Name + "[0];       \n\
    randSeeds2 = " + DataNode::MaterialOuts.VertexOutputs[2].Name + "[0];       \n\
    EmitVertex();                                                               \n\
}";


    //Fragment shader uses the color output.
    fragmentOuts.insert(fragmentOuts.end(), ShaderOutput("out_particleColor", outputs[GPUPOutputs::GPUP_COLOR]));


    return ShaderGenerator::GenerateMaterial(outUniforms, mode);
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