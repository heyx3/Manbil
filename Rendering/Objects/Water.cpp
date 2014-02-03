#include "Water.h"

#include "../../OpenGLIncludes.h"
#include "../../RenderDataHandler.h"
#include "../../Math/Higher Math/Terrain.h"

void CreateWaterMesh(unsigned int size, Mesh & outM)
{
    Vector3f offset(size * -0.5f, size * -0.5f, 0.0f);

    //Just create a flat terrain and let it do the math.

    Terrain terr(size);
    Vertex * vertices = new Vertex[terr.GetVerticesCount()];
    Vector3f * poses = new Vector3f[terr.GetVerticesCount()];
    Vector2f * texCoords = new Vector2f[terr.GetVerticesCount()];
    Vector3f * normals = new Vector3f[terr.GetVerticesCount()];
    unsigned int * indices = new unsigned int[terr.GetIndicesCount()];

    terr.CreateVertexPositions(poses);
    terr.CreateVertexNormals(normals, poses, Vector3f());
    terr.CreateVertexTexCoords(texCoords);
    terr.CreateVertexIndices(indices);
    for (int i = 0; i < terr.GetVerticesCount(); ++i)
    {
        vertices[i] = Vertex(poses[i] + offset, texCoords[i], normals[i]);
    }

    RenderObjHandle vbo, ibo;
    RenderDataHandler::CreateVertexBuffer(vbo, vertices, terr.GetVerticesCount(), RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
    RenderDataHandler::CreateIndexBuffer(ibo, indices, terr.GetIndicesCount(), RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
    delete[] vertices, poses, texCoords, normals, indices;

    VertexIndexData vid(terr.GetVerticesCount(), vbo, terr.GetIndicesCount(), ibo);
    outM.SetVertexIndexData(&vid, 1);
}

Water::Water(unsigned int size, unsigned int maxRipples,
             Vector2f texturePanDir, Vector3f pos)
    : currentRippleIndex(0), maxRipples(maxRipples), nextRippleID(0), totalRipples(0),
      Mat(0), waterMesh(PrimitiveTypes::Triangles)
{
    Mat = new Material(GetRippleWaterRenderer(maxRipples));

    if (Mat->HasError())
    {
        errorMsg = "Error creating ripple water material: ";
        errorMsg += Mat->GetErrorMessage();
        return;
    }

    CreateWaterMesh(size, waterMesh);

    RippleWaterArgs args(Vector3f(), 0.0f, 0.0f, 1.0f, 1.0f);
    RippleWaterArgsElement argsE(args, -1);
    for (int i = 0; i < maxRipples; ++i)
    {
        ripples.insert(ripples.end(), argsE);
    }
}
Water::Water(unsigned int size, Vector2f texPanDir, DirectionalWaterArgs mainFlow, unsigned int _maxFlows)
    : currentFlowIndex(0), maxFlows(_maxFlows), nextFlowID(0), totalFlows(0), Mat(0), waterMesh(PrimitiveTypes::Triangles)
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
}

int Water::AddRipple(const RippleWaterArgs & args)
{
    //Prepare to translate the source into object space.
    Matrix4f inv;
    Transform.GetWorldTransform(inv);
    inv = inv.Inverse();
    

    //Put in the new value.

    RippleWaterArgsElement el(args, nextRippleID);
    nextRippleID += 1;

    int index = currentRippleIndex;
    ripples.insert(ripples.begin() + currentRippleIndex, el);
    currentRippleIndex += 1;
    currentRippleIndex %= ripples.size();


    //Apply the transformation now.
    ripples[currentRippleIndex].Args.Source = inv.Apply(ripples[currentRippleIndex].Args.Source);


    return nextRippleID - 1;
}
void Water::ChangeRipple(int element, const RippleWaterArgs & args)
{
    for (int i = 0; i < ripples.size(); ++i)
    {
        if (ripples[i].Element == element)
        {
            //Translate the source into object space.
            Matrix4f inv;
            Transform.GetWorldTransform(inv);
            inv = inv.Inverse();

            ripples[i].Args = args;
            ripples[i].Args.Source = inv.Apply(ripples[i].Args.Source);
            
            return;
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

    return RenderingPass(std::string() +
            "uniform vec2 sources[" + n + "];\n\
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
                float heightOffset = getheightOffset(in_pos.xy);\n\
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
            uniform float bumpmapHeight;\n\
            void main()\n\
            {\n\
                finalPos = out_pos +\n\
                           out_normal * bumpmapHeight * texture(u_sampler0, u_textureScale * in_tex).x);\n\
            }");
}

RenderingPass Water::GetDirectionalWaterRenderer(void)
{
    return Materials::BareColor;
}