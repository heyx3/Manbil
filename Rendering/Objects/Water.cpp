#include "Water.h"

#include "../../OpenGLIncludes.h"
#include "../../RenderDataHandler.h"
#include "../../Math/Higher Math/Terrain.h"

void CreateWaterMesh(unsigned int size, Mesh & outM)
{
    Vector3f offset(size * -0.5f, size * -0.5f, 0.0f);

    //Just create a flat terrain and let it do the math.

    Terrain terr(size);
    int nVs = terr.GetVerticesCount(),
        nIs = terr.GetIndicesCount();
    Vector3f * poses = new Vector3f[nVs];
    Vector2f * texCoords = new Vector2f[nVs];
    Vector3f * normals = new Vector3f[nVs];
    terr.CreateVertexPositions(poses);
    terr.CreateVertexNormals(normals, poses, Vector3f());
    terr.CreateVertexTexCoords(texCoords);

    Vertex * vertices = new Vertex[nVs];
    for (int i = 0; i < nVs; ++i)
    {
        vertices[i] = Vertex(poses[i] + offset, texCoords[i], normals[i]);
    }
    delete[] poses, texCoords, normals;

    unsigned int * indices = new unsigned int[nIs];
    terr.CreateVertexIndices(indices);

    RenderObjHandle vbo, ibo;
    RenderDataHandler::CreateVertexBuffer(vbo, vertices, nVs, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
    RenderDataHandler::CreateIndexBuffer(ibo, indices, nIs, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);

    VertexIndexData vid(terr.GetVerticesCount(), vbo, terr.GetIndicesCount(), ibo);
    outM.SetVertexIndexData(&vid, 1);
    
    delete[] vertices, indices;
}

Water::Water(unsigned int size, unsigned int maxRipples,
             Vector2f texturePanDir, Vector3f pos)
    : currentRippleIndex(0), maxRipples(maxRipples), nextRippleID(0), totalRipples(0),
      Mat(0), waterMesh(PrimitiveTypes::Triangles),
      rippleIDs(0), dp_tsc_h_p(0), sXY_sp(0)
{
    Mat = new Material(GetRippleWaterRenderer(maxRipples));

    if (Mat->HasError())
    {
        errorMsg = "Error creating ripple water material: ";
        errorMsg += Mat->GetErrorMessage();
        return;
    }

    CreateWaterMesh(size, waterMesh);

    Mat->AddUniform("dropoffPoints_timesSinceCreated_heights_periods");
    Mat->AddUniform("sourcesXY_speeds");
    Mat->AddUniform("bumpmapHeight");

    rippleIDs = new int[maxRipples];
    dp_tsc_h_p = new Vector4f[maxRipples];
    sXY_sp = new Vector3f[maxRipples];

    float bumpmapHeight = 10.0f;
    Mat->SetUniformArrayF("dropoffPoints_timesSinceCreated_heights_periods", &(dp_tsc_h_p[0][0]), 4, maxRipples);
    Mat->SetUniformArrayF("sourcesXY_speeds", &(sXY_sp[0][0]), 3, maxRipples);
    Mat->SetUniformF("bumpmapHeight", &bumpmapHeight, 1);
}
Water::Water(unsigned int size, Vector2f texPanDir, DirectionalWaterArgs mainFlow, unsigned int _maxFlows)
    : currentFlowIndex(0), maxFlows(_maxFlows), nextFlowID(0), totalFlows(0),
      Mat(0), waterMesh(PrimitiveTypes::Triangles),
      rippleIDs(0), dp_tsc_h_p(0), sXY_sp(0)
{
    Mat = new Material(GetDirectionalWaterRenderer());

    if (Mat->HasError())
    {
        errorMsg = "Error creating directional water material: ";
        errorMsg += Mat->GetErrorMessage();
        return;
    }

    CreateWaterMesh(size, waterMesh);

    DirectionalWaterArgs args(Vector2f(1.0f, 0.0f), 0.0f);
    DirectionalWaterArgsElement argsE(args, -1);
    for (int i = 0; i < maxFlows; ++i)
    {
        flows.insert(flows.end(), argsE);
    }
}

Water::~Water(void)
{
    delete Mat;

    if (rippleIDs != 0)
    {
        delete[] rippleIDs, dp_tsc_h_p, sXY_sp;
    }
}

int Water::AddRipple(const RippleWaterArgs & args)
{
    //Translate the source into object space.
    Matrix4f inv;
    Transform.GetWorldTransform(inv);
    inv = inv.Inverse();
    Vector3f sourcePos = args.Source;
    sourcePos = inv.Apply(sourcePos);
    

    //Update tracking values.
    int rippleID = nextRippleID;
    nextRippleID += 1;
    int index = currentRippleIndex;
    currentRippleIndex += 1;
    currentRippleIndex %= maxRipples;


    //Set the uniforms.
    dp_tsc_h_p[currentRippleIndex] = Vector4f(args.DropoffPoint, args.TimeSinceCreated, args.Amplitude, args.Period);
    sXY_sp[currentRippleIndex] = Vector3f(sourcePos.x, sourcePos.y, args.Speed);
    rippleIDs[currentRippleIndex] = rippleID;


    return rippleID;
}
void Water::ChangeRipple(int element, const RippleWaterArgs & args)
{
    for (int i = 0; i < maxRipples; ++i)
    {
        if (rippleIDs[i] == element)
        {
            //Translate the source into object space.
            Matrix4f inv;
            Transform.GetWorldTransform(inv);
            inv = inv.Inverse();
            Vector3f sourcePos = args.Source;
            sourcePos = inv.Apply(sourcePos);

            //Set the uniforms.
            dp_tsc_h_p[i] = Vector4f(args.DropoffPoint, args.TimeSinceCreated, args.Amplitude, args.Period);
            sXY_sp[i] = Vector3f(sourcePos.x, sourcePos.y, args.Speed);
        }
    }
}

int Water::AddFlow(const DirectionalWaterArgs & args)
{
    //Translate the source into object space.
    Matrix4f inv;
    Transform.GetWorldTransform(inv);
    inv = inv.Inverse();

    //TODO: Apply the transformation after putting the ripple into the list.

    return -1;
}
void Water::ChangeFlow(int element, const DirectionalWaterArgs & args)
{
    for (int i = 0; i < flows.size(); ++i)
    {
        if (flows[i].Element == element)
        {
            flows[i].Args = args;
            return;
        }
    }
}


RenderingPass Water::GetRippleWaterRenderer(int maxRipples)
{
    std::string n = std::to_string(maxRipples);

    return RenderingPass(std::string() + "\
             //Keep uniforms compacted into vectors so they can be sent to the GPU quicker.\n\
             uniform vec4 dropoffPoints_timesSinceCreated_heights_periods[" + n + "];\n\
             uniform vec3 sourcesXY_speeds[" + n + "];\n\
             \n\
             float getHeightOffset(vec2 horizontalPos)\n\
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
                    float heightScale = mix(0.0, 1.0, 1.0 - (dist / dropoffPoint));\n\
                    //'cutoff' will be either 0 or 1 based on how far away this vertex is.\n\
                    float cutoff = timeSinceCreated * speed;\n\
                    cutoff = max(0, sign(cutoff - dist));\n\
                    offset += height * heightScale * cutoff * sin((dist / period) + (u_elapsed_seconds / speed));\n\
                }\n\
                return offset;\n\
             }\n\
             \n\
             void main()\n\
             {\n\
                float heightOffset = getHeightOffset(in_pos.xy);\n\
                vec3 finalPos = in_pos + vec3(0.0, 0.0, getHeightOffset(in_pos.xy));\n\
                out_tex = in_tex;\n\
                //Change the normal by shifting it towards the player based on the height change.\n\
                vec2 toPlayer = normalize(u_cam_pos.xy - in_pos.xy);\n\
                vec3 normalOffset = vec3(heightOffset * toPlayer, 0.0);\n\
                out_normal = normalize(worldTo3DScreen(vec3(0.0, 0.0, 1.0)) + normalOffset);\n\
                \n\
                gl_Position = worldTo4DScreen(finalPos);\n\
             }",
            std::string() + "\
            struct DirectionalLightStruct\n\
				{\n\
					vec3 Dir, Col;\n\
					float Ambient, Diffuse, Specular;\n\
					float SpecularIntensity;\n\
				};\n\
			uniform DirectionalLightStruct DirectionalLight;\n\
            \n\
            uniform float bumpmapHeight;\n\
            \n\
            void main()\n\
            {\n\
                vec3 finalPos = out_pos +\n\
                               (out_normal * bumpmapHeight * texture(u_sampler1, u_textureScale * out_tex).x);\n\
                //TODO: Compute normals.\n\
                float brightness = getBrightness(out_normal, normalize(out_pos - u_cam_pos),\n\
                                                 DirectionalLight.Dir, DirectionalLight.Ambient,\n\
                                                 DirectionalLight.Diffuse, DirectionalLight.Specular,\n\
                                                 DirectionalLight.SpecularIntensity);\n\
                vec4 texCol = texture(u_sampler0, u_textureScale * out_tex);\n\
                out_finalCol = vec4(brightness * DirectionalLight.Col * texCol.xyz, texCol.w);\n\
            }");
}

RenderingPass Water::GetDirectionalWaterRenderer(void)
{
    return Materials::BareColor;
}

bool Water::Render(const RenderInfo & info)
{
    //Set the data.
    Mat->SetUniformArrayF("dropoffPoints_timesSinceCreated_heights_periods", &(dp_tsc_h_p[0][0]), 4, maxRipples);
    Mat->SetUniformArrayF("sourcesXY_speeds", &(sXY_sp[0][0]), 3, maxRipples);

    //Render.
    std::vector<const Mesh*> meshes;
    meshes.insert(meshes.end(), &waterMesh);
    if (!Mat->Render(info, meshes))
    {
        errorMsg = "Error rendering water: ";
        errorMsg += Mat->GetErrorMessage();
        return false;
    }
    else return true;
}