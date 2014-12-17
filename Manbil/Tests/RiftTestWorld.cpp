#include "RiftTestWorld.h"


#include "../Rendering/PrimitiveGenerator.h"
#include "../DebugAssist.h"



RiftTestWorld::RiftTestWorld(void)
    : objectMat(0), quad(0), cube(TriangleList),
      floorTex(TextureSampleSettings2D(FT_LINEAR, WT_WRAP), PixelSizes::PS_8U, true),
      obj1Tex(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PixelSizes::PS_8U, true),
      obj2Tex(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PixelSizes::PS_8U, true),
      lightCol(1.0f, 1.0f, 1.0f), lightDir(Vector3f(-1.0f, -1.0f, -1.0f).Normalized()),
      ambientLight(0.35f), diffuseLight(0.65f), specLight(1.0f), specIntensity(256.0f),
      baseCam(Vector3f(1.0f, 1.0f, 5.0f), 2.0f, 0.06f,
              (-Vector3f(1.0f, 1.0f, 5.0f)).Normalized(), Vector3f(0.0f, 0.0f, 1.0f),
              true),
      windowSize(800, 600),
      floorT(Vector3f(0.0f, 0.0f, -1.0f), Vector3f(), Vector3f(10.0f, 10.0f, 1.0f)),
      obj1T(Vector3f(3.0f, 3.0f, 1.5f), Vector3f(), Vector3f(0.25f, 0.25f, 0.25f)),
      obj2T(Vector3f(-2.0f, -2.0f, 1.5f), Vector3f(), Vector3f(0.25f, 0.25f, 0.25f)),
      SFMLOpenGLWorld(800, 600, sf::ContextSettings(24, 0, 0, 4, 1))
{

}
RiftTestWorld::~RiftTestWorld(void)
{

}

