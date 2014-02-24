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
//TODO: Use two normal maps and interpolate between them using the per-vertex random seed.

void CreateWaterMesh(unsigned int size, Mesh & outM)
{
    Vector3f offset(size * -0.5f, size * -0.5f, 0.0f);

    //Create some random noise.

    Noise2D noise(size, size);

    //Layered Perin noise.
    Perlin per1(32.0f, Perlin::Smoothness::Quintic, 135213),
           per2(16.0f, Perlin::Smoothness::Quintic, 135213),
           per3(8.0f, Perlin::Smoothness::Quintic, 135213),
           per4(4.0f, Perlin::Smoothness::Quintic, 135213),
           per5(2.0f, Perlin::Smoothness::Quintic, 135213),
           per6(1.0f, Perlin::Smoothness::Quintic, 135213);
    Generator * gens[] = { &per1, &per2, &per3, &per4, &per5, &per6 };
    float weights[6];
    float weight = 0.5f; for (int i = 0; i < 6; ++i) { weights[i] = weight; weight *= 0.5f; }
    LayeredOctave octaves(6, weights, gens);

    //Filter the layered Perlin noise to have less contrast.
    NoiseFilterer nf;
    MaxFilterRegion mfr;
    mfr.StrengthLerp = 0.25f;
    nf.FillRegion = &mfr;
    nf.NoiseToFilter = &octaves;
    nf.FilterFunc = &NoiseFilterer::Average;

    nf.Generate(noise);

    Value2D().Generate(noise);


    //Just create a flat terrain and let it do the math.

    Terrain terr(size);
    //Put the noise into the terrain heightmap so that the terrain class will automatically put each noise value into the correct vertex.
    terr.SetHeightmap(noise);
    int nVs = terr.GetVerticesCount(),
        nIs = terr.GetIndicesCount();
    Vector3f * poses = new Vector3f[nVs];
    Vector2f * texCoords = new Vector2f[nVs];
    Vector3f * normals = new Vector3f[nVs];
    terr.CreateVertexPositions(poses);
    terr.CreateVertexNormals(normals, poses, Vector3f(1, 1, 1));
    terr.CreateVertexTexCoords(texCoords);

    Vertex * vertices = new Vertex[nVs];
    FastRand fr(146230);
    for (int i = 0; i < nVs; ++i)
    {
        //Red and Green color channels are already used to store the object-space vertex position.
        //Use Blue to store randomized values for variation in the water surface. Use the noise that was generated and put into the vertex Z coordinates.
        vertices[i] = Vertex(Vector3f(poses[i].x, poses[i].y, 0.0f) + offset,
                             texCoords[i],
                             Vector4f(0.0f, 0.0f, poses[i].z, fr.GetZeroToOne()),
                             normals[i]);
    }
    delete[] poses, texCoords, normals;

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

Water::Water(unsigned int size, unsigned int maxRipples, Vector3f pos, RenderingModes mode, bool useLighting, LightSettings settings)
    : currentRippleIndex(0), maxRipples(maxRipples), nextRippleID(0), totalRipples(0),
      rippleIDs(0), dp_tsc_h_p(0), sXY_sp(0),
      waterMat(0), waterMesh(PrimitiveTypes::Triangles), waterType(WaterTypes::Rippling)
{
    //Create mesh.
    CreateWaterMesh(size, waterMesh);
    waterMesh.Transform.SetPosition(pos);

    //Set up shaders and uniforms.
    MaterialShaderData dat = GetRippleWaterShaderData(maxRipples, useLighting);
    waterMesh.Uniforms = dat.Uniforms;
    rippleIDs = new int[maxRipples];
    dp_tsc_h_p = new Vector4f[maxRipples];
    sXY_sp = new Vector3f[maxRipples];
    for (unsigned int i = 0; i < maxRipples; ++i)
    {
        dp_tsc_h_p[i].w = 999.0f;
        dp_tsc_h_p[i].x = 0.001f;
        sXY_sp[i].z = 0.001f;
    }
    waterMesh.Uniforms.FloatArrayUniforms["dropoffPoints_timesSinceCreated_heights_periods"] = UniformArrayValueF(&(dp_tsc_h_p[0][0]), maxRipples, 4, 0, "dropoffPoints_timesSinceCreated_heights_periods");
    waterMesh.Uniforms.FloatArrayUniforms["sourcesXY_speeds"] = UniformArrayValueF(&(sXY_sp[0][0]), maxRipples, 3, 0, "sourcesXY_speeds");


    //Lighting uniforms.
    if (useLighting)
    {
        SetLighting(DirectionalLight(0.8f, 0.2f, Vector3f(1, 1, 1), Vector3f(1.0f, 1.0f, -1.0f).Normalized()));
    }

    //Creating the material.
    waterMat = new Material(dat.VS, dat.FS, dat.Uniforms, mode, useLighting, settings);
    if (waterMat->HasError())
    {
        errorMsg = "Error creating ripple water material: ";
        errorMsg += waterMat->GetErrorMsg();
        return;
    }
}
Water::Water(unsigned int size, unsigned int _maxFlows, Vector3f pos, RenderingModes mode, bool useLighting, LightSettings settings)
    : currentFlowIndex(0), maxFlows(_maxFlows), nextFlowID(0), totalFlows(0),
      waterMat(0), waterMesh(PrimitiveTypes::Triangles), waterType(WaterTypes::Directed),
      rippleIDs(0), dp_tsc_h_p(0), sXY_sp(0)
{
    //Create mesh.
    CreateWaterMesh(size, waterMesh);
    waterMesh.Transform.SetPosition(pos);

    //Set up shaders and uniforms.
    MaterialShaderData dat = GetDirectionalWaterShaderData(maxFlows, useLighting);
    waterMesh.Uniforms = dat.Uniforms;
    DirectionalWaterArgs args(Vector2f(1.0f, 0.0f), 0.0f);
    DirectionalWaterArgsElement argsE(args, -1);
    for (int i = 0; i < maxFlows; ++i)
    {
        flows.insert(flows.end(), argsE);
    }
    //Lighting uniforms.
    if (useLighting)
    {
        SetLighting(DirectionalLight(0.8f, 0.2f, Vector3f(1, 1, 1), Vector3f(1.0f, 1.0f, -1.0f).Normalized()));
    }


    //Creating the material.
    waterMat = new Material(dat.VS, dat.FS, dat.Uniforms, mode, useLighting, settings);
    if (waterMat->HasError())
    {
        errorMsg = "Error creating flowing water material: ";
        errorMsg += waterMat->GetErrorMsg();
        return;
    }
}
Water::Water(const Fake2DArray<float> & seedValues, TextureManager & texM, Vector3f pos, RenderingModes mode, bool useLighting, LightSettings settings)
    : waterMat(0), waterMesh(PrimitiveTypes::Triangles), waterType(WaterTypes::Rippling)
{
    assert(seedValues.GetWidth() == seedValues.GetHeight());


    //Create mesh.
    CreateWaterMesh(seedValues.GetWidth(), waterMesh);
    waterMesh.Transform.SetPosition(pos);

    //Set up uniforms.
    MaterialShaderData dat = GetSeededHeightmapWaterShaderData(useLighting);
    waterMesh.Uniforms = dat.Uniforms;
    waterMesh.Uniforms.FloatUniforms["amplitude_period_speed"] = UniformValueF(Vector3f(1.0f, 1.0f, 1.0f), 0, "amplitude_period_speed");
    waterMesh.Uniforms.FloatUniforms["seedMapResolution"] = UniformValueF(Vector2f(seedValues.GetWidth(), seedValues.GetHeight()), 0, "seedMapResolution");
    if (useLighting)
    {
        SetLighting(DirectionalLight(0.8f, 0.2f, Vector3f(1, 1, 1), Vector3f(1.0f, 1.0f, -1.0f).Normalized()));
    }


    //Create a texture from the seed map.

    sf::Image img;
    img.create(seedValues.GetWidth(), seedValues.GetHeight());
    TextureConverters::ToImage<float>(seedValues, img, (void*)0, [](void* pd, float inF) { sf::Uint8 cmp = (sf::Uint8)BasicMath::RoundToInt(inF * 255.0f); return sf::Color(cmp, cmp, cmp, 255); });
    
    unsigned int id = texM.CreateTexture(seedValues.GetWidth(), seedValues.GetHeight());
    sf::Texture * seedHeightmap = texM.GetTexture(id);
    seedHeightmap->loadFromImage(img);
    seedHeightmap->setSmooth(false);
    seedHeightmap->setRepeated(true);
    waterMesh.Uniforms.TextureUniforms["seedMap"] = UniformSamplerValue(seedHeightmap, 0, "seedMap");



    //Creating the material.
    waterMat = new Material(dat.VS, dat.FS, dat.Uniforms, mode, useLighting, settings);
    if (waterMat->HasError())
    {
        errorMsg = "Error creating flowing water material: ";
        errorMsg += waterMat->GetErrorMsg();
        return;
    }
}

Water::~Water(void)
{
    delete waterMat;

    if (rippleIDs != 0)
    {
        delete[] rippleIDs, dp_tsc_h_p, sXY_sp;
    }
}

int Water::AddRipple(const RippleWaterArgs & args)
{
    if (waterType != WaterTypes::Rippling) return -1;

    RippleWaterArgs cpy(args);

    //Translate the source into object space.
    Matrix4f inv;
    waterMesh.Transform.GetWorldTransform(inv);
    inv = inv.Inverse();
    cpy.Source = inv.Apply(args.Source);
    

    //Update tracking values.
    int rippleID = nextRippleID;
    nextRippleID += 1;
    int index = currentRippleIndex;
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
bool Water::ChangeRipple(int element, const RippleWaterArgs & args)
{
    if (waterType != WaterTypes::Rippling) return false;

    for (int i = 0; i < maxRipples; ++i)
    {
        if (rippleIDs[i] == element)
        {
            //Translate the source into object space.
            Matrix4f inv;
            waterMesh.Transform.GetWorldTransform(inv);
            inv = inv.Inverse();
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

int Water::AddFlow(const DirectionalWaterArgs & args)
{
    if (waterType != WaterTypes::Directed) return -1;

    //Translate the source into object space.
    Matrix4f inv;
    waterMesh.Transform.GetWorldTransform(inv);
    inv = inv.Inverse();

    //TODO: Apply the transformation after putting the ripple into the list.

    return -1;
}
bool Water::ChangeFlow(int element, const DirectionalWaterArgs & args)
{
    if (waterType != WaterTypes::Directed) return false;

    for (int i = 0; i < flows.size(); ++i)
    {
        if (flows[i].Element == element)
        {
            flows[i].Args = args;
            return true;
        }
    }

    return false;
}

bool Water::SetSeededWater(const SeededWaterArgs & args)
{
    if (waterType != WaterTypes::SeededHeightmap) return false;

    Vector3f data(args.Amplitude, args.Period, args.Speed);
    waterMesh.Uniforms.FloatUniforms["amplitude_period_speed"].SetValue(data);

    return true;
}
bool Water::SetSeededWaterSeed(sf::Texture * image, Vector2i resolution)
{
    if (waterType != WaterTypes::SeededHeightmap) return false;

    waterMesh.Uniforms.FloatUniforms["seedMapResolution"].SetValue(Vector2f(resolution.x, resolution.y));
    waterMesh.Uniforms.TextureUniforms["seedMap"].Texture = image;

    return true;
}


void Water::SetLighting(const DirectionalLight & light)
{
    waterMesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_AmbientName] = UniformValueF(light.AmbientIntensity, 0, MaterialConstants::DirectionalLight_AmbientName);
    waterMesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_DiffuseName] = UniformValueF(light.DiffuseIntensity, 0, MaterialConstants::DirectionalLight_DiffuseName);
    waterMesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_DirName] = UniformValueF(light.Direction, 0, MaterialConstants::DirectionalLight_DirName);
    waterMesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_ColorName] = UniformValueF(light.Color, 0, MaterialConstants::DirectionalLight_ColorName);
}


Water::MaterialShaderData Water::GetRippleWaterShaderData(unsigned int maxRipples, RenderingModes mode, bool useLighting, const LightSettings & settings, RenderChannels channels)
{
    std::string n = std::to_string(maxRipples);

    std::string header = MaterialConstants::GetFragmentHeader(useLighting);

    ShaderGenerator::AddMissingChannels(channels, mode, useLighting, settings);
    ShaderGenerator::RemoveUnusedChannels(channels, mode, useLighting, settings);
}

struct RenderingPass { public: std::string vs, fs; RenderingPass(std::string _vs, std::string _fs) : vs(_vs), fs(_fs) { } };
RenderingPass GetRippleWaterRenderer(int maxRipples)
{
    std::string n = std::to_string(maxRipples);
    
    std::string commonGround = std::string() + "\
             //Keep uniforms compacted into vectors so they can be sent to the GPU quicker.\n\
             uniform vec4 dropoffPoints_timesSinceCreated_heights_periods[" + n + "];\n\
             uniform vec3 sourcesXY_speeds[" + n + "];\n\
             \n\
             float getWaveHeight(vec2 horizontalPos)\n\
             {\n\
                float offset = 0.0;\n\
                for (int i = 0; i < " + n + "; ++i)\n\
                {\n\
                    //Extract the uniform data.\n\
                    float dropoffPoint = dropoffPoints_timesSinceCreated_heights_periods[i].x;\n\
                    float timeSinceCreated = dropoffPoints_timesSinceCreated_heights_periods[i].y;\n\
                    float height = dropoffPoints_timesSinceCreated_heights_periods[i].z;\n\
                    float period = dropoffPoints_timesSinceCreated_heights_periods[i].w;\n\
                    vec2 source = sourcesXY_speeds[i].xy;\n\
                    float speed = sourcesXY_speeds[i].z;\n\
                    \n\
                    float dist = distance(source, horizontalPos);\n\
                    float heightScale = max(0, mix(0.0, 1.0, 1.0 - (dist / dropoffPoint)));\n\
                    //TODO: Turn the dropoff exponent into a uniform.\n\
                    heightScale = heightScale * heightScale * heightScale;\n\
                    //'cutoff' will be either 0 or 1 based on how far away this vertex is.\n\
                    //TODO: Smooth cutoff, not a binary 1/0 thing.\n\
                    float cutoff = period * speed * timeSinceCreated;\n\
                    cutoff = max(0, sign(cutoff - dist));\n\
                    \n\
                    float innerVal = (dist / period) + (-timeSinceCreated * speed);\n\
                    float waveScale = height * heightScale * cutoff;\n\
                    \n\
                    float heightOffset = sin(innerVal);\n\
                    //TODO: Add uniform 'waveSharpness' for this 'pow' function.\n\
                    heightOffset = -1.0 + 2.0 * pow(0.5 + 0.5 * heightOffset, 2.0);\n\
                    offset += waveScale * heightOffset;\n\
                }\n\
                return offset;\n\
             }\n\
             \n\
             struct NormalData\n\
             {\n\
                vec3 normal, tangent, bitangent;\n\
             };\n\
             NormalData getWaveNormal(vec2 horizontalPos)\n\
             {\n\
                NormalData dat;\n\
                dat.normal = vec3(0.0, 0.0, 0.001);\n\
                dat.tangent = vec3(0.001, 0.0, 0.0);\n\
                dat.bitangent = vec3(0.0, 0.001, 0.0);\n\
                \n\
                vec2 epsilon = vec2(0.1);\n\
                \n\
                for (int i = 0; i < " + n + "; ++i)\n\
                {\n\
                    //Get the height at nearby vertices and compute the normal via cross-product.\n\
                    \n\
                    vec2 one_zero = horizontalPos + vec2(epsilon.x, 0.0f),\n\
                         nOne_zero = horizontalPos + vec2(-epsilon.x, 0.0f),\n\
                         zero_one = horizontalPos + vec2(0.0f, epsilon.y),\n\
                         zero_nOne = horizontalPos + vec2(0.0f, -epsilon.y);\n\
                    \n\
                    vec3 p_zero_zero = vec3(horizontalPos, getWaveHeight(horizontalPos));\n\
                    vec3 p_one_zero = vec3(one_zero, getWaveHeight(one_zero)),\n\
                         p_nOne_zero = vec3(nOne_zero, getWaveHeight(nOne_zero)),\n\
                         p_zero_one = vec3(zero_one, getWaveHeight(zero_one)),\n\
                         p_zero_nOne = vec3(zero_nOne, getWaveHeight(zero_nOne));\n\
                    \n\
                    vec3 norm1 = cross(normalize(p_one_zero - p_zero_zero),\n\
                                       normalize(p_zero_one - p_zero_zero)),\n\
                         norm2 = cross(normalize(p_nOne_zero - p_zero_zero),\n\
                                       normalize(p_zero_nOne - p_zero_zero)),\n\
                         normFinal = normalize((norm1 * sign(norm1.z)) + (norm2 * sign(norm2.z)));\n\
                    \n\
                    dat.normal += normFinal;\n\
                    if (false) {\n\
                    //Extract the uniform data.\n\
                    //float dropoffPoint = dropoffPoints_timesSinceCreated_heights_periods[i].x;\n\
                    //float timeSinceCreated = dropoffPoints_timesSinceCreated_heights_periods[i].y;\n\
                    //float height = dropoffPoints_timesSinceCreated_heights_periods[i].z;\n\
                    //float period = dropoffPoints_timesSinceCreated_heights_periods[i].w;\n\
                    //vec2 source = sourcesXY_speeds[i].xy;\n\
                    //float speed = sourcesXY_speeds[i].z;\n\
                    \n\
                    //float dist = distance(source, horizontalPos);\n\
                    //float heightScale = max(0, mix(0.0, 1.0, 1.0 - (dist / dropoffPoint)));\n\
                    //'cutoff' will be either 0 or 1 based on how far away this vertex is.\n\
                    //float cutoff = timeSinceCreated * speed * 3.0;\n\
                    //cutoff = max(0, sign(cutoff - dist));\n\
                    \n\
                    //float innerVal = (dist / period) + (-u_elapsed_seconds * speed);\n\
                    //float waveScale = height * heightScale * cutoff;\n\
                    \n\
                    //float derivative = cos(innerVal);\n\
                    //vec3 toSource = vec3(normalize(source.xy - horizontalPos.xy), 0.001);\n\
                    \n\
                    //norm += height * heightScale * cutoff * normalize(mix(vec3(0.0, 0.0, 1.0), toSource, derivative));\n\
                    }\n\
                }\n\
                dat.normal = normalize(dat.normal);\n\
                return dat;\n\
             }\n";


    return RenderingPass(commonGround + "\
             void main()\n\
             {\n\
                out_tex = in_tex;\n\
                out_col = vec4(in_pos.xy, in_col.zw);\n\
                \n\
                float heightOffset = getWaveHeight(in_pos.xy);\n\
                vec3 finalPos = in_pos + vec3(0.0, 0.0, heightOffset);\n\
                gl_Position = worldTo4DScreen(finalPos);\n\
                \n\
                vec4 out_pos4 = (u_world * vec4(finalPos, 1.0));\n\
                out_pos = out_pos4.xyz / out_pos4.w;\n\
             }",

           commonGround + "\
            struct DirectionalLightStruct\n\
			{\n\
				vec3 Dir, Col;\n\
				float Ambient, Diffuse, Specular;\n\
				float SpecularIntensity;\n\
			};\n\
			uniform DirectionalLightStruct DirectionalLight;\n\
            \n\
            void main()\n\
            {\n\
                //Remap the random seed from [0, 1] to [-1, 1].\n\
                vec2 f2 = -1.0 + (2.0 * out_col.zw);\n\
                \n\
                //Get the normal and the normal map sample.\n\
                vec3 norm = getWaveNormal(out_col.xy).normal;\n\
                \n\
                //TODO: Take this math involving 'out_col.z' and turn it into two uniforms.\n\
                vec2 specialOffset = vec2(f2.x * 0.01 * sin(0.25 * f2.x * u_elapsed_seconds));\n\
                vec3 normalMap = sampleTex(1, out_tex, specialOffset).xyz;\n\
                \n\
                //Remap the normal map so that the horizontal coordinates are in the range [-1, 1] instead of [0, 1].\n\
                normalMap = vec3(-1.0 + (2.0 * normalMap.xy), abs(normalMap.z));\n\
                \n\
                //Combine the normal maps.\n\
                //TODO: Adding the normal map to the wave normal is incorrect. You have to rotate the normal map so it is relative to the wave normal.\n\
                norm = normalize(norm + normalMap);\n\
                \n\
                float brightness = getBrightness(normalize(norm), normalize(out_pos - u_cam_pos),\n\
                                                 DirectionalLight.Dir, DirectionalLight.Ambient,\n\
                                                 DirectionalLight.Diffuse, DirectionalLight.Specular,\n\
                                                 DirectionalLight.SpecularIntensity);\n\
                vec4 texCol = sampleTex(0, out_tex);\n\
                \n\
                out_finalCol = vec4(brightness * DirectionalLight.Col * texCol.xyz, 1.0);\n\
            }");
}
RenderingPass GetDirectionalWaterRenderer(int maxFlows)
{
    return Materials::LitTexture;
}
RenderingPass GetSeededHeightRenderer(void)
{
    std::string commonGround = std::string() + "\
                uniform vec3 amplitude_period_speed;\n\
                uniform vec2 seedMapResolution;\n\
                uniform sampler2D seedMap;\n\
                \n\
                float getWaveHeight(vec2 horizontalPos)\n\
                {\n\
                    float amp = amplitude_period_speed.x,\n\
                          per = amplitude_period_speed.y,\n\
                          spd = amplitude_period_speed.z;\n\
                    \n\
                    vec2 uvLookup = in_tex + (u_elapsed_seconds * seedMapPanDir);\n\
                    \n\
                    float seed = 6346.1634 * sampleTex(2, in_tex).x;]\n\
                    return amp * sin((seed / per) + (u_elapsed_seconds * spd));\n\
                }\n\
                vec3 getWaveNormal(vec2 horizontalPos)\n\
                {\n\
                    //Get the height at nearby vertices and compute the normal via cross-product.\n\
                    vec2 epsilon = 1.0 / seedMapResolution;\n\
                    \n\
                    vec2 one_zero = horizontalPos + vec2(epsilon, 0.0f),\n\
                         nOne_zero = horizontalPos + vec2(-epsilon, 0.0f),\n\
                         zero_one = horizontalPos + vec2(0.0f, epsilon),\n\
                         zero_nOne = horizontalPos + vec2(0.0f, -epsilon);\n\
                    \n\
                    vec3 p_zero_zero = vec3(horizontalPos, getWaveHeight(horizontalPos));\n\
                    vec3 p_one_zero = vec3(one_zero, getWaveHeight(one_zero)),\n\
                         p_nOne_zero = vec3(nOne_zero, getWaveHeight(nOne_zero)),\n\
                         p_zero_one = vec3(zero_one, getWaveHeight(zero_one)),\n\
                         p_zero_nOne = vec3(zero_nOne, getWaveHeight(zero_nOne));\n\
                    \n\
                    //TODO: See if we can remove the outer 'normalize()' here without messing anything up.\n\
                    vec3 norm1 = normalize(cross(normalize(p_one_zero - p_zero_zero),\n\
                                                 normalize(p_zero_one - p_zero_zero))),\n\
                         norm2 = normalize(cross(normalize(p_nOne_zero - p_zero_zero),\n\
                                                 normalize(p_zero_nOne - p_zero_zero))),\n\
                         normFinal = normalize(norm1 + norm2);\n\
                    //Make sure it's positive along the vertical axis.\n\
                    normFinal *= sign(normFinal.z);\n\
                    \n\
                    return normFinal;\n\
                }\n\
                ";

    return RenderingPass(
        commonGround + "\
            void main()\n\
            {\n\
               out_tex = in_tex;\n\
               out_col = vec4(in_pos, 0.0);\n\
               \n\
               float heightOffset = getWaveHeight(in_pos.xy);\n\
               vec3 finalPos = in_pos + vec3(0.0, 0.0, heightOffset);\n\
               gl_Position = worldTo4DScreen(finalPos);\n\
               \n\
               vec4 out_pos4 = (u_world * vec4(finalPos, 1.0));\n\
               out_pos = out_pos4.xyz / out_pos4.w;\n\
            }",
        commonGround + "\
            struct DirectionalLightStruct\n\
			{\n\
				vec3 Dir, Col;\n\
				float Ambient, Diffuse, Specular;\n\
				float SpecularIntensity;\n\
			};\n\
			uniform DirectionalLightStruct DirectionalLight;\n\
            \n\
            uniform vec2 texturePanDir;\n\
            uniform vec2 normalmapTexturePanDir;\n\
            \n\
            void main()\n\
            {\n\
                vec3 norm = getWaveNormal(out_col.xy);\n\
                vec3 normalMap = sampleTex(1, out_tex).xyz;\n\
                norm = normalize(norm + vec3(-1.0 + (2.0 * normalMap.xy), normalMap.z));\n\
                \n\
                float brightness = getBrightness(normalize(norm), normalize(out_pos - u_cam_pos),\n\
                                                 DirectionalLight.Dir, DirectionalLight.Ambient,\n\
                                                 DirectionalLight.Diffuse, DirectionalLight.Specular,\n\
                                                 DirectionalLight.SpecularIntensity);\n\
                vec4 texCol = sampleTex(0, out_tex);\n\
                \n\
                out_finalCol = vec4(brightness * DirectionalLight.Col * texCol.xyz, 1.0);\n\
            }");
}

void Water::Update(float elapsed)
{
    switch (waterType)
    {
    case WaterTypes::Rippling:

        //Keep in mind that negative time values indicate the ripple source was stopped and is disappearing.
        for (int i = 0; i < maxRipples; ++i)
            if (dp_tsc_h_p[i].y >= 0.0f)
                dp_tsc_h_p[i].y += elapsed;
            else dp_tsc_h_p[i].y -= elapsed;

        break;


    case WaterTypes::Directed:

        break;


    case WaterTypes::SeededHeightmap:

        break;

    default: assert(false);
    }
}
bool Water::Render(const RenderInfo & info)
{
    //Set uniforms.
    switch (waterType)
    {
    case WaterTypes::Rippling:
        waterMesh.Uniforms.FloatArrayUniforms["dropoffPoints_timesSinceCreated_heights_periods"].SetData(&(dp_tsc_h_p[0][0]));
        waterMesh.Uniforms.FloatArrayUniforms["sourcesXY_speeds"].SetData(&(sXY_sp[0][0]));
        break;

    case WaterTypes::Directed:

        break;

    case WaterTypes::SeededHeightmap:

        break;

    default: assert(false);
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