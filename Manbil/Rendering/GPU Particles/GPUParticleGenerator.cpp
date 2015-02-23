#include "GPUParticleGenerator.h"

#include "../Materials/Data Nodes/DataNodes.hpp"
#include "../../DebugAssist.h"
#include "../../Math/Lower Math/Array2D.h"


unsigned int GPUParticleGenerator::GetNumbParticles(NumberOfParticles number)
{
    switch (number)
    {
        case NumberOfParticles::NOP_1: return 1;
        case NumberOfParticles::NOP_4: return 4;
        case NumberOfParticles::NOP_16: return 16;
        case NumberOfParticles::NOP_64: return 64;
        case NumberOfParticles::NOP_256: return 256;
        case NumberOfParticles::NOP_1024: return 1024;
        case NumberOfParticles::NOP_4096: return 4096;
        case NumberOfParticles::NOP_16384: return 16384;
        case NumberOfParticles::NOP_65536: return 65536;
        case NumberOfParticles::NOP_262144: return 262144;
        case NumberOfParticles::NOP_1048576: return 1048576;

        default: assert(false); return 0;
    }
}
unsigned int GPUParticleGenerator::GetParticleDataLength(NumberOfParticles number)
{
    switch (number)
    {
        case NumberOfParticles::NOP_1: return 1;
        case NumberOfParticles::NOP_4: return 2;
        case NumberOfParticles::NOP_16: return 4;
        case NumberOfParticles::NOP_64: return 8;
        case NumberOfParticles::NOP_256: return 16;
        case NumberOfParticles::NOP_1024: return 32;
        case NumberOfParticles::NOP_4096: return 64;
        case NumberOfParticles::NOP_16384: return 128;
        case NumberOfParticles::NOP_65536: return 256;
        case NumberOfParticles::NOP_262144: return 512;
        case NumberOfParticles::NOP_1048576: return 1024;

        default: assert(false); return 0;
    }
}


typedef ShaderGenerator::GeneratedMaterial GenM;
GenM GPUParticleGenerator::GenerateMaterial(GPUParticleGenerator::GPUPOuts outputs,
                                            UniformDictionary& outUniforms,
                                            BlendMode mode)
{
    //First check for any missing outputs.

    if (outputs.find(GPUPOutputs::GPUP_WORLDPOSITION) == outputs.end())
    {
        return ShaderGenerator::GeneratedMaterial("No 'GPUP_WORLDPOSITION' output");
    }
    if (outputs.find(GPUPOutputs::GPUP_SIZE) == outputs.end())
    {
        outputs[GPUPOutputs::GPUP_SIZE] = DataLine(Vector2f(1.0f, 1.0f));
    }
    if (outputs.find(GPUPOutputs::GPUP_QUADROTATION) == outputs.end())
    {
        outputs[GPUPOutputs::GPUP_QUADROTATION] = DataLine(0.0f);
    }
    if (outputs.find(GPUPOutputs::GPUP_COLOR) == outputs.end())
    {
        outputs[GPUPOutputs::GPUP_COLOR] = DataLine(Vector4f(1.0f, 0.0f, 1.0f, 1.0f));
    }

    //Make sure every output is a valid size.
    for (auto val = outputs.begin(); val != outputs.end(); ++val)
    {
        if (!IsValidGPUPOutput(val->second, val->first))
        {
            return ShaderGenerator::GeneratedMaterial("DataLine input for " +
                                                      DebugAssist::ToString(val->first) +
                                                      " isn't valid!");
        }
    }

    //Prepare material data to be created.
    DataNode::ClearMaterialData();
    DataNode::VertexIns = ParticleVertex::GetVertexInputData();
    std::vector<ShaderOutput> &vertexOuts = DataNode::MaterialOuts.VertexOutputs,
                              &fragmentOuts = DataNode::MaterialOuts.FragmentOutputs;


    //First, convert the particle inputs into vertex shader outputs.
    DataNode::CurrentShader = SH_VERTEX;
    DataNode::Ptr worldPos(new CombineVectorNode(outputs[GPUPOutputs::GPUP_WORLDPOSITION],
                                                 DataLine(VectorF(1.0f)), "worldPos"));
    DataNode::MaterialOuts.VertexPosOutput = std::string("worldPos");
    vertexOuts.insert(vertexOuts.end(),
                      ShaderOutput("vOut_ParticleID",
                                   DataLine(VertexInputNode::GetInstanceName(), 0)));
    vertexOuts.insert(vertexOuts.end(),
                      ShaderOutput("vOut_RandSeeds1",
                                   DataLine(VertexInputNode::GetInstanceName(), 1)));
    vertexOuts.insert(vertexOuts.end(),
                      ShaderOutput("vOut_RandSeeds2",
                                   DataLine(VertexInputNode::GetInstanceName(), 2)));


    //Next, use the geometry shader to turn points into quads.
    //This shader uses the "size", "quad rotation", and "world position" outputs.

    DataNode::CurrentShader = SH_GEOMETRY;
    RenderIOAttributes geoOuts = ParticleVertex::GetFragInputData();
    MaterialUsageFlags geoUsage;
    geoUsage.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_FORWARD);
    geoUsage.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_SIDEWAYS);
    geoUsage.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_UPWARDS);
    geoUsage.EnableFlag(MaterialUsageFlags::DNF_USES_VIEWPROJ_MAT);
    DataNode::GeometryShader = GeoShaderData(geoOuts, geoUsage, 4, PT_POINTS, PT_TRIANGLE_STRIP,
                                             UniformDictionary(), " ");

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
                typeT = GPUPOutputs::GPUP_QUADROTATION;
                typeName = "GPUP_QUADROTATION";
                break;
            case 2:
                typeT = GPUPOutputs::GPUP_WORLDPOSITION;
                typeName = "GPUP_WORLDPOSITION";
                break;

            default: assert(false);
        }

        DataLine dat = outputs[typeT];
        if (dat.IsConstant())
        {
            continue;
        }

        DataNode* pNode = dat.GetNode();
        std::string action;
        try
        {
            action = "Invalid input chain";
            pNode->AssertAllInputsValid();
            action = "Error setting flags";
            pNode->SetFlags(DataNode::GeometryShader.UsageFlags, dat.GetNonConstantOutputIndex());
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
            return ShaderGenerator::GeneratedMaterial(action + " for channel '" + typeName +
                                                          "': " + pNode->GetError());
        }
    }
