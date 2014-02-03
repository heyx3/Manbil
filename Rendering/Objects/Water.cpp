#include "Water.h"


Water::Water(unsigned int width, unsigned int height, unsigned int maxRipples,
             Vector2f texturePanDir, Vector3f pos)
    : currentRippleIndex(0), maxRipples(maxRipples), nextRippleID(0), totalRipples(0),
      Mat(0), waterMesh(PrimitiveTypes::Triangles)
{
    Mat = new Material(GetRippleWaterRenderer(maxRipples));

    RippleWaterArgs args(Vector3f(), 0.0f, 0.0f, 1.0f, 1.0f);
    RippleWaterArgsElement argsE(args, -1);
    for (int i = 0; i < maxRipples; ++i)
    {
        ripples.insert(ripples.end(), argsE);
    }
}
Water::Water(Vector2f texPanDir, DirectionalWaterArgs mainFlow, unsigned int maxRipples)
    : currentFlowIndex(0), maxFlows(maxFlows), Mat(0), waterMesh(PrimitiveTypes::Triangles)
{
    Mat = new Material(GetDirectionalWaterRenderer());
}


int Water::AddRipple(const RippleWaterArgs & args)
{
    //Translate the source into object space.
    Matrix4f inv;
    Transform.GetWorldTransform(inv);
    inv = inv.Inverse();
    
    //TODO: Apply the transformation after putting the ripple into the list.

    return -1;
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
                out_normal = normalize(vec3(0.0, 0.0, 1.0) + normalOffset);\n\
                \n\
                gl_Position = worldTo4DScreen(finalPos);\n\
             }",
            std::string() + "\
            uniform float bumpmapHeight;\n\
            void main()\n\
            {\n\
                finalPos = out_pos + vec3(0.0, 0.0, bumpmapHeight * texture(u_sampler0, u_textureScale * in_tex).x);\n\
            }");
}

RenderingPass Water::GetDirectionalWaterRenderer(void)
{
    return Materials::BareColor;
}