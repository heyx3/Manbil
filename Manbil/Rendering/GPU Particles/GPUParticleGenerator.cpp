#include "GPUParticleGenerator.h"

#include "../Materials/Data Nodes/DataNodeIncludes.h"
#include "../../DebugAssist.h"
#include "../../Math/Array2D.h"


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
    DataNodePtr worldPos(new CombineVectorNode(outputs[GPUPOutputs::GPUP_WORLDPOSITION], DataLine(VectorF(1.0f)), "worldPos"));
    DataNode::MaterialOuts.VertexPosOutput = std::string("worldPos");
    vertexOuts.insert(vertexOuts.end(), ShaderOutput("vOut_particleID", DataLine(VertexInputNode::GetInstance()->GetName(), 0)));
    vertexOuts.insert(vertexOuts.end(), ShaderOutput("vOut_randSeeds1", DataLine(VertexInputNode::GetInstance()->GetName(), 1)));
    vertexOuts.insert(vertexOuts.end(), ShaderOutput("vOut_randSeeds2", DataLine(VertexInputNode::GetInstance()->GetName(), 2)));


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
 
#pragma warning(disable: 4101)
    if (!outputs[GPUPOutputs::GPUP_SIZE].IsConstant())
    {
        DataNode* sizeNode = outputs[GPUPOutputs::GPUP_SIZE].GetNode();
        if (sizeNode == 0) return "Size input node '" + outputs[GPUPOutputs::GPUP_SIZE].GetNonConstantValue() + "' doesn't exist!";

        try
        {
            sizeNode->SetFlags(DataNode::GeometryShader.UsageFlags, outputs[GPUPOutputs::GPUP_SIZE].GetSize());
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial("Error setting flags for channel 'GPUP_SIZE': " + sizeNode->GetError());
        }
        try
        {
            sizeNode->GetParameterDeclarations(DataNode::GeometryShader.Params, usedNodesParams);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial("Error getting params for channel 'GPUP_SIZE': " + sizeNode->GetError());
        }
        try
        {
            sizeNode->GetFunctionDeclarations(functionDecls, usedNodesFuncs);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial("Error writing functions for channel 'GPUP_SIZE': " + sizeNode->GetError());
        }
        try
        {
            sizeNode->WriteOutputs(finalOutputs, writtenNodeIDs);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial("Error writing outputs for channel 'GPUP_SIZE': " + sizeNode->GetError());
        }
    }
    if (!outputs[GPUPOutputs::GPUP_QUADROTATION].IsConstant())
    {
        DataNode* rotNode = outputs[GPUPOutputs::GPUP_QUADROTATION].GetNode();
        if (rotNode == 0) return "Rotation input node '" + outputs[GPUPOutputs::GPUP_QUADROTATION].GetNonConstantValue() + "' doesn't exist!";

        try
        {
            rotNode->SetFlags(DataNode::GeometryShader.UsageFlags, outputs[GPUPOutputs::GPUP_QUADROTATION].GetNonConstantOutputIndex());
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial("Error setting flags for channel 'GPUP_QUADROTATION': " + rotNode->GetError());
        }
        try
        {
            rotNode->GetParameterDeclarations(DataNode::GeometryShader.Params, usedNodesParams);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial("Error getting params for channel 'GPUP_QUADROTATION': " + rotNode->GetError());
        }
        try
        {
            rotNode->GetFunctionDeclarations(functionDecls, usedNodesFuncs);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial(std::string() + "Error writing functions for channel 'GPUP_QUADROTATION': " + rotNode->GetError());
        }
        try
        {
            rotNode->WriteOutputs(finalOutputs, writtenNodeIDs);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial("Error writing outputs for channel 'GPUP_QUADROTATION': " + rotNode->GetError());
        }
    }
    if (!outputs[GPUPOutputs::GPUP_WORLDPOSITION].IsConstant())
    {
        DataNode* posNode = outputs[GPUPOutputs::GPUP_WORLDPOSITION].GetNode();
        if (posNode == 0) return "Position input node '" + outputs[GPUPOutputs::GPUP_WORLDPOSITION].GetNonConstantValue() + "' doesn't exist!";

        try
        {
            posNode->SetFlags(DataNode::GeometryShader.UsageFlags, outputs[GPUPOutputs::GPUP_WORLDPOSITION].GetNonConstantOutputIndex());
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial("Error setting flags for channel 'GPUP_WORLDPOSITION': " + posNode->GetError());
        }
        try
        {
            posNode->GetParameterDeclarations(DataNode::GeometryShader.Params, usedNodesParams);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial("Error getting params for channel 'GPUP_WORLDPOSITION': " + posNode->GetError());
        }
        try
        {
            posNode->GetFunctionDeclarations(functionDecls, usedNodesFuncs);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial(std::string() + "Error writing functions for channel 'GPUP_WORLDPOSITION': " + posNode->GetError());
        }
        try
        {
            posNode->WriteOutputs(finalOutputs, writtenNodeIDs);
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return ShaderGenerator::GeneratedMaterial("Error writing outputs for channel 'GPUP_WORLDPOSITION': " + posNode->GetError());
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
    DataNode::MaterialOuts.FragmentOutputs.insert(DataNode::MaterialOuts.FragmentOutputs.end(),
                                                  ShaderOutput("out_particleColor", outputs[GPUPOutputs::GPUP_COLOR]));


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