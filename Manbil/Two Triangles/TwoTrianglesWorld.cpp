#include "TwoTrianglesWorld.h"

#include "../Rendering/Basic Rendering/RenderingState.h"
#include "../Rendering/Basic Rendering/ScreenClearer.h"
#include "../Rendering/Basic Rendering/Viewport.h"
#include "../Rendering/Textures/MTexture2D.h"
#include "../Input/Input Objects/KeyboardBoolInput.h"
#include "../Input/MovingCamera.h"
#include "tinydir.h"
#include <iostream>
#include <fstream>


//Debug printing stuff.
namespace TTWPrints
{
	void Pause(void)
    {
        std::cout << "Enter any character to continue.\n";
        std::string dummy;
        std::getline(std::cin, dummy);
    }
	void PrintData(std::string datIntro, std::string dat, bool useLineBreak = true)
	{
		std::cout << datIntro << ": " << dat;
		if (useLineBreak) std::cout << "\n";
	}
	bool PrintRenderError(const char* errorIntro)
	{
		const char* error = GetCurrentRenderingError();
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


Material* mat = 0;
UniformDictionary params;
std::string shaderPath = "";

bool CreateMaterial(const std::string& vs, const std::string& fs, UniformDictionary& uniforms,
                    std::string& outErr)
{
    if (mat != 0)
    {
        delete mat;
    }

    mat = new Material(vs, fs, uniforms, DrawingQuad::GetVertexInputData(),
                       BlendMode::GetOpaque(), outErr);

    return outErr.empty();
}

//Continuously asks the user for a valid shader and tries to create the material.
//Exits once a valid material is created.
void GetCreateMaterial(bool askForFile = true)
{
    std::string vs, fs;
    UniformDictionary uniforms;
    bool valid = false;
    typedef MaterialConstants MC;

    //The vertex shader is a very simple program.
    RenderIOAttributes vertIns = DrawingQuad::GetVertexInputData();
    MaterialUsageFlags vertFlags;
    vs = MC::GetVertexHeader("out vec2 in_UV;\n", vertIns, vertFlags) + "\n\n\
void main()                              \n\
{                                        \n\
    gl_Position = vec4(" + vertIns.GetAttribute(0).Name + ", 1.0);  \n\
    in_UV = " + vertIns.GetAttribute(1).Name + ";\n\
}";
    //Add any custom QuadWorld uniforms.
    float initialTimeVal = 0.0f;
    uniforms[TTW::ShaderElapsedName] = Uniform::MakeF(TTW::ShaderElapsedName, 1, &initialTimeVal);
    uniforms[TTW::CustomSamplerName] = Uniform(TTW::CustomSamplerName, UT_VALUE_SAMPLER2D);
    uniforms[TTW::NoiseSamplerName] = Uniform(TTW::NoiseSamplerName, UT_VALUE_SAMPLER2D);

    bool first = true;

    while (!valid)
    {
        //Ask for a shader file path if it is needed.
        if (!first || askForFile)
        {
            valid = false;
            std::cout << "Enter a shader path relative to the \"Shaders\" folder.\n" <<
                         "Enter nothing to get a list of all files in that folder.\n";

            shaderPath = "";
            std::getline(std::cin, shaderPath);
            if (shaderPath.empty())
            {
                //Get all files inside the "Shaders" folder and display them.
                
                tinydir_dir dir;
                tinydir_open_sorted(&dir, "Shaders");

                for (unsigned int i = 0; i < dir.n_files; ++i)
                {
                    tinydir_file file;
                    tinydir_readfile_n(&dir, &file, i);

                    std::cout << "* " << file.name;
                    if (file.is_dir)
                        std::cout << "/";
                    std::cout << "\n";
                }

                tinydir_close(&dir);
                std::cout << "\n\n";

                continue;
            }
            if (shaderPath.find(".txt") == std::string::npos)
            {
                shaderPath += ".txt";
            }

            shaderPath = "Shaders/" + shaderPath;
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
        fs = MC::GetFragmentHeader("in vec2 in_UV;\n", "out vec4 out_FinalColor;\n", flags);
        fs += "\n\
uniform float " + TTW::ShaderElapsedName + ";   \n\
uniform sampler2D " + TTW::CustomSamplerName + ";\n\
uniform sampler2D " + TTW::NoiseSamplerName + ";\n\n\n";
        std::string line = "";
        while (std::getline(reader, line))
        {
            fs += line + "\n";
        }
        reader.close();

        //Try creating the material.
        std::string err;
        if (!CreateMaterial(vs, fs, uniforms, err))
        {
            PrintData("Error compiling material", err);
            std::cout << "\n\n";
            Pause();
            std::cout << "\n\n\n\n\n\n";
            continue;
        }
        
        RenderObjHandle customTexHandle =
            (params.find(TTW::CustomSamplerName) == params.end() ?
                INVALID_RENDER_OBJ_HANDLE :
                params[TTW::CustomSamplerName].Tex());
        RenderObjHandle noiseTexHandle =
            (params.find(TTW::NoiseSamplerName) == params.end() ?
                INVALID_RENDER_OBJ_HANDLE :
                params[TTW::NoiseSamplerName].Tex());
        Uniform::AddUniforms(uniforms, params, true);
        params[TTW::CustomSamplerName].Tex() = customTexHandle;
        params[TTW::NoiseSamplerName].Tex() = noiseTexHandle;

        valid = true;
    }
}

//Gets whether or not "mat" uses the custom user texture.
bool DoesMatUseCustomTex(void)
{
    const UniformList& uniforms = mat->GetUniforms();
    auto location = std::find_if(uniforms.begin(), uniforms.end(),
                                 [](const Uniform& unf)
                                 {
                                     return unf.Name == TTW::CustomSamplerName;
                                 });
    return location != uniforms.end();
}


#pragma endregion


#pragma region Textures


MTexture2D customTex(TextureSampleSettings2D(FT_LINEAR, WT_WRAP), PixelSizes::PS_32F, false),
           noiseTex(TextureSampleSettings2D(FT_NEAREST, WT_WRAP), PixelSizes::PS_32F, false);
std::string customTexPath = "";

//Gets the noise texture as well as a custom texture from the user if necessary.
//Puts all loaded textures into the correct uniform values for the quad.
void LoadTextures(bool getUserTex, bool askUserTexPath = true)
{
    //Get the noise texture.
    
    if (!noiseTex.IsValidTexture())
    {
        noiseTex.Create();
    }
    std::string error;
    if (!noiseTex.SetDataFromFile("Content/Textures/NoiseTex.png", error))
    {
        PrintData("Error loding 'Content/Textures/NoiseTex.png'", error);
        Pause();
    }
    params[TTW::NoiseSamplerName].Tex() = noiseTex.GetTextureHandle();


    //Ask the user for a custom texture.

    if (!getUserTex)
    {
        return;
    }

    if (!customTex.IsValidTexture())
    {
        customTex.Create();
    }

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

        //Try loading the file.
        if (!customTex.SetDataFromFile(customTexPath, error))
        {
            PrintData("Error loading '" + customTexPath + "'", error);
            std::cout << "\n\n";
            Pause();
            std::cout << "\n\n\n\n\n\n";
            continue;
        }

        valid = true;
    }

    params[TTW::CustomSamplerName] = Uniform(TTW::CustomSamplerName, UT_VALUE_SAMPLER2D);
    params[TTW::CustomSamplerName].Tex() = customTex.GetTextureHandle();
}


#pragma endregion



//Miscellaneous world data.

MovingCamera worldCam(10.0f, 0.03f);

const Vector2i windowSize(500, 500);
float shaderLoadedTime = 0.0f;
bool isInFocus = true;


void SetUpInput(InputManager<unsigned int>& input)
{
    input.AddBoolInput(1, BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Num1,
                                                                         BoolInput::JustPressed)));
    input.AddBoolInput(2, BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Num2,
                                                                         BoolInput::JustPressed)));
    input.AddBoolInput(3, BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Num3,
                                                                         BoolInput::JustPressed)));
    input.AddBoolInput(4, BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Num4,
                                                                         BoolInput::JustPressed)));

    input.AddBoolInput(5, BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Escape,
                                                                         BoolInput::JustPressed)));
}


