#include "DataNodeRenderWorld.h"

#include "../Rendering/Data Nodes/DataNodes.hpp"
#include "../Input/Input.hpp"

#include <iostream>
#include "../Rendering/Primitives/PrimitiveGenerator.h"
#include "../DebugAssist.h"


typedef DataNodeRenderWorld DNRW;


DNRW::DataNodeRenderWorld(void)
    : objMat(0), windowSize(800, 600),

      //Use bilinear filtering for the texture, make it wrap around,
      //    use 8-bit color components, and generate mipmaps.
      objTex(TextureSampleSettings2D(FT_LINEAR, WT_WRAP), PixelSizes::PS_8U, true),

      cam(Vector3f(0.0f, 0.0f, 2.0f),
          10.0f, 0.16f,
          Vector3f(1.0f, 1.0f, -1.0f).Normalized(),
          Vector3f(0.0f, 0.0f, 1.0f)),
      SFMLOpenGLWorld(800, 600)
{

}


sf::ContextSettings DNRW::GenerateContext(void)
{
    return sf::ContextSettings(24, 0, 0, 4, 1);
}


void DNRW::InitializeTextures(void)
{
    //Create the texture, leaving the settings we already gave it in its constructor.
    objTex.Create();

    //Try to load the texture from a file.
    std::string errorMsg;
    if (!Assert(objTex.SetDataFromFile("Content/Sample Worlds/grass.png", errorMsg),
                "Error loading 'Content/Sample Worlds/grass.png'",
                errorMsg))
    {
        return;
    }
}
void DNRW::InitializeMaterials(void)
{
    //The DataNode system allows a user to create modular shaders without having to write shader code.
    //If you have ever used popular material graph editors such as the one in Unreal Engine 4, this is
    //    essentially a lower-level version of that -- instead of setting high-level outputs like "Diffuse",
    //    "Normal", "Roughness", etc., you manually set the actual outputs of the vertex and fragment
    //    shaders.
    //Each "DataNode" instance represents a single atomic operation that takes some number of inputs
    //    and has at least one output.
    //Each input and output is a float vector of size 1 (float), 2 (vec2), 3 (vec3), or 4 (vec4).
    //For example, the "AddNode" takes in any number of inputs of the same size and outputs another
    //    float vector of the same size that represents the component-wise sum of all the inputs.
    //Inputs to Data Nodes are called "DataLines"; a DataLine is either a constant value or
    //    the output of another Data Node.
    //Some DataNodes represent more complex things such as "uniforms", which are shader parameters
    //    that can be changed at run-time. The universal built-in uniforms such as elapsed time
    //    or camera/projection data are outputs of singleton DataNodes like TimeNode and CameraDataNode.
    //There are also two singleton DataNodes that output shader inputs: "VertexInputNode" and "FragmentInputNode".
    //Note that a few nodes (like VertexInputNode and FragmentInputNode) are only applicable in one type of shader.

    //The material is defined by various outputs:
    //  * The vertex shader's position output
    //  * The other vertex shader outputs, which become inputs for the fragment shader
    //  * The fragment shader's color output(s)
    SerializedMaterial matData;

    //We also need to specify the vertex data.
    //In this case, we are using vertices with position, UV, and normal attributes
    //    (although we don't actually use the normal in this shader).
    matData.VertexInputs = VertexPosUVNormal::GetVertexAttributes();

    //The vertex shader will be a simple object-to-screen-space conversion.
    //It also outputs UVs to the fragment shader.

    //Define the vertex inputs for easy access.
    //Each one is an outupt from the singleton "VertexInputNode".
    DataLine vIn_ObjPos(VertexInputNode::GetInstance(), 0),
             vIn_UV(VertexInputNode::GetInstance(), 1),
             vIn_ObjNormal(VertexInputNode::GetInstance(), 2);

    //Compute and output screen-space position using "SpaceConverterNode".
    //We want the second outupt of the node (index 1) for the shader's position output.
    DataNode::Ptr objPosToScreen = SpaceConverterNode::ObjPosToScreenPos(vIn_ObjPos);
    matData.MaterialOuts.VertexPosOutput = DataLine(objPosToScreen, 1);

    //Pass the UVs straight into the fragment shader.
    matData.MaterialOuts.VertexOutputs.push_back(ShaderOutput("vOut_UV", vIn_UV));


    //The fragment shader just outputs a texture.

    //Define the UV input for convenience.
    DataLine fIn_UV(FragmentInputNode::GetInstance(), 0);
    //Sample the texture at the UV coordinate.
    DataNode::Ptr textureSample(new TextureSample2DNode(fIn_UV, "u_texSampler"));
    //Get the RGB values out of the texture.
    DataLine textureRGB(textureSample,
                        TextureSample2DNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    //Combine the texture RGB with an alpha of 1.0.
    DataNode::Ptr finalColor(new CombineVectorNode(textureRGB, 1.0f));
    //Output the color to the screen.
    matData.MaterialOuts.FragmentOutputs.push_back(ShaderOutput("fOut_Color", finalColor));
    

    //Generate the final material.
    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(matData, matParams,
                                                                                BlendMode::GetOpaque());
    if (Assert(genM.ErrorMessage.empty(),
               "Error generating terrain shaders",
               genM.ErrorMessage))
    {
        objMat = genM.Mat;
    }


    //Set up the custom material parameters.
    matParams["u_texSampler"].Tex() = objTex.GetTextureHandle();
}
void DNRW::InitializeObjects(void)
{
    RenderIOAttributes vertexInfo = VertexPosUVNormal::GetVertexAttributes();

    std::vector<VertexPosUVNormal> vertices;
    std::vector<unsigned int> indices;

    //Generate a cube with tiled faces.
    PrimitiveGenerator::GenerateCube(vertices, indices, false, false,
                                     Vector2f(20.0f, 1.0f), Vector2f(20.0f, 1.0f),
                                     Vector2f(20.0f, 20.0f));

    //Set the mesh data to use the cube.
    objMesh.SubMeshes.push_back(MeshData(false, PrimitiveTypes::PT_TRIANGLE_LIST));
    objMesh.SubMeshes[0].SetVertexData(vertices, MeshData::BUF_STATIC, vertexInfo);
    objMesh.SubMeshes[0].SetIndexData(indices, MeshData::BUF_STATIC);

    //Scale up the cube so that it's noticeable.
    objMesh.Transform.SetScale(Vector3f(20.0f, 20.0f, 0.5f));
}

void DNRW::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();

    //If basic initialization failed, just stop here.
    if (IsGameOver())
    {
        return;
    }

    InitializeTextures();
    InitializeMaterials();
    InitializeObjects();

    cam.Window = GetWindow();
    cam.PerspectiveInfo.SetFOVDegrees(55.0f);
    cam.PerspectiveInfo.Width = windowSize.x;
    cam.PerspectiveInfo.Height = windowSize.y;
    cam.PerspectiveInfo.zNear = 0.1f;
    cam.PerspectiveInfo.zFar = 100.0f;
}
void DNRW::OnWorldEnd(void)
{
    delete objMat;
    objMat = 0;

    objTex.DeleteIfValid();
}

