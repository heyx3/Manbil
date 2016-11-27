#include "SimpleRenderWorld.h"

#include <iostream>
#include "../Rendering/Primitives/PrimitiveGenerator.h"


const int startWidth = 800,
          startHeight = 600;

SimpleRenderWorld::SimpleRenderWorld(void)
    : windowSize(startWidth, startHeight),
      SFMLOpenGLWorld(startWidth, startHeight)
{

}

sf::VideoMode SimpleRenderWorld::GetModeToUse(unsigned int windowW, unsigned int windowH)
{
    //Change this return value to change the window resolution mode.
    //To use native fullscreen, return "sf::VideoMode::getFullscreenModes()[0];".
    return sf::VideoMode(windowW, windowH);
}
std::string SimpleRenderWorld::GetWindowTitle(void)
{
    //Change this to change the string on the window's title-bar
    //    (assuming it has a title-bar).
    return "SimpleRenderWorld";
}
sf::Uint32 SimpleRenderWorld::GetSFStyleFlags(void)
{
    //Change this to change the properties of the window.
    return sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close;
}
sf::ContextSettings SimpleRenderWorld::GenerateContext(void)
{
    return sf::ContextSettings(24, 0, 0, 4, 1);
}

void SimpleRenderWorld::SetUpMaterial(void)
{
    //There are two shaders in a simple material:
    //The vertex shader takes mesh vertices and translates them into "screen space",
    //    which is a rectangular region of space from {-1, -1, -1} to {1, 1, 1}.
    //Anything outside that space is not visible on the screen.
    //The fragment shader outputs the final color of the mesh's surface.
    //Data can be output from the vertex shader into the fragment shader
    //    to assist in coloring the surface.

    //This material will have a simple vertex shader that uses the built-in
    //    "World-View-Projection" matrix to convert the vertex data into screen space.
    //It will also use the built-in "World" matrix to convert the vertex data into world space,
    //    and output those world-space positions to the fragment shader.
    //The fragment shader will color the surface based on its position in world space.

    //This is an example of a low-level way to write shaders
    //    with the assistance of the "MaterialConstants" class.

    std::string vertexShader, fragmentShader;

    //The vertex inputs have positions, UVs, and normals, though we only need position for this material.
    RenderIOAttributes vertexInputs = VertexPosUVNormal::GetVertexAttributes();

    //First, generate the vertex shader header stuff.
    {
        //Define which built-in parameters are needed.
        MaterialUsageFlags usedParams;
        usedParams.EnableFlag(MaterialUsageFlags::DNF_USES_WVP_MAT);
        usedParams.EnableFlag(MaterialUsageFlags::DNF_USES_WORLD_MAT);

        //There is only one extra output from the vertex shader -- the world-space position.
        std::string vertShaderOutputs = "out vec3 vOut_WorldPos;";
    
        vertexShader = MaterialConstants::GetVertexHeader(vertShaderOutputs, vertexInputs,
                                                          usedParams);
    }

    //Now add the actual shader behavior.
    //One neat little feature of C++ is multi-line strings with the use of "\n\".
    std::string wvpMat = MaterialConstants::WVPMatName,
                worldMat = MaterialConstants::WorldMatName;
    std::string vertPosInput = vertexInputs.GetAttribute(0).Name;
    vertexShader += "\n\
void main() \n\
{ \n\
    //Output the screen-space position. Note that order of operations \n\
    //    is very important (matrix * vector). \n\
    gl_Position = " + wvpMat + " * vec4(" + vertPosInput + ", 1.0); \n\
    //Output the world-space position for the fragment shader. \n\
    vec4 worldPos4 = " + worldMat + " * vec4(" + vertPosInput + ", 1.0); \n\
    vOut_WorldPos = worldPos4.xyz / worldPos4.w; \n\
}";

    //The fragment shader is even simpler; it just takes in the world position
    //    then outputs that position as the color.
    fragmentShader = MaterialConstants::GetFragmentHeader("in vec3 vOut_WorldPos;",
                                                          "out vec4 fOut_Color;",
                                                          MaterialUsageFlags());
    fragmentShader += "\n\
void main() \n\
{ \n\
    //Use the world-space coordinates (which we've set up to be between {0, 0, 0} and {1, 1, 1}) \n\
    //    as the surface color. \n\
    fOut_Color = vec4(vOut_WorldPos, 1.0); \n\
}";


    //Now that the shaders are set up, create the actual material.
    //The material's constructor takes in an error message that gets set if something went wrong.
    std::string errorMsg;
    objMat.reset(new Material(vertexShader, fragmentShader, objMatParams,
					          vertexInputs, BlendMode::GetOpaque(), errorMsg));
    if (!errorMsg.empty())
    {
        std::cout << "Error setting up material: " << errorMsg << "\n\nEnter anything to end the world.";
        char dummy;
        std::cin >> dummy;

        EndWorld();
    }
}
void SimpleRenderWorld::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();

    //If there was an error initializing the game, don’t bother with the rest of initialization.
    if (IsGameOver())
    {
        return;
    }


    //Set up the mesh to render.
    std::vector<VertexPosUVNormal> vertices;
    std::vector<unsigned int> indices;
    //The below function call generates a cube from {-1, -1, -1} to {1, 1, 1}.
    PrimitiveGenerator::GenerateCube(vertices, indices, false, false);
    //Put the generated cube data into the mesh.
	objMesh.reset(new Mesh(false, PrimitiveTypes::PT_TRIANGLE_LIST));
	objMesh->SetVertexData(vertices, Mesh::BUF_STATIC,
                           VertexPosUVNormal::GetVertexAttributes());
	objMesh->SetIndexData(indices, Mesh::BUF_STATIC);

    //Move/size the object so that its vertices stay within about the range [0, 1].
	objTr.SetScale(0.4f);
	objTr.SetPosition(Vector3f(0.5f, 0.5f, 0.5f));

    SetUpMaterial();

    //Now set up the game camera.
    gameCam = MovingCamera(Vector3f(-1.0f, -1.0f, 1.0f),             //Position
                           4.0f, 0.2,                                //Move/look speed
                           Vector3f(1.0f, 1.0f, -1.0f).Normalized(), //'Forward' vector
                           Vector3f(0.0f, 0.0f, 1.0f));              //'Upward' vector
    gameCam.Window = GetWindow();
    gameCam.PerspectiveInfo.SetFOVDegrees(60.0f);
    gameCam.PerspectiveInfo.Width = windowSize.x;
    gameCam.PerspectiveInfo.Height = windowSize.y;
    gameCam.PerspectiveInfo.zNear = 0.1f;
    gameCam.PerspectiveInfo.zFar = 100.0f;
}