const std::string TTW::CustomSamplerName = "u_mySampler",
                  TTW::NoiseSamplerName = "u_noiseSampler",
                  TTW::ShaderElapsedName = "u_shader_seconds";


TTW::TwoTrianglesWorld(void)
    : SFMLOpenGLWorld(windowSize.x, windowSize.y)
{
    mat = 0;
}

void TTW::InitializeWorld(void)
{
    mat = 0;

    SetUpInput(Input);


	//GL/SFML/window initialization.
	SFMLOpenGLWorld::InitializeWorld();
    if (IsGameOver())
    {
        return;
    }


	GetWindow()->setVerticalSyncEnabled(true);
	GetWindow()->setMouseCursorVisible(true);

    //Create the quad and the material.
    DrawingQuad::InitializeQuad();
    GetCreateMaterial();

    //Set up textures.
    LoadTextures(DoesMatUseCustomTex());


    //Set up camera.
	worldCam.SetPosition(Vector3f());
    worldCam.SetRotation(Vector3f(1.0, 0.0, 0.0), Vector3f(0.0, 1.0, 0.0));
    worldCam.Window = GetWindow();

    RenderingState(RenderingState::C_BACK).EnableState();
}

void TTW::OnWorldEnd(void)
{
    delete mat;
    noiseTex.DeleteIfValid();
    DrawingQuad::DestroyQuad();
}

void TTW::OnInitializeError(std::string errorMsg)
{
	EndWorld();

	SFMLOpenGLWorld::OnInitializeError(errorMsg);

	Pause();
}


void TTW::OnOtherWindowEvent(sf::Event& event)
{
    if (event.type == sf::Event::LostFocus)
    {
        isInFocus = false;
    }
    if (event.type == sf::Event::GainedFocus)
    {
        isInFocus = true;
    }
}


void TTW::UpdateWorld(float elapsedSeconds)
{
    //Update camera.
    if (isInFocus)
    {
        worldCam.Update(elapsedSeconds);
    }

    //Update time.
    shaderLoadedTime += elapsedSeconds;
    params[TTW::ShaderElapsedName].Float().SetValue(shaderLoadedTime);

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
	Matrix4f identity;
    RenderInfo info(GetTotalElapsedSeconds(), &worldCam, &identity, &identity);

    ScreenClearer().ClearScreen();
    Viewport(0, 0, GetWindow()->getSize().x, GetWindow()->getSize().y).Use();

    RenderingState(RenderingState::C_BACK).EnableState();
    DrawingQuad::GetInstance()->Render(Transform(), info, *mat, params);
}

void TTW::OnWindowResized(unsigned int newWidth, unsigned int newHeight)
{
    worldCam.PerspectiveInfo.Width = newWidth;
    worldCam.PerspectiveInfo.Height = newHeight;
}