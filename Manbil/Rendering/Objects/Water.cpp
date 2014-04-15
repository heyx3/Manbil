#include "Water.h"


#include "../../OpenGLIncludes.h"
#include "../../RenderDataHandler.h"
#include "../../Math/Higher Math/Terrain.h"
#include "../../TextureSettings.h"
#include "../Texture Management/TextureConverters.h"
#include "../../Math/NoiseGeneration.hpp"

#include "../Materials/Data Nodes/DataNodeIncludes.h"
#include "../Materials/Data Nodes/ShaderGenerator.h"


//TODO: Sample a "water floor" texture and for every water pixel cast a ray down to the ocean floor.
//TODO: Take several "seed maps" and use them for things like horiontalPos offset, base height offset, etc.





//Calculates the height and surface normal for water.
//TODO: Add support for seeded heightmap. Fix Water's interface so that it uses one big shader that combines flow, ripples, and seeded heightmap.
class WaterNode : public DataNode
{
public:

    static unsigned int GetVertexOffsetOutputIndex(void) { return 0; }
    static unsigned int GetSurfaceNormalOutputIndex(void) { return 1; }


    virtual std::string GetName(void) const override { return "rippleHeightNode"; }
    virtual std::string GetOutputName(unsigned int i) const override { assert(i < 2); return GetName() + std::to_string(GetUniqueID()) + (i == 0 ? "_waterHeightOffset" : "waterNormal"); }

    WaterNode(unsigned int _maxRipples = 0, Vector4f * _dp_tsc_h_p = 0, Vector3f * _sXY_sp = 0,
              unsigned int _maxFlows = 0, Vector4f * f_a_p = 0, float * _tsc = 0)
        : DataNode(MakeVector(DataLine(DataNodePtr(new ObjectPosNode()), 0)), MakeVector(3, 3)),
        maxRipples(_maxRipples), dp_tsc_h_p(_dp_tsc_h_p), sXY_sp(_sXY_sp),
        maxFlows(_maxFlows), fl_am_per(f_a_p), tsc(_tsc)
    {
    }

protected:

    //TODO: Finish these functions.

