#include "TwoTrianglesWorld.h"

#include "TextureSettings.h"
#include "RenderingState.h"
#include "ScreenClearer.h"
#include "Rendering/Texture Management/TextureManager.h"
#include "Input/Input Objects/KeyboardBoolInput.h"
#include "MovingCamera.h"
#include <iostream>
#include <fstream>


//Debug printing stuff.
namespace TTWPrints
{
	void Pause(void) { std::cout << "Enter any character to continue.\n"; std::string dummy; std::getline(std::cin, dummy); }
	void PrintData(std::string datIntro, std::string dat, bool useLineBreak = true)
	{
		std::cout << datIntro << ": " << dat;
		if (useLineBreak) std::cout << "\n";
	}
	bool PrintRenderError(const char * errorIntro)
	{
		const char * error = GetCurrentRenderingError();
		if (strcmp(error, "") != 0)
		{
			PrintData(errorIntro, error);
			ClearAllRenderingErrors();
			Pause();
			return false;
		}

		return true;
	}
}
using namespace TTWPrints;


typedef TwoTrianglesWorld TTW;

#pragma region Material and quad


Material * mat = 0;
DrawingQuad * quad = 0;
std::string shaderPath = "";

void CreateQuad(void)
{
    if (quad != 0) delete quad;
    quad = new DrawingQuad();
    quad->SetSize(Vector2f(1.0f, -1.0f));
}
bool CreateMaterial(const std::string & vs, const std::string & fs, UniformDictionary & uniforms)
{
    if (mat != 0) delete mat;
    mat = new Material(vs, fs, uniforms, RenderingModes::RM_Opaque, false, LightSettings(false));

    return !mat->HasError();
}

//Continuously asks the user for a valid shader and tries to create the material.
//Exits once a valid material is created.
void GetCreateMaterial(bool askForFile = true)
{
    std::string vs, fs;
    UniformDictionary uniforms;
    bool valid = false;
    typedef MaterialConstants MC;

    //The vertex shader is a very simple, constant program.
    MaterialUsageFlags vertFlags;
    vs = MC::GetVertexHeader("out vec2 in_UV;\n", vertFlags) + "\n\n\
void main()                              \n\
{                                        \n\
    gl_Position = vec4(" + MC::InObjPos + ", 1.0);  \n\
    in_UV = " + MC::InUV + ";\n\
}";
    //Add any custom QuadWorld uniforms.
    uniforms.FloatUniforms[TTW::ShaderElapsedName] = UniformValueF(0.0f, TTW::ShaderElapsedName);
    uniforms.TextureUniforms[TTW::CustomSamplerName] = UniformSamplerValue(TTW::CustomSamplerName);
    uniforms.TextureUniforms[TTW::NoiseSamplerName] = UniformSamplerValue(TTW::NoiseSamplerName);

    bool first = true;

    while (!valid)
    {
        //Ask for a shader file path if it is needed.
        if (!first || askForFile)
        {
            valid = false;
            std::cout << "Enter a shader path relative to this program's .exe:\n";

            shaderPath = "";
            std::getline(std::cin, shaderPath);
            if (shaderPath.find(".txt") == std::string::npos)
                shaderPath += ".txt";
        }
        first = false;

        //Try opening the file.
        std::ifstream reader;
        reader.open(shaderPath);
        if (!reader.is_open())
        {
            PrintData("Error loading shader file '" + shaderPath + "'", "file not found");
            std::cout << "\n\n";
            Pause();
            std::cout << "\n\n\n\n\n\n";
            continue;
        }

        //Read in the shader. When setting the usage flags, just assume all uniforms are being used.
        MaterialUsageFlags flags;
        typedef MaterialUsageFlags::Flags FL;
        flags.EnableFlag(FL::DNF_USES_TIME);
        flags.EnableFlag(FL::DNF_USES_CAM_POS);
        flags.EnableFlag(FL::DNF_USES_CAM_FORWARD);
        flags.EnableFlag(FL::DNF_USES_CAM_UPWARDS);
        flags.EnableFlag(FL::DNF_USES_CAM_SIDEWAYS);
        flags.EnableFlag(FL::DNF_USES_WIDTH);
        flags.EnableFlag(FL::DNF_USES_HEIGHT);
        fs = MC::GetFragmentHeader("in vec2 in_UV;\n", "out vec4 " + MC::FinalOutColor + ";\n", flags);
        fs += "\n\
uniform float " + TTW::ShaderElapsedName + ";   \n\
uniform sampler2D " + TTW::CustomSamplerName + ";\n\
uniform sampler2D " + TTW::NoiseSamplerName + ";\n\n\n";
        std::string line = "";
        while (std::getline(reader, line))
            fs += line + "\n";
        reader.close();

        //Try creating the material.
        if (!CreateMaterial(vs, fs, uniforms))
        {
            PrintData("Error compiling material", mat->GetErrorMsg());
            std::cout << "\n\n";
            Pause();
            std::cout << "\n\n\n\n\n\n";
            continue;
        }

        quad->GetMesh().Uniforms.AddUniforms(uniforms, true);
        valid = true;
    }
}