void RiftTestWorld::InitializeTextures(void)
{
    floorTex.Create();
    obj1Tex.Create();
    obj2Tex.Create();

    std::string err;
    if (!Assert(floorTex.SetDataFromFile("Content/Textures/Grass.png", err),
                "Error loading floor tex 'Grass.png'", err))
    {
        return;
    }
    if (!Assert(obj1Tex.SetDataFromFile("Content/Textures/VoxelTex.png", err),
                "Error loading obj1 tex 'VoxelTex.png'", err))
    {
        return;
    }
    if (!Assert(obj2Tex.SetDataFromFile("Content/Textures/Sky.png", err),
                "Error loading obj2 tex 'Sky.png'", err))
    {
        return;
    }
}
void RiftTestWorld::InitializeMaterials(void)
{
    #pragma region Quad mat

    std::string vertShader = "\
#version 400            \n\
\n\
layout (location = 0) in vec3 in_pos;            \n\
layout (location = 1) in vec2 in_uv;            \n\
layout (location = 2) in vec3 in_normal;            \n\
\n\
out vec2 out_uv;            \n\
\n\
void main()            \n\
{            \n\
    gl_Position = vec4(in_pos, 1.0f);        \n\
    out_uv = in_uv;        \n\
}";
    std::string fragShader = "\
#version 400        \n\
\n\
in vec2 out_uv;        \n\
\n\
out vec4 finalColor4;        \n\
\n\
uniform sampler2D u_tex;        \n\
\n\
void main()        \n\
{        \n\
    finalColor4 = texture2D(u_tex, out_uv);    \n\
}";

    #pragma endregion
    

    #pragma region Object mat

    //Create shaders.

    vertShader = "\
#version 400            \n\
\n\
layout (location = 0) in vec3 in_pos;            \n\
layout (location = 1) in vec2 in_uv;            \n\
layout (location = 2) in vec3 in_normal;            \n\
\n\
out vec2 out_uv;            \n\
out vec3 out_worldPos,            \n\
         out_worldNormal;            \n\
\n\
uniform mat4 " + MaterialConstants::WVPMatName + ";            \n\
uniform mat4 " + MaterialConstants::WorldMatName + ";            \n\
\n\
void main()            \n\
{            \n\
    vec4 screenPos4 = " + MaterialConstants::WVPMatName + " * vec4(in_pos, 1.0f);            \n\
    vec4 worldPos4 = " + MaterialConstants::WorldMatName + " * vec4(in_pos, 1.0f);            \n\
\n\
    out_worldPos = worldPos4.xyz / worldPos4.w;            \n\
    out_worldNormal = normalize((" + MaterialConstants::WorldMatName + " * vec4(in_normal, 0.0f)).xyz);            \n\
    out_uv = in_uv;         \n\
\n\
    gl_Position = screenPos4;            \n\
}";

    fragShader = "\
#version 400            \n\
\n\
in vec2 out_uv;            \n\
in vec3 out_worldPos, out_worldNormal;            \n\
\n\
out vec4 finalOutColor;            \n\
\n\
uniform sampler2D u_tex;            \n\
uniform float u_ambientLight, u_diffuseLight, u_specularLight;            \n\
uniform float u_specularIntensity;            \n\
uniform vec3 u_lightCol, u_lightDir;            \n\
uniform float u_texScale;                   \n\
uniform vec3 " + MaterialConstants::CameraPosName + ";            \n\
\n\
void main()            \n\
{            \n\
    vec3 texSample = texture2D(u_tex, out_uv * u_texScale).xyz;            \n\
\n\
    vec3 worldNormal = normalize(out_worldNormal);            \n\
    float lightDot = max(dot(worldNormal, -u_lightDir), 0.0f);            \n\
    vec3 reflectedLight = normalize(reflect(u_lightDir, worldNormal));            \n\
    float specFactor = max(0.0f, dot(reflectedLight, normalize(" + MaterialConstants::CameraPosName + " - out_worldPos)));            \n\
    specFactor = pow(specFactor, u_specularIntensity);            \n\
    vec3 finalLight = u_lightCol * (u_ambientLight + (u_diffuseLight * lightDot) + (u_specularLight * specFactor));            \n\
\n\
    finalOutColor = vec4(finalLight * texSample, 1.0f);            \n\
}";


    //Initialize parameters.
    UniformDictionary params;
    params.FloatUniforms["u_ambientLight"] = UniformValueF(ambientLight, "u_ambientLight");
    params.FloatUniforms["u_diffuseLight"] = UniformValueF(diffuseLight, "u_diffuseLight");
    params.FloatUniforms["u_specularLight"] = UniformValueF(specLight, "u_specularLight");
    params.FloatUniforms["u_specularIntensity"] = UniformValueF(specIntensity, "u_specularIntensity");
    params.FloatUniforms["u_lightCol"] = UniformValueF(lightCol, "u_lightCol");
    params.FloatUniforms["u_lightDir"] = UniformValueF(lightDir, "u_lightDir");
    params.FloatUniforms["u_texScale"] = UniformValueF(1.0f, "u_texScale");
    params.Texture2DUniforms["u_tex"] = UniformSampler2DValue(0, "u_tex");

    //Create the material.
    objectMat = new Material(vertShader, fragShader, params,
                             VertexPosTex1Normal::GetAttributeData(), RenderingModes::RM_Opaque);
    if (!Assert(objectMat->GetErrorMsg().empty(),
                "Error creating object material", objectMat->GetErrorMsg()))
    {
        return;
    }

    floorParams = params;
    floorParams.Texture2DUniforms["u_tex"].Texture = floorTex.GetTextureHandle();
    floorParams.FloatUniforms["u_texScale"].SetValue(4.0f);
    obj1Params = params;
    obj1Params.Texture2DUniforms["u_tex"].Texture = obj1Tex.GetTextureHandle();
    obj2Params = params;
    obj2Params.Texture2DUniforms["u_tex"].Texture = obj2Tex.GetTextureHandle();

    #pragma endregion
}
void RiftTestWorld::InitializeObjects(void)
{
    std::vector<VertexPosTex1Normal> verts;
    std::vector<unsigned int> indices;
    PrimitiveGenerator::GenerateCube(verts, indices, false, false);

    RenderObjHandle vbo, ibo;
    RenderDataHandler::CreateVertexBuffer(vbo, verts.data(), verts.size(),
                                          RenderDataHandler::UPDATE_ONCE_AND_DRAW);
    RenderDataHandler::CreateIndexBuffer(ibo, indices.data(), indices.size(),
                                         RenderDataHandler::UPDATE_ONCE_AND_DRAW);
    VertexIndexData cubeDat(verts.size(), vbo, indices.size(), ibo);
    cube.SetVertexIndexData(cubeDat);


    quad = new DrawingQuad();
}

void RiftTestWorld::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();
    if (IsGameOver()) return;

    std::string err = InitializeStaticSystems(true, false, true);
    if (!Assert(err.empty(), "Error initializing systems", err))
        return;

    GetWindow()->setVerticalSyncEnabled(false);

    InitializeTextures();
    InitializeMaterials();
    InitializeObjects();

    baseCam.Window = GetWindow();
    baseCam.Info.zNear = 0.1f;
    baseCam.Info.zFar = 1000.0f;
    baseCam.Info.Width = windowSize.x;
    baseCam.Info.Height = windowSize.y;
    baseCam.Info.SetFOVDegrees(55.0f);

    //Set up OVR HMD.
    if (ovrHmd_Detect() > 0)
    {
        std::cout << "Rift HMD was found!\n";
        hmd = ovrHmd_Create(0);

        std::cout << "Manufacturer: " << hmd->Manufacturer << "\n" <<
                     "Serial number: " << hmd->SerialNumber << "\n" <<
                     "Display device name: " << hmd->DisplayDeviceName << "\n";

        if (Assert(ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation |
                                                 ovrTrackingCap_MagYawCorrection |
                                                 ovrTrackingCap_Position, 0),
                   "Error configuring Rift headset for tracking", ovrHmd_GetLastError(hmd)))
        {

        }

        std::cout << "\n";
    }
    else
    {
        hmd = ovrHmd_CreateDebug(ovrHmdType::ovrHmd_DK2);
        std::cout << "No Rift HMD was found.\n\n";
    }

    windowSize = Vector2u((unsigned int)hmd->Resolution.w, (unsigned int)hmd->Resolution.h);
    GetWindow()->setSize(sf::Vector2u(windowSize.x, windowSize.y));


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowSize.x, windowSize.y);
}
void RiftTestWorld::OnWorldEnd(void)
{
    DeleteAndSetToNull(objectMat);
    DeleteAndSetToNull(quad);

    floorTex.DeleteIfValid();
    obj1Tex.DeleteIfValid();
    obj2Tex.DeleteIfValid();


    ovrHmd_Destroy(hmd);


    DestroyStaticSystems(true, false, true);
}