    virtual void GetMyParameterDeclarations(UniformDictionary & outUniforms) const override
    {
        if (maxRipples > 0)
        {
            outUniforms.FloatArrayUniforms["dropoffPoints_timesSinceCreated_heights_periods"] = UniformArrayValueF((float*)dp_tsc_h_p, maxRipples, 4, "dropoffPoints_timesSinceCreated_heights_periods");
            outUniforms.FloatArrayUniforms["sourcesXY_speeds"] = UniformArrayValueF(&sXY_sp[0][0], maxRipples, 3, "sourcesXY_speeds");
        }
        if (maxFlows > 0)
        {
            outUniforms.FloatArrayUniforms["flow_amplitude_period"] = UniformArrayValueF(&fl_am_per[0][0], maxFlows, 4, "flows_amplitudes_periods");
            outUniforms.FloatArrayUniforms["timesSinceCreated"] = UniformArrayValueF(tsc, maxFlows, 1, "timesSinceCreated");
        }
    }
    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const override
    {
        std::string func =
"float getWaveHeight(vec2 horizontalPos)\n\
{\n\
    float offset = 0.0f;                                                                           \n";
        if (maxRipples > 0)
        {
            std::string dptschp = "dropoffPoints_timesSinceCreated_heights_periods[i]",
                        sxysp = "sourcesXY_speeds[i]";
            func +=
"    //Ripples.                                                                                     \n\
    for (int i = 0; i < " + std::to_string(maxRipples) + "; ++i)                                    \n\
    {                                                                                               \n\
        float dropoffPoint = " + dptschp + ".x;                                                     \n\
        float timeSinceCreated = " + dptschp + ".y;                                                 \n\
        float height = " + dptschp + ".z;                                                           \n\
        float period = " + dptschp + ".w;                                                           \n\
        vec2 source = " + sxysp + ".xy;                                                             \n\
        float speed = " + sxysp + ".z;                                                              \n\
                                                                                                    \n\
        float dist = distance(source, horizontalPos);                                               \n\
        float heightScale = max(0, mix(0.0, 1.0, 1.0 - (dist / dropoffPoint)));                     \n\
        heightScale = pow(heightScale, 3.0); //TODO: turn into a uniform.                           \n\
                                                                                                    \n\
        float cutoff = period * speed * timeSinceCreated;                                           \n\
        cutoff = max(0, (cutoff - dist) / cutoff);                                                  \n\
                                                                                                    \n\
        float innerVal = (dist / period) + (-timeSinceCreated * speed);                             \n\
        float waveScale = height * heightScale * cutoff;                                            \n\
                                                                                                    \n\
        float heightOffset = sin(innerVal);                                                         \n\
        heightOffset = -1.0 + 2.0 * pow(0.5 + (0.5 * heightOffset), 2.0); //TODO: Make uniform.     \n\
        offset += waveScale * heightOffset;                                                         \n\
    }\n";
        }
        if (maxFlows > 0)
        {
            std::string fap = "flows_amplitudes_periods[i]",
                        tsc = "timesSinceCreated[i]";
            func +=
"    //Directional flows.                                                                           \n\
     for (int i = 0; i < " + std::to_string(maxFlows) + "; ++i)                                     \n\
     {                                                                                              \n\
         vec2 flowDir = " + fap + ".xy;                                                             \n\
         float speed = length(flowDir);                                                             \n\
         flowDir /= speed;                                                                          \n\
         float amplitude = " + fap + ".z;                                                           \n\
         float period = " + fap + ".w;                                                              \n\
         float timeSinceCreated = " + tsc + ";                                                      \n\
                                                                                                    \n\
         float dist = dot(flowDir, horizontalPos);                                                  \n\
                                                                                                    \n\
         float innerVal = (dist / period) + (-timeSinceCreated * speed);                            \n\
         float waveScale = amplitude;                                                               \n\
                                                                                                    \n\
         float heightOffset = sin(innerVal);                                                        \n\
         heightOffset = -1.0 + 2.0 * pow(0.5 + (0.5 * heightOffset), 2.0); //TODO: Make uniform.    \n\
         offset += waveScale * heightOffset;                                                        \n\
     }\n";
        }
        func +=
"    return offset;                                                                                 \n\
}\n";
        outDecls.insert(outDecls.end(), func);

        if (GetShaderType() == Shaders::SH_Fragment_Shader)
        {
            func = std::string() +
"struct NormalData                                                                      \n\
{                                                                                       \n\
    vec3 normal, tangent, bitangent;                                                    \n\
};                                                                                      \n\
NormalData getWaveNormal(vec2 horizontalPos)                                            \n\
{                                                                                       \n\
    NormalData dat;                                                                     \n\
    dat.normal = vec3(0.0, 0.0, 0.001);                                                 \n\
    dat.tangent = vec3(0.001, 0.0, 0.0);                                                \n\
    dat.bitangent = vec3(0.0, 0.001, 0.0);                                              \n\
                                                                                        \n\
    vec2 epsilon = vec2(0.1);                                                           \n\
                                                                                        \n\
    //Get the height at nearby vertices and compute the normal via cross-product.       \n\
                                                                                        \n\
    vec2 one_zero = horizontalPos + vec2(epsilon.x, 0.0f),                              \n\
         nOne_zero = horizontalPos + vec2(-epsilon.x, 0.0f),                            \n\
         zero_one = horizontalPos + vec2(0.0f, epsilon.y),                              \n\
         zero_nOne = horizontalPos + vec2(0.0f, -epsilon.y);                            \n\
                                                                                        \n\
    vec3 p_zero_zero = vec3(horizontalPos, getWaveHeight(horizontalPos));               \n\
    vec3 p_one_zero = vec3(one_zero, getWaveHeight(one_zero)),                          \n\
         p_nOne_zero = vec3(nOne_zero, getWaveHeight(nOne_zero)),                       \n\
         p_zero_one = vec3(zero_one, getWaveHeight(zero_one)),                          \n\
         p_zero_nOne = vec3(zero_nOne, getWaveHeight(zero_nOne));                       \n\
                                                                                        \n\
    vec3 norm1 = cross(normalize(p_one_zero - p_zero_zero),                             \n\
                       normalize(p_zero_one - p_zero_zero)),                            \n\
         norm2 = cross(normalize(p_nOne_zero - p_zero_zero),                            \n\
                       normalize(p_zero_nOne - p_zero_zero)),                           \n\
         normFinal = normalize((norm1 * sign(norm1.z)) + (norm2 * sign(norm2.z)));      \n\
                                                                                        \n\
    dat.normal = normFinal;                                                             \n\
    return dat;                                                                         \n\
}                                                                                       \n\
";
            outDecls.insert(outDecls.end(), func);
        }
    }
    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        switch (GetShaderType())
        {
            case Shaders::SH_Vertex_Shader:
                outCode += "\tvec3 " + GetOutputName(GetVertexOffsetOutputIndex()) + " = vec3(0.0, 0.0, getWaveHeight(" + GetObjectPosInput().GetValue() + ".xy));\n";
                break;
            case Shaders::SH_Fragment_Shader:
                outCode += "\tvec3 " + GetOutputName(GetSurfaceNormalOutputIndex()) + " = getWaveNormal(" + GetObjectPosInput().GetValue() + ".xy).normal;\n";
                break;

            default: assert(false);
        }
    }