void DNRW::UpdateWorld(float elapsedSeconds)
{
    cam.Update(elapsedSeconds);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        EndWorld();
    }
}

void DNRW::RenderWorldGeometry(const RenderInfo& info)
{
    ScreenClearer().ClearScreen();
    objMat->Render(info, &objMesh, matParams);
}
void DNRW::RenderOpenGL(float elapsedSeconds)
{
    //Set up rendering state.
    ScreenClearer().ClearScreen();
    RenderingState(RenderingState::C_BACK).EnableState();
    Viewport(0, 0, windowSize.x, windowSize.y).Use();

    Matrix4f viewM, projM;
    cam.GetViewTransform(viewM);
    cam.GetPerspectiveProjection(projM);

    RenderInfo info(GetTotalElapsedSeconds(), &cam, &viewM, &projM);
    RenderWorldGeometry(info);
}

void DNRW::OnInitializeError(std::string errorMsg)
{
	EndWorld();

	SFMLOpenGLWorld::OnInitializeError(errorMsg);

	std::cout << "Enter any key to continue:\n";
    char dummy;
    std::cin >> dummy;
}
void DNRW::OnWindowResized(unsigned int newW, unsigned int newH)
{
    cam.PerspectiveInfo.Width = newW;
    cam.PerspectiveInfo.Height = newH;

    windowSize.x = newW;
    windowSize.y = newH;
}

bool DNRW::Assert(bool test, std::string errorIntro, const std::string& error)
{
    if (!test)
    {
        std::cout << errorIntro << ": " << error << "\nEnter anything to continue: ";
        char dummy;
        std::cin >> dummy;

        EndWorld();
    }

    return test;
}