void SimpleRenderWorld::OnWorldEnd(void)
{
	objMat.reset();
	objMesh.reset();
}

void SimpleRenderWorld::UpdateWorld(float elapsedSeconds)
{
    gameCam.Update(elapsedSeconds);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        EndWorld();
    }


    //Slowly rotate the mesh.
    objTr.Rotate(Quaternion(Vector3f(0.0f, 0.0f, 1.0f),
                            elapsedSeconds * 1.2f));
}
void SimpleRenderWorld::RenderOpenGL(float elapsedSeconds)
{
    //Set up rendering state.
    //Modify these constructors to change various aspects of how rendering is done.
    ScreenClearer(true, true, false, Vector4f(0.2, 0.2, 0.2f, 0.0f)).ClearScreen();
    RenderingState(RenderingState::C_BACK).EnableState();
    Viewport(0, 0, windowSize.x, windowSize.y).Use();

    //Set up the data structure for rendering objects from our camera.
    Matrix4f viewM, projM;
    gameCam.GetViewTransform(viewM);
    gameCam.GetPerspectiveProjection(projM);
    RenderInfo cameraInfo(GetTotalElapsedSeconds(), &gameCam, &viewM, &projM);
    
    //Render the mesh with the material.
    objMat->Render(*objMesh, objTr, cameraInfo, objMatParams);
}

void SimpleRenderWorld::OnInitializeError(std::string errorMsg)
{
    //Print the error message and end the game.
    SFMLOpenGLWorld::OnInitializeError(errorMsg);
    EndWorld();
}
void SimpleRenderWorld::OnWindowResized(unsigned int newWidth, unsigned int newHeight)
{
    windowSize.x = newWidth;
    windowSize.y = newHeight;

    gameCam.PerspectiveInfo.Width = windowSize.x;
    gameCam.PerspectiveInfo.Height = windowSize.y;
}