#pragma warning(default: 4101)
    
    DataNode::GeometryShader.ShaderCode += "\n\n//Helper functions.\n";
    for (unsigned int i = 0; i < functionDecls.size(); ++i)
    {
        DataNode::GeometryShader.ShaderCode += functionDecls[i];
    }
    
    //Define quaternion rotation.
    if (!outputs[GPUPOutputs::GPUP_QUADROTATION].IsConstant(0.0f))
    {
        DataNode::GeometryShader.ShaderCode += "                                 \n\
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
    gOut_UV = vec2(1.0, 1.0);                                                   \n\
    gOut_ParticleID = " + DataNode::MaterialOuts.VertexOutputs[0].Name + "[0];  \n\
    gOut_RandSeeds1 = " + DataNode::MaterialOuts.VertexOutputs[1].Name + "[0];  \n\
    gOut_RandSeeds2 = " + DataNode::MaterialOuts.VertexOutputs[2].Name + "[0];  \n\
    EmitVertex();                                                               \n\
                                                                                \n\
    cornerPos = (-up * " + sizeY + ") + (side * " + sizeX + ");                 \n\
" + rotByQuat + "                                                               \n\
    gl_Position = " + vpTransf + "pos + cornerPos, 1.0));                       \n\
    gOut_UV = vec2(1.0, 0.0);                                                   \n\
    gOut_ParticleID = " + DataNode::MaterialOuts.VertexOutputs[0].Name + "[0];  \n\
    gOut_RandSeeds1 = " + DataNode::MaterialOuts.VertexOutputs[1].Name + "[0];  \n\
    gOut_RandSeeds2 = " + DataNode::MaterialOuts.VertexOutputs[2].Name + "[0];  \n\
    EmitVertex();                                                               \n\
                                                                                \n\
    cornerPos = (up * " + sizeY + ") - (side * " + sizeX + ");                  \n\
" + rotByQuat + "                                                               \n\
    gl_Position = " + vpTransf + "pos + cornerPos, 1.0));                       \n\
    gOut_UV = vec2(0.0, 1.0);                                                   \n\
    gOut_ParticleID = " + DataNode::MaterialOuts.VertexOutputs[0].Name + "[0];  \n\
    gOut_RandSeeds1 = " + DataNode::MaterialOuts.VertexOutputs[1].Name + "[0];  \n\
    gOut_RandSeeds2 = " + DataNode::MaterialOuts.VertexOutputs[2].Name + "[0];  \n\
    EmitVertex();                                                               \n\
                                                                                \n\
    cornerPos = -((up * " + sizeY + ") + (side * " + sizeX + "));               \n\
" + rotByQuat + "                                                               \n\
    gl_Position = " + vpTransf + "pos + cornerPos, 1.0));                       \n\
    gOut_UV = vec2(0.0, 0.0);                                                   \n\
    gOut_ParticleID = " + DataNode::MaterialOuts.VertexOutputs[0].Name + "[0];  \n\
    gOut_RandSeeds1 = " + DataNode::MaterialOuts.VertexOutputs[1].Name + "[0];  \n\
    gOut_RandSeeds2 = " + DataNode::MaterialOuts.VertexOutputs[2].Name + "[0];  \n\
    EmitVertex();                                                               \n\
}";


    //Fragment shader just uses the color output.
    fragmentOuts.insert(fragmentOuts.end(),
                        ShaderOutput("fOut_FinalColor",
                                     outputs[GPUPOutputs::GPUP_COLOR]));


    return ShaderGenerator::GenerateMaterial(outUniforms, mode);
}

typedef GPUParticleGenerator GPUPG;
void GPUPG::GenerateGPUPParticles(MeshData& outMesh, GPUPG::NumberOfParticles numb, int randSeed)
{
    //Get the total number of particles and the number of particles in each row/column
    //    (in terms of particle ID).
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
                                            Vector4f(fr.GetZeroToOne(), fr.GetZeroToOne(),
                                                     fr.GetZeroToOne(), fr.GetZeroToOne()),
                                            Vector2f(fr.GetZeroToOne(), fr.GetZeroToOne()));
        }
    }

    //Create the mesh data. Double-check that indices aren't attached.
    outMesh.PrimType = PT_POINTS;
    outMesh.SetVertexData(particles.GetArray(), particles.GetNumbElements(),
                          MeshData::BUF_STATIC, ParticleVertex::GetVertexInputData());
    outMesh.RemoveIndexData();
}