//Gets whether or not "mat" uses the custom user texture.
bool DoesMatUseCustomTex(void)
{
    const std::vector<UniformList::Uniform> & uniforms = mat->GetUniforms(RenderPasses::BaseComponents).TextureUniforms;
    auto location = std::find_if(uniforms.begin(), uniforms.end(),
                                 [](const UniformList::Uniform & unf)
                                 {
                                     return unf.Name == TTW::CustomSamplerName;
                                 });
    return location != uniforms.end();
}


#pragma endregion


#pragma region Textures


TextureManager * tManager;
unsigned int customTexID = TextureManager::UNUSED_ID,
             noiseTexID = TextureManager::UNUSED_ID;
std::string customTexPath = "";

//Gets the noise texture as well as a custom texture from the user if necessary.
//Puts all loaded textures into the correct uniform values for the quad.
void LoadTextures(bool getUserTex, bool askUserTexPath = true)
{
    //Get the noise texture.

    noiseTexID = tManager->CreateTexture("Content/Textures/NoiseTex.png");
    if (noiseTexID == TextureManager::UNUSED_ID)
    {
        PrintData("Error loading 'Content/Textures/NoiseTex.png'", "SFML could not find or load the file.");
        Pause();
    }
    else if ((*tManager)[noiseTexID].BindTexture())
    {
        TextureSettings(TextureSettings::TF_LINEAR, TextureSettings::TW_WRAP, false).SetData();
        quad->GetMesh().Uniforms.TextureUniforms[TTW::NoiseSamplerName] =
            UniformSamplerValue((*tManager)[noiseTexID], TTW::NoiseSamplerName);
    }
    else
    {
        std::cout << "Error: couldn't access noise texture ID after creating it.\n";
        Pause();
    }


    //Ask the user for a custom texture.

    if (!getUserTex) return;

    customTexID = tManager->CreateSFMLTexture();
    bool valid = false;
    bool first = true;

    while (!valid)
    {
        valid = false;

        //Ask the user for a texture path if necessary.
        if (!first || askUserTexPath)
        {
            std::cout << "Enter a texture path relative to this program's .exe:\n";
            customTexPath = "";
            std::getline(std::cin, customTexPath);
        }

        first = false;

        if (!(*tManager)[customTexID].SFMLTex->loadFromFile(customTexPath))
        {
            PrintData("Error loading texture '" + customTexPath + "'", "SFML could not find or load the file.");
            std::cout << "\n\n";
            Pause();
            std::cout << "\n\n\n\n\n\n";
            continue;
        }


        valid = true;
    }

    TextureSettings(TextureSettings::TF_LINEAR, TextureSettings::TW_WRAP, false).SetData();
    quad->GetMesh().Uniforms.TextureUniforms[TTW::CustomSamplerName] =
        UniformSamplerValue((*tManager)[customTexID], TTW::CustomSamplerName);
}


#pragma endregion



//Miscellaneous world data.

MovingCamera worldCam(10.0f, 0.03f);

const Vector2i windowSize(500, 500);
float shaderLoadedTime = 0.0f;
bool isInFocus = true;


void SetUpInput(InputManager<unsigned int> & input)
{
    input.AddBoolInput(1, BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Num1, BoolInput::JustPressed)));
    input.AddBoolInput(2, BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Num2, BoolInput::JustPressed)));
    input.AddBoolInput(3, BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Num3, BoolInput::JustPressed)));
    input.AddBoolInput(4, BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Num4, BoolInput::JustPressed)));

    input.AddBoolInput(5, BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Escape, BoolInput::JustPressed)));
}


const std::string TTW::CustomSamplerName = "u_mySampler",
                  TTW::NoiseSamplerName = "u_noiseSampler",
                  TTW::ShaderElapsedName = "u_shader_seconds";


TTW::TwoTrianglesWorld(void)
    : SFMLOpenGLWorld(windowSize.x, windowSize.y)
{
    mat = 0;
    quad = 0;
}

void TTW::InitializeWorld(void)
{
    mat = 0;
    quad = 0;

    SetUpInput(Input);

    tManager = &Textures;


	//GL/SFML/window initialization.

	SFMLOpenGLWorld::InitializeWorld();
	if (IsGameOver()) return;

	GetWindow()->setVerticalSyncEnabled(true);
	GetWindow()->setMouseCursorVisible(true);

    //Create the quad and the material.
    CreateQuad();
    GetCreateMaterial();

    //Set up textures.
    LoadTextures(DoesMatUseCustomTex());


    //Set up camera.
	worldCam.SetPosition(Vector3f());
    worldCam.SetRotation(Vector3f(1.0, 0.0, 0.0), Vector3f(0.0, 1.0, 0.0), true);
    worldCam.Window = GetWindow();

    RenderingState(false, false, RenderingState::Cullables::C_BACK).EnableState();
}

void TTW::OnWorldEnd(void)
{
	DeleteAndSetToNull(mat);
    DeleteAndSetToNull(quad);
}
TTW::~TwoTrianglesWorld(void)
{
    DeleteAndSetToNull(mat);
    DeleteAndSetToNull(quad);
}

void TTW::OnInitializeError(std::string errorMsg)
{
	EndWorld();

	SFMLOpenGLWorld::OnInitializeError(errorMsg);

	Pause();
}


void TTW::OnOtherWindowEvent(sf::Event & event)
{
	if (event.type == sf::Event::LostFocus)
		isInFocus = false;
	if (event.type == sf::Event::GainedFocus)
		isInFocus = true;
}


void TTW::UpdateWorld(float elapsedSeconds)
{
    //Update camera.
    if (isInFocus) worldCam.Update(elapsedSeconds);

    //Update time.
    shaderLoadedTime += elapsedSeconds;
    quad->GetMesh().Uniforms.FloatUniforms[TTW::ShaderElapsedName].SetValue(shaderLoadedTime);

    //Reload materials/textures based on player input.
    if (isInFocus)
    {
        if (Input.GetBoolInputValue(1))
        {
            GetCreateMaterial(false);
            shaderLoadedTime = 0.0f;
        }
        if (Input.GetBoolInputValue(2))
        {
            GetCreateMaterial(true);
            shaderLoadedTime = 0.0f;
        }
        if (Input.GetBoolInputValue(3))
        {
            LoadTextures(DoesMatUseCustomTex(), false);
        }
        if (Input.GetBoolInputValue(4))
        {
            LoadTextures(DoesMatUseCustomTex(), true);
        }
    }
}

void TTW::RenderOpenGL(float elapsedSeconds)
{
	Matrix4f id;
	id.SetAsIdentity();
    TransformObject trans;
    RenderInfo info(this, &worldCam, &trans, &id, &id, &id);


	ScreenClearer().ClearScreen();
    if (!quad->Render(RenderPasses::BaseComponents, info, *mat))
    {
        PrintData("Error rendering quad", mat->GetErrorMsg());
        std::cout << "\n\n\n";
        Pause();
        std::cout << "\n\n\n\n\n";
    }
}

void TTW::OnWindowResized(unsigned int newWidth, unsigned int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
    worldCam.Info.Width = newWidth;
    worldCam.Info.Height = newHeight;
}