private:

    unsigned int maxRipples, maxFlows;
    Vector4f * dp_tsc_h_p;
    Vector3f * sXY_sp;
    Vector4f * fl_am_per;
    float * tsc;

    const DataLine & GetObjectPosInput(void) const { return GetInputs()[0]; }
};





void CreateWaterMesh(unsigned int size, Vector3f scle, Mesh & outM)
{
    Vector3f offset(size * -0.5f, size * -0.5f, 0.0f);

    //Create some random noise.

    Noise2D noise(size, size);

    //Layered Perin noise.
    int scale = (size > 256 ? 8 : (size > 128 ? 4 : (size > 64 ? 2 : 1)));
    Perlin2D per1(scale * 32.0f, Perlin2D::Smoothness::Quintic, Vector2i(), 135213),
             per2(scale * 16.0f, Perlin2D::Smoothness::Quintic, Vector2i(), 3523),
             per3(scale * 8.0f, Perlin2D::Smoothness::Quintic, Vector2i(), 24623),
             per4(scale * 4.0f, Perlin2D::Smoothness::Quintic, Vector2i(), 136),
             per5(scale * 2.0f, Perlin2D::Smoothness::Quintic, Vector2i(), 24675476),
             per6(scale * 1.0f, Perlin2D::Smoothness::Quintic, Vector2i(), 3463);
    Generator * gens[] = { &per3, &per4, &per5, &per6 };
    float weights[] = { 0.5f, 0.25f, 0.125f, 0.0625f, 0.03125f, 0.1f };
    LayeredOctave octaves(4, weights, gens);

    //Filter the layered Perlin noise to have less contrast.
    NoiseFilterer nf;
    MaxFilterRegion mfr;
    mfr.StrengthLerp = 0.0f;
    nf.FillRegion = &mfr;
    nf.NoiseToFilter = &octaves;
    nf.FilterFunc = &NoiseFilterer::Average;

    nf.Generate(noise);


    //Just create a flat terrain and let it do the math.

    Terrain terr(size);
    //Put the noise into the terrain heightmap so that the terrain class will automatically put each noise value into the correct vertex.
    //However, because we're putting noise into the z coordinate, don't calculate normals until after the noise is extracted.
    terr.SetHeightmap(noise);
    int nVs = terr.GetVerticesCount(),
        nIs = terr.GetIndicesCount();
    Vector3f * poses = new Vector3f[nVs];
    Vector2f * texCoords = new Vector2f[nVs];
    Vector3f * normals = new Vector3f[nVs];
    terr.CreateVertexPositions(poses);
    terr.CreateVertexTexCoords(texCoords);

    Vertex * vertices = new Vertex[nVs];
    FastRand fr(146230);
    for (int i = 0; i < nVs; ++i)
    {
        //Red and Green color channels are already used to store the object-space vertex position.
        //Use Blue to store randomized values for variation in the water surface. Use the noise that was generated and put into the vertex Z coordinates.
        vertices[i] = Vertex(scle.ComponentProduct(Vector3f(poses[i].x, poses[i].y, 0.0f)) + offset,
                             texCoords[i],
                             Vector4f(0.0f, 0.0f, -1.0f + (2.0f * poses[i].z), (12.0f * poses[i].z)),
                             Vector3f());
        poses[i].z = 0.0f;
    }
    delete[] texCoords;

    terr.CreateVertexNormals(normals, poses, scle);
    for (int i = 0; i < nVs; ++i)
    {
        vertices[i].Normal = normals[i];
    }
    delete[] normals, poses;

    unsigned int * indices = new unsigned int[nIs];
    terr.CreateVertexIndices(indices);
    
    //Create the vertex and index buffers.
    RenderObjHandle vbo, ibo;
    RenderDataHandler::CreateVertexBuffer(vbo, vertices, nVs, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
    RenderDataHandler::CreateIndexBuffer(ibo, indices, nIs, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
    VertexIndexData vid(terr.GetVerticesCount(), vbo, terr.GetIndicesCount(), ibo);
    outM.SetVertexIndexData(&vid, 1);

    delete[] vertices, indices;
}

Water::Water(unsigned int size, Vector3f pos, Vector3f scale,
             OptionalValue<RippleWaterCreationArgs> rippleArgs,
             OptionalValue<DirectionalWaterCreationArgs> directionArgs,
             OptionalValue<SeedmapWaterCreationArgs> seedmapArgs,
             RenderingModes mode, bool useLighting, LightSettings settings,
             RenderChannels & channels)
    : currentRippleIndex(0), totalRipples(0), nextRippleID(0),
      currentFlowIndex(0), totalFlows(0), nextFlowID(0),
      rippleIDs(0), dp_tsc_h_p(0), sXY_sp(0),
      flowIDs(0), f_a_p(0), tsc(0),
      waterMat(0), waterMesh(PrimitiveTypes::Triangles)
{
    //Create mesh.
    CreateWaterMesh(size, scale, waterMesh);
    waterMesh.Transform.SetPosition(pos);

    //Set up ripples.
    if (rippleArgs.HasValue())
    {
        RippleWaterCreationArgs ripArgs = rippleArgs.GetValue();
        maxRipples = ripArgs.MaxRipples;

        rippleIDs = new unsigned int[maxRipples];
        dp_tsc_h_p = new Vector4f[maxRipples];
        sXY_sp = new Vector3f[maxRipples];
        for (unsigned int i = 0; i < maxRipples; ++i)
        {
            dp_tsc_h_p[i].w = 999.0f;
            dp_tsc_h_p[i].x = 0.001f;
            sXY_sp[i].z = 0.001f;
        }
        waterMesh.Uniforms.FloatArrayUniforms["dropoffPoints_timesSinceCreated_heights_periods"] = UniformArrayValueF(&(dp_tsc_h_p[0][0]), maxRipples, 4, "dropoffPoints_timesSinceCreated_heights_periods");
        waterMesh.Uniforms.FloatArrayUniforms["sourcesXY_speeds"] = UniformArrayValueF(&(sXY_sp[0][0]), maxRipples, 3, "sourcesXY_speeds");
    }
    else
    {
        maxRipples = 0;
    }

    //Set up flows.
    if (directionArgs.HasValue())
    {
        DirectionalWaterCreationArgs dirArgs = directionArgs.GetValue();
        maxFlows = dirArgs.MaxFlows;

        flowIDs = new unsigned int[maxFlows];
        f_a_p = new Vector4f[maxFlows];
        tsc = new float[maxFlows];

        for (unsigned int i = 0; i < maxFlows; ++i)
        {
            f_a_p[i] = Vector4f(0.001f, 0.0f, 0.0f, 9999.0f);
            tsc[i] = 0.0f;
        }
        waterMesh.Uniforms.FloatArrayUniforms["flows_amplitudes_periods"] = UniformArrayValueF(&f_a_p[0][0], maxFlows, 4, "flows_amplitudes_periods");
        waterMesh.Uniforms.FloatArrayUniforms["timesSinceCreated"] = UniformArrayValueF(tsc, maxFlows, 1, "timesSinceCreated");
    }
    else
    {
        maxFlows = 0;
    }

    //Set up seedmap.
    if (seedmapArgs.HasValue())
    {
        SeedmapWaterCreationArgs seedArgs = seedmapArgs.GetValue();

        assert(seedArgs.SeedValues->GetWidth() == seedArgs.SeedValues->GetHeight());

        waterMesh.Uniforms.FloatUniforms["amplitude_period_speed"] = UniformValueF(Vector3f(1.0f, 1.0f, 1.0f), "amplitude_period_speed");
        waterMesh.Uniforms.FloatUniforms["seedMapResolution"] = UniformValueF(Vector2f(seedArgs.SeedValues->GetWidth(), seedArgs.SeedValues->GetHeight()), "seedMapResolution");


        //Create a texture from the seed map.

        sf::Image img;
        img.create(seedArgs.SeedValues->GetWidth(), seedArgs.SeedValues->GetHeight());
        TextureConverters::ToImage<float>(*seedArgs.SeedValues, img, (void*)0, [](void* pd, float inF) { sf::Uint8 cmp = (sf::Uint8)BasicMath::RoundToInt(inF * 255.0f); return sf::Color(cmp, cmp, cmp, 255); });

        sf::Texture * seedHeightmap = (*seedArgs.TexManager)[seedArgs.TexManager->CreateSFMLTexture()].SFMLTex;
        seedHeightmap->loadFromImage(img);
        seedHeightmap->setSmooth(false);
        seedHeightmap->setRepeated(true);
        waterMesh.Uniforms.TextureUniforms["seedMap"] = UniformSamplerValue(seedHeightmap, "seedMap");
    }

    //Set up lighting.
    if (useLighting)
    {
        SetLighting(DirectionalLight(0.8f, 0.2f, Vector3f(1, 1, 1), Vector3f(1.0f, 1.0f, -1.0f).Normalized()));
    }

    //Generate shader code.
    std::string vertexShader, fragmentShader;
    DataNodePtr waterNode(new WaterNode(maxRipples, dp_tsc_h_p, sXY_sp, maxFlows));
    ShaderGenerator::AddMissingChannels(channels, mode, useLighting, settings);
    channels[RenderingChannels::RC_ObjectVertexOffset] =
        DataLine(DataNodePtr(new AddNode(channels[RenderingChannels::RC_ObjectVertexOffset], DataLine(waterNode, WaterNode::GetVertexOffsetOutputIndex()))), 0);
    //TODO: Figure out how to correctly combine previous normal channel value with water channel.
    if (ShaderGenerator::IsChannelUsed(RenderingChannels::RC_Normal, mode, useLighting, settings))
        channels[RenderingChannels::RC_Normal] = DataLine(DataNodePtr(new NormalizeNode(DataLine(DataNodePtr(new AddNode(DataLine(waterNode, WaterNode::GetSurfaceNormalOutputIndex()),
                                                                                                                         channels[RenderingChannels::RC_Normal])),
                                                                                        0))),
                                                          0);
    UniformDictionary dict;
    ShaderGenerator::GenerateShaders(vertexShader, fragmentShader, dict, mode, useLighting, settings, channels);
    waterMesh.Uniforms.AddUniforms(dict, false);

    //Create the material.
    waterMat = new Material(vertexShader, fragmentShader, waterMesh.Uniforms, mode, useLighting, settings);
    if (waterMat->HasError())
    {
        errorMsg = "Error creating water material: ";
        errorMsg += waterMat->GetErrorMsg();
        return;
    }
}
Water::~Water(void)
{
    delete waterMat;
    waterMesh.DeleteVertexIndexBuffers();

    if (rippleIDs != 0)
    {
        delete[] rippleIDs, dp_tsc_h_p, sXY_sp;
    }
    if (flowIDs != 0)
    {
        delete[] flowIDs, f_a_p, tsc;
    }
}

unsigned int Water::AddRipple(const RippleWaterArgs & args)
{
    assert(maxRipples > 0);

    RippleWaterArgs cpy(args);

    //Translate the source into object space.
    Matrix4f inv;
    waterMesh.Transform.GetWorldTransform(inv);
    inv = inv.GetInverse();
    cpy.Source = inv.Apply(args.Source);
    

    //Update tracking values.
    unsigned int rippleID = nextRippleID;
    nextRippleID += 1;
    unsigned int index = currentRippleIndex;
    currentRippleIndex += 1;
    currentRippleIndex %= maxRipples;

    //Make sure uniform values aren't invalid.
    if (args.DropoffPoint <= 0.0f)
        cpy.DropoffPoint = 9999.0f;
    if (args.Period <= 0.0f)
        cpy.Period = 5.0f;
    if (args.Speed <= 0.0f)
        cpy.Speed = 1.0f;

    //Set the uniforms.
    dp_tsc_h_p[index] = Vector4f(cpy.DropoffPoint, cpy.TimeSinceCreated, cpy.Amplitude, cpy.Period);
    sXY_sp[index] = Vector3f(cpy.Source.x, cpy.Source.y, cpy.Speed);
    rippleIDs[index] = rippleID;


    return rippleID;
}
bool Water::ChangeRipple(unsigned int element, const RippleWaterArgs & args)
{
    assert(maxRipples > 0);

    for (int i = 0; i < maxRipples; ++i)
    {
        if (rippleIDs[i] == element)
        {
            //Translate the source into object space.
            Matrix4f inv;
            waterMesh.Transform.GetWorldTransform(inv);
            inv = inv.GetInverse();
            Vector3f sourcePos = args.Source;
            sourcePos = inv.Apply(sourcePos);

            //Set the uniforms.
            dp_tsc_h_p[i] = Vector4f(args.DropoffPoint, args.TimeSinceCreated, args.Amplitude, args.Period);
            sXY_sp[i] = Vector3f(sourcePos.x, sourcePos.y, args.Speed);

            return true;
        }
    }

    return false;
}

unsigned int Water::AddFlow(const DirectionalWaterArgs & args)
{
    assert(maxFlows > 0);

    //Create a copy of the arguments so that any invalid uniform values can be changed.
    DirectionalWaterArgs cpy(args);
    if (cpy.Flow == Vector2f())
        cpy.Flow = Vector2f(0.001f, 0.0f);
    if (cpy.Period <= 0.0f)
        cpy.Period = 0.001f;

    //Update tracking values.
    unsigned int flowID = nextFlowID;
    nextFlowID += 1;
    unsigned int index = currentFlowIndex;
    currentFlowIndex += 1;
    currentFlowIndex %= maxFlows;

    //Set the uniforms.
    f_a_p[index] = Vector4f(cpy.Flow.x, cpy.Flow.y, cpy.Amplitude, cpy.Period);
    tsc[index] = cpy.TimeSinceCreated;


    return flowID;
}
bool Water::ChangeFlow(unsigned int element, const DirectionalWaterArgs & args)
{
    assert(maxFlows > 0);

    for (int i = 0; i < maxFlows; ++i)
    {
        if (flowIDs[i] == element)
        {
            //Set the uniforms.
            f_a_p[i] = Vector4f(args.Flow.x, args.Flow.y, args.Amplitude, args.Period);
            tsc[i] = args.TimeSinceCreated;

            return true;
        }
    }

    return false;
}

void Water::SetSeededWater(const SeededWaterArgs & args)
{
    Vector3f data(args.Amplitude, args.Period, args.Speed);
    waterMesh.Uniforms.FloatUniforms["amplitude_period_speed"].SetValue(data);
}
void Water::SetSeededWaterSeed(sf::Texture * image, bool deletePrevious, Vector2i resolution)
{
    waterMesh.Uniforms.FloatUniforms["seedMapResolution"].SetValue(Vector2f(resolution.x, resolution.y));
    if (deletePrevious)
        waterMesh.Uniforms.TextureUniforms["seedMap"].Texture.DeleteTexture();
    waterMesh.Uniforms.TextureUniforms["seedMap"].Texture.SetData(image);
}


void Water::SetLighting(const DirectionalLight & light)
{
    waterMesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_AmbientName].SetValue(light.AmbientIntensity);
    waterMesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_DiffuseName].SetValue(light.DiffuseIntensity);
    waterMesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_DirName].SetValue(light.Direction);
    waterMesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_ColorName].SetValue(light.Color);
}