void RiftTestWorld::UpdateWorld(float elapsedSeconds)
{
    baseCam.Update(elapsedSeconds);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        EndWorld();


    //Update each param list.
    for (int i = 0; i < 3; ++i)
    {
        UniformDictionary& d = (i == 0 ? floorParams : (i == 1 ? obj1Params : obj2Params));
        d.FloatUniforms["u_ambientLight"].SetValue(ambientLight);
        d.FloatUniforms["u_diffuseLight"].SetValue(diffuseLight);
        d.FloatUniforms["u_specularLight"].SetValue(specLight);
        d.FloatUniforms["u_specularIntensity"].SetValue(specIntensity);
        d.FloatUniforms["u_lightCol"].SetValue(lightCol);
        d.FloatUniforms["u_lightDir"].SetValue(lightDir);
    }
}

void RiftTestWorld::RenderWorldGeometry(const RenderInfo& info)
{
    ScreenClearer(true, true, false, Vector4f(0.2f, 0.2f, 0.2f, 0.0f)).ClearScreen();
    RenderingState(true, true, RenderingState::C_NONE).EnableState();

    std::vector<const Mesh*> toRender;
    toRender.insert(toRender.end(), &cube);

    //Floor.
    cube.Transform = floorT;
    if (!Assert(objectMat->Render(info, toRender, floorParams),
                "Error rendering floor", objectMat->GetErrorMsg()))
    {
        return;
    }
    //Object 1.
    cube.Transform = obj1T;
    if (!Assert(objectMat->Render(info, toRender, obj1Params),
                "Error rendering obj1", objectMat->GetErrorMsg()))
    {
        return;
    }
    //Object 2.
    cube.Transform = obj2T;
    if (!Assert(objectMat->Render(info, toRender, obj2Params),
                "Error rendering obj2", objectMat->GetErrorMsg()))
    {
        return;
    }
}
void RiftTestWorld::RenderOpenGL(float elapsedSeconds)
{
    Matrix4f worldM, viewM, projM;
    worldM.SetAsIdentity();
    baseCam.GetViewTransform(viewM);
    baseCam.GetPerspectiveTransform(projM);

    
    //Update OVR/camera state.

    Vector3f forward = baseCam.GetForward(),
             up = baseCam.GetUpward();

    hmdState = ovrHmd_GetTrackingState(hmd, ovr_GetTimeInSeconds());
    Quaternion hmdRot(hmdState.HeadPose.ThePose.Orientation.x,
                      hmdState.HeadPose.ThePose.Orientation.y,
                      hmdState.HeadPose.ThePose.Orientation.z,
                      hmdState.HeadPose.ThePose.Orientation.w);
    baseCam.Rotate(hmdRot);

    //Render.
    TransformObject dummy;
    RenderInfo info(this, (Camera*)&baseCam, &dummy, &worldM, &viewM, &projM);

    glViewport(0, 0, windowSize.x, windowSize.y);
    RenderWorldGeometry(info);

    std::string generalError = GetCurrentRenderingError();
    if (!Assert(generalError.empty(), "General rendering error", generalError))
        return;


    //Revert camera to pre-OVR rotation.
    baseCam.SetRotation(forward, up, true);
}

void RiftTestWorld::OnWindowResized(unsigned int newW, unsigned int newH)
{
    glViewport(0, 0, newW, newH);

    windowSize.x = newW;
    windowSize.y = newH;

    baseCam.Info.Width = (float)newW;
    baseCam.Info.Height = (float)newH;
}
void RiftTestWorld::OnInitializeError(std::string errorMsg)
{
    EndWorld();

    SFMLOpenGLWorld::OnInitializeError(errorMsg);

    std::cout << "Enter any key to continue: ";
    char dummy;
    std::cin >> dummy;
}

bool RiftTestWorld::Assert(bool val, std::string errorIntro, const std::string& error)
{
    if (!val)
    {
        std::cout << errorIntro << ": " << error << "\n";

        std::cout << "Enter anything to continue: ";
        char dummy;
        std::cin >> dummy;

        EndWorld();
    }
    return val;
}