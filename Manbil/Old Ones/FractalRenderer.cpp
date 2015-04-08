#include "FractalRenderer.h"

#include "../Rendering/Primitives/DrawingQuad.h"
#include <fstream>


const float FractalRenderer::AppearTime = 3.0f;


FractalRenderer::FractalRenderer(std::string& err)
    : mat(0)
{
    if (!appearSndBuff.loadFromFile("Content/Old Ones/Audio/OldOneAppear.wav"))
    {
        err = "Unable to load 'OldOneAppear.wav'";
        return;
    }
    appearSound.setBuffer(appearSndBuff);
    appearSound.setRelativeToListener(true);
    
        
    RegenerateMaterial(err);
}
FractalRenderer::~FractalRenderer(void)
{
    if (mat != 0)
    {
        delete mat;
    }
}

void FractalRenderer::Update(const OldOneEditableData& data, float frameSeconds, float totalSeconds)
{
    totalTime = totalSeconds;

    //Update first fractal appearance.
    if (totalTime > AppearTime)
    {
        if (!appeared)
        {
            appeared = true;
            appearSound.play();
        }

        SetFractalPower(data.Shape);
    }
    else
    {
        //Animate the fractal power.
        float power;
        const float powStartT = 3.0f,
                    powEndT = 15.0f;
        if (totalSeconds < powStartT)
        {
            power = 1.0f;
        }
        else if (totalSeconds < powEndT)
        {
            power = Mathf::Lerp(1.0f, 10.0f,
                                Mathf::Supersmooth(Mathf::LerpComponent(powStartT, powEndT,
                                                                        totalSeconds)));
        }
        else
        {
            power = 10.0f;
        }
        SetFractalPower(power);
    }


    //Animate the fractal's size.
    SetFractalSize(Mathf::Clamp(GetFractalSize() + (frameSeconds * 0.1f), 1.0f, 6.0f));


    //Set the fractal's various bits of data.
    SetFractalColor(Vector3f::Lerp(Vector3f(0.0f, 1.0f, 0.0f), Vector3f(1.0f, 0.0f, 0.0f),
                                   data.Angriness),
                    Vector3f::Lerp(Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.8f, 0.4f, 0.0f),
                                   data.Angriness));
    SetFractalRoundness(data.Roundness);
}
#include <iostream>
void FractalRenderer::Render(const OldOneEditableData& data, RenderInfo& info)
{
    if (!appeared)
    {
        return;
    }


    //Get a sphere that bounds the fractal.
    //The default fractal spans the unit cube from {0, 0, 0} to {1, 1, 1}.
    Vector3f oldFractalPos = GetFractalPos();
    float fractalRadius = 1.1f * GetFractalSize();

    //Make the fractal shake a bit based on its angriness.
    const float minShake = 0.01f,
                maxShake = 1.0f;
    Vector3f shakePos = Vector3f(((rand() % 100) * 0.02f) - 1.0f,
                                 ((rand() % 100) * 0.02f) - 1.0f,
                                 ((rand() % 100) * 0.02f) - 1.0f).Normalized() *
                        Mathf::Lerp(minShake, maxShake, data.Angriness);
    Vector3f finalPos = oldFractalPos + shakePos;
    SetFractalPos(finalPos);

    //Now position a quad so that it covers the sphere and points towards the player.
    Vector3f fractalToCam = (info.Cam->GetPosition() - finalPos).Normalized();
    TransformObject& trans = DrawingQuad::GetInstance()->GetMesh().Transform;
    trans.SetPosition(finalPos + (fractalToCam * fractalRadius));
    trans.SetScale(fractalRadius);
    trans.SetRotation(Quaternion(TransformObject::Upward(), fractalToCam));
    
    //Set up the render state.
    RenderingState(RenderingState::C_NONE, true, true,
                   RenderingState::AT_GREATER, 0.0f).EnableState();

    //Finally, render the quad.
    std::cout << params.Floats.find("u_oldOne_roundness")->second.Value[0] << "\n";
    DrawingQuad::GetInstance()->Render(info, params, *mat);
    

    //Reset state.
    SetFractalPos(oldFractalPos);
}

void FractalRenderer::RegenerateMaterial(std::string& err)
{
    if (mat != 0)
    {
        delete mat;
        mat = 0;
    }


    //Use a normal text shader, not the DataNode system, to simplify the shader writing.

    //The way this thing is rendered is that a quad is drawn just in front of the fractal in world
    //    space.
    //This quad is positioned to completely cover the space the fractal is visible in.
    //The quad acts as a "window" onto the fractal; rays are cast from the camera through each pixel
    //    on the quad to see what part of the fractal shape is hit.

    //The vertex shader just does a normal object-space-to-screen-space conversion.
    //It outputs the world-space position of the quad vertices to the fragment shader,
    //    which does the ray-marching stuff.
    //The body of the fragment shader is stored in a text file because it is pretty complex as far as
    //    shaders go and it would be a hassle to inline it here.

    RenderIOAttributes vertIns = DrawingQuad::GetVertexInputData();

    std::string vOut = "out vec3 vOut_WorldPos;",
                fIn = "in vec3 vOut_WorldPos;";
    MaterialUsageFlags vUsage;
    vUsage.EnableFlag(MaterialUsageFlags::DNF_USES_WVP_MAT);
    vUsage.EnableFlag(MaterialUsageFlags::DNF_USES_WORLD_MAT);
    std::string vHeader = MaterialConstants::GetVertexHeader(vOut, vertIns, vUsage);

    std::string wvpMat = MaterialConstants::WVPMatName,
                worldMat = MaterialConstants::WorldMatName;
    std::string vShader = vHeader + "\n\n" +
"void main() \n\
{ \n\
    vec4 worldPos4 = " + worldMat + " * vec4(" + vertIns.GetAttribute(0).Name + ", 1.0); \n\
    vOut_WorldPos = worldPos4.xyz / worldPos4.w; \n\
    gl_Position = " + wvpMat + " * vec4(" + vertIns.GetAttribute(0).Name + ", 1.0); \n\
}";
    
    //Set up the fragment shader header.

    MaterialUsageFlags fUsage;
    fUsage.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_POS);
    fUsage.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_FORWARD);
    fUsage.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_UPWARDS);
    fUsage.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_SIDEWAYS);
    std::string fHeader = MaterialConstants::GetFragmentHeader(fIn, "out vec4 fOut_Final;", fUsage);
    fHeader +=
"\nuniform vec3 u_oldOne_pos;\n\
uniform float u_oldOne_size;\n\
uniform float u_oldOne_power;\n\
uniform vec3 u_oldOne_color1;\n\
uniform vec3 u_oldOne_color2;\n\
uniform float u_oldOne_roundness;\n\n";

    
    //Read in the main fragment shader body.

    std::ifstream inFrag("Content/Old Ones/Old One Frag Shader Body.txt", std::ios::in);
    if (!inFrag.is_open() || inFrag.bad() || inFrag.fail())
    {
        err = "Couldn't open 'Content/Old Ones/Old One Frag Shader Body.txt'";
        return;
    }
    std::string fragShaderBody;
    while (!inFrag.eof())
    {
        std::string temp;
        std::getline(inFrag, temp);
        fragShaderBody += temp;
        fragShaderBody += '\n';
    }
    inFrag.close();

    std::string fShader = fHeader + fragShaderBody;


    //Compile the material.
    params.Floats["u_oldOne_pos"].Name = "u_oldOne_pos";
    params.Floats["u_oldOne_size"].Name = "u_oldOne_size";
    params.Floats["u_oldOne_power"].Name = "u_oldOne_power";
    params.Floats["u_oldOne_color1"].Name = "u_oldOne_color1";
    params.Floats["u_oldOne_color2"].Name = "u_oldOne_color2";
    params.Floats["u_oldOne_roundness"].Name = "u_oldOne_roundness";
    SetFractalPos(Vector3f(150.278f, 10.134f, 10.772f));
    SetFractalSize(3.0f);
    SetFractalPower(1.0f);
    SetFractalRoundness(1.0f);
    SetFractalColor(Vector3f(), Vector3f());
    mat = new Material(vShader, fShader, params, vertIns, BlendMode::GetOpaque(), err);
    if (!err.empty())
    {
        err = "Error creating material: " + err;
        return;
    }
}

Vector3f FractalRenderer::GetFractalPos(void) const
{
    return *(Vector3f*)&params.Floats.find("u_oldOne_pos")->second.Value;
}
float FractalRenderer::GetFractalSize(void) const
{
    return params.Floats.find("u_oldOne_size")->second.Value[0];
}
float FractalRenderer::GetFractalPower(void) const
{
    return params.Floats.find("u_oldOne_power")->second.Value[0];
}

void FractalRenderer::SetFractalPos(Vector3f newPos)
{
    params.Floats["u_oldOne_pos"].SetValue(newPos);
}
void FractalRenderer::SetFractalSize(float newSize)
{
    params.Floats["u_oldOne_size"].SetValue(newSize);
}
void FractalRenderer::SetFractalPower(float newValue)
{
    params.Floats["u_oldOne_power"].SetValue(newValue);
}
void FractalRenderer::SetFractalRoundness(float newValue)
{
    params.Floats["u_oldOne_roundness"].SetValue(newValue);
}
void FractalRenderer::SetFractalColor(Vector3f col1, Vector3f col2)
{
    params.Floats["u_oldOne_color1"].SetValue(col1);
    params.Floats["u_oldOne_color2"].SetValue(col2);
}