void Water::Update(float elapsed)
{
    if (maxRipples > 0)
    {
        //Keep in mind that negative time values indicate the ripple source was stopped and is fading out.
        for (unsigned int i = 0; i < maxRipples; ++i)
            if (dp_tsc_h_p[i].y >= 0.0f)
                dp_tsc_h_p[i].y += elapsed;
            else dp_tsc_h_p[i].y -= elapsed;

    }

    if (maxFlows > 0)
    {
        for (unsigned int i = 0; i < maxFlows; ++i)
        {
            if (tsc[i] > 0.0f)
                tsc[i] += elapsed;
            else tsc[i] -= elapsed;
        }
    }
}
bool Water::Render(const RenderInfo & info)
{
    //Set water uniforms.
    if (maxRipples > 0)
    {
        waterMesh.Uniforms.FloatArrayUniforms["dropoffPoints_timesSinceCreated_heights_periods"].SetData(&(dp_tsc_h_p[0][0]));
        waterMesh.Uniforms.FloatArrayUniforms["sourcesXY_speeds"].SetData(&(sXY_sp[0][0]));
    }
    if (maxFlows > 0)
    {
        waterMesh.Uniforms.FloatArrayUniforms["flows_amplitudes_periods"].SetData(&f_a_p[0][0]);
        waterMesh.Uniforms.FloatArrayUniforms["timesSinceCreated"].SetData(tsc);
    }


    //Render.
    std::vector<const Mesh*> meshes;
    meshes.insert(meshes.end(), &waterMesh);
    if (!waterMat->Render(RenderPasses::BaseComponents, info, meshes))
    {
        errorMsg = "Error rendering water: ";
        errorMsg += waterMat->GetErrorMsg();
        return false;
    }
    else return true;
}