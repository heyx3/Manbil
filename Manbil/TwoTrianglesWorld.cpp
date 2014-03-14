#include "TwoTrianglesWorld.h"

#include "TextureSettings.h"
#include "RenderingState.h"
#include "ScreenClearer.h"
#include "Rendering/Texture Management/TextureManager.h"
#include "Input/Input Objects/KeyboardBoolInput.h"
#include "MovingCamera.h"
#include <iostream>
#include <fstream>

typedef TwoTrianglesWorld TTW;


//Debug printing stuff.
namespace TTWPrints
{
	void Pause(void) { std::cout << "Enter any character to continue.\n"; char dummy; std::cin >> dummy; }
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



Material * mat = 0;
DrawingQuad * quad = 0;
std::string shaderPath = "";

bool CreateMaterial(const std::string & vs, const std::string & fs, UniformDictionary & uniforms)
{
    if (mat != 0) delete mat;
    mat = new Material(vs, fs, uniforms, RenderingModes::RM_Opaque, false, LightSettings(false));

    return !mat->HasError();
}
void CreateQuad(void)
{
    if (quad != 0) delete quad;
    quad = new DrawingQuad();
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
    vs = MC::GetVertexHeader(false) + "\n\n\
void main()                              \n\
{                                        \n\
    gl_Position = " + MC::InObjPos + ";  \n\
    " + MC::OutUV + " = " + MC::InUV + ";\n\
}";
    //Add any custom QuadWorld uniforms.
    uniforms.FloatUniforms["u_shader_seconds"] = UniformValueF(0.0f, "u_shader_seconds");

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

        //Read in the shader.
        fs = MC::GetFragmentHeader(false);
        fs += "\nuniform float u_shader_seconds;\n\n";
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
                                     return unf.Name == TwoTrianglesWorld::CustomSamplerName;
                                 });
    return location != uniforms.end();
}



TextureManager * tManager;
unsigned int customTexID = TextureManager::UNUSED_ID,
             noiseTexID = TextureManager::UNUSED_ID;
std::string customTexPath = "";

//Gets the noise texture as well as a custom texture from the user if necessary.
//Puts all loaded textures into the correct uniform values for the quad.
void LoadTextures(bool getUserTex, bool askUserTexPath = true)
{
    //Get the noise texture.

    noiseTexID = tManager->CreateTexture("NoiseTex.png");
    if (noiseTexID == TextureManager::UNUSED_ID)
    {
        PrintData("Error loading 'NoiseTex.png'", "SFML could not find or load the file.");
        Pause();
    }
    else
    {
        sf::Texture::bind((*tManager)[noiseTexID]);
        TextureSettings(TextureSettings::TF_LINEAR, TextureSettings::TW_WRAP, false).SetData();
        quad->GetMesh().Uniforms.TextureUniforms[TwoTrianglesWorld::NoiseSamplerName] =
            UniformSamplerValue((*tManager)[noiseTexID], TwoTrianglesWorld::NoiseSamplerName);
    }


    //Ask the user for a custom texture.

    if (!getUserTex) return;

    customTexID = tManager->CreateTexture();
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

        if (!(*tManager)[customTexID]->loadFromFile(customTexPath))
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
    quad->GetMesh().Uniforms.TextureUniforms[TwoTrianglesWorld::CustomSamplerName] =
        UniformSamplerValue((*tManager)[customTexID], TwoTrianglesWorld::CustomSamplerName);
}


void SetUpInput(InputManager<unsigned int> & input)
{
    input.AddBoolInput(1, BoolInputPtr(new KeyboardBoolInput(KeyboardBoolInput::Key::Num1, BoolInput::JustPressed)));
    input.AddBoolInput(2, BoolInputPtr(new KeyboardBoolInput(KeyboardBoolInput::Key::Num2, BoolInput::JustPressed)));
    input.AddBoolInput(3, BoolInputPtr(new KeyboardBoolInput(KeyboardBoolInput::Key::Num3, BoolInput::JustPressed)));
    input.AddBoolInput(4, BoolInputPtr(new KeyboardBoolInput(KeyboardBoolInput::Key::Num4, BoolInput::JustPressed)));

    input.AddBoolInput(5, BoolInputPtr(new KeyboardBoolInput(KeyboardBoolInput::Key::Escape, BoolInput::JustPressed)));
}



MovingCamera worldCam(10.0f, 0.03f);

const Vector2i windowSize(500, 500);
float shaderLoadedTime = 0.0f;
bool isInFocus = true;

bool shouldCaptureMouse = false,
     mousePressedLastFrame = false;


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

	GetWindow()->setMouseCursorVisible(!shouldCaptureMouse);

    //Create the quad and the material.
    CreateQuad();
    GetCreateMaterial();

    //Set up textures.
    LoadTextures(DoesMatUseCustomTex());


    //Set up cameras.
    
	worldCam.SetPosition(Vector3f());
    worldCam.SetRotation(Vector3f(1.0, 0.0, 0.0), Vector3f(0.0, 1.0, 0.0), true);
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
    worldCam.Window = (shouldCaptureMouse ? GetWindow() : 0);
    if (isInFocus) worldCam.Update(elapsedSeconds);

    //Update time.
    shaderLoadedTime += elapsedSeconds;
    quad->GetMesh().Uniforms.FloatUniforms["u_shader_seconds"].SetValue(shaderLoadedTime);

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

    //Update mouse capturing.
    if (Input.GetBoolInputValue(5))
    {
        shouldCaptureMouse = false;
        GetWindow()->setMouseCursorVisible(!shouldCaptureMouse);
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        if (!mousePressedLastFrame)
        {
            shouldCaptureMouse = !shouldCaptureMouse;
            GetWindow()->setMouseCursorVisible(!shouldCaptureMouse);
        }

        mousePressedLastFrame = true;
    }
    else mousePressedLastFrame = false;
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