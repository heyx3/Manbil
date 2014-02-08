#include "TwoTrianglesWorld.h"

#include "Vertices.h"
#include "MovingCamera.h"
#include "TextureSettings.h"
#include "RenderingState.h"
#include "ScreenClearer.h"
#include <iostream>
#include <fstream>

typedef TwoTrianglesWorld TTW;


//Debug printing stuff.
namespace TTWPrints
{
	void Pause(void) { char dummy; std::cin >> dummy; }
	void PrintData(string datIntro, string dat, bool useLineBreak = true)
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




//Fragment shader loading code.

string shaderPath = "";

//Continuously asks the user for a fragment shader file.
//Returns the contents of the first valid text file the user entered.
//Does NOT try to compile or otherwise verify that the actual shader code is valid code.
string GetFragmentShader()
{
	std::ifstream file;


	//Keep asking until the user provides a valid path.

	while (true)
	{
		//Get the file path.

		if (strcmp(shaderPath.c_str(), "") == 0)
		{
			std::cout << "Enter the path of the fragment shader to use:\n";
			std::cin >> shaderPath;
		}



		//Attempt to open the file.

		file.open(shaderPath.c_str());
		if (!file.is_open())
		{
			std::cout << "Sorry, that file could not be accessed.\n\n";
			shaderPath = "";
			continue;
		}



		//Read the file.

		string text = "";
		string line;
		while (std::getline(file, line))
		{
			text += line;
			text += "\n";
		}
		file.close();
		return text;
	}
}

//Gets a shader from the user, then allocates a new TTF material using it.
//Continues doing this until a valid material is succesfully created.
void LoadTTFShader(TwoTrianglesMaterial * & tMat)
{
	while (true)
	{
		if (tMat != 0) delete tMat;
		tMat = 0;

		tMat = new TwoTrianglesMaterial(GetFragmentShader());
		if (strcmp(tMat->GetErrorMessage(), "") != 0)
		{
			std::cout << "\nSorry, that is not a valid GLSL program. Compilation errors:\n" << tMat->GetErrorMessage() << "\n\n";
			shaderPath = "";
		}
		else
		{
			break;
		}
	}

	//Now output instructions.
	std::cout << "\n\n\n\n\n\n\n\n";
	std::cout << "Press '1' to reload the shader.\n" <<
				 "Press '2' to choose a new shader.\n" <<
				 "Press '3' to reload the loaded image into 'u_sampler_1'.\n" <<
				 "Press '4' to load a new image into 'u_sampler_1'.\n";
}




//Texture loading code.

string texPath = "";
RenderObjHandle texObjLoaded = RenderDataHandler::INVALID_UNIFORM_LOCATION,
				noiseObjLoaded = RenderDataHandler::INVALID_UNIFORM_LOCATION;

//Continuously asks the user fora texture until a valid one is supplied.
void GetLoadedTexture(void)
{
	sf::Image img;

	while (true)
	{
		//Get the file path.

		if (strcmp(texPath.c_str(), "") == 0)
		{
			std::cout << "Enter the path of the texture for u_sampler_1:\n";
			std::cin >> texPath;
		}



		//Attempt to read the image.

		if (!img.loadFromFile(texPath))
		{
			std::cout << "Sorry, that image could not be read.\n\n";
			texPath = "";
			continue;
		}

		break;
	}

	//Try to delete the previous texture whether or not it existed, then just ignore the potential resuling OpenGL error --
	//     if GL generated an error, that just means the texture doesn't need to be deleted.
	RenderDataHandler::DeleteTexture2D(texObjLoaded);
	ClearAllRenderingErrors();

	//Create a new replacement texture.
	RenderDataHandler::CreateTexture2D(texObjLoaded, img);
	if (!PrintRenderError("Error creating a new 2d texture")) return;
}

void SetUpLoadedTexture(TwoTrianglesMaterial * mat)
{
	GetLoadedTexture();
	
	TextureSettings(TextureSettings::TextureFiltering::TF_LINEAR, TextureSettings::TextureFiltering::TF_LINEAR,
					TextureSettings::TextureWrapping::TW_CLAMP, TextureSettings::TextureWrapping::TW_CLAMP).SetData(texObjLoaded);
	PrintRenderError("Error occurred setting texture info for custom image.");

	mat->SetTexture(TwoTrianglesMaterial::FileSampler, texObjLoaded);
}





const Vector2i windowSize(500, 500);
float shaderLoadedTime = 0.0f;
bool isInFocus = true;



MovingCamera shaderCam;

bool shouldCaptureMouse = false,
	 mousePressedLastFrame = false;


TTW::TwoTrianglesWorld(void) : SFMLOpenGLWorld(windowSize.x, windowSize.y), mat(0)
{

}

void TTW::InitializeWorld(void)
{
	//GL/SFML/window initialization.

	SFMLOpenGLWorld::InitializeWorld();
	if (IsGameOver()) return;

	GetWindow()->setVerticalSyncEnabled(true);
	GetWindow()->setMouseCursorVisible(true);

	GetWindow()->setMouseCursorVisible(!shouldCaptureMouse);

	//Create the material to render the triangles.

	LoadTTFShader(mat);
	mat->SetResolution(Vector2f(GetWindow()->getSize().x, GetWindow()->getSize().y));

	//Noise sampler texture.
	mat->SetSamplerTextureUnit(0, TwoTrianglesMaterial::NoiseSampler);
	sf::Image noiseImg;
	if (!noiseImg.loadFromFile("NoiseTex.png"))
	{
		PrintData("Error loading 'NoiseTex.png'", "SFML could not load the file.");
	}
	else
	{
		RenderDataHandler::CreateTexture2D(noiseObjLoaded, noiseImg);
		PrintRenderError("Error occurred creating specified texture 2D");
			
		TextureSettings(TextureSettings::TextureFiltering::TF_NEAREST, TextureSettings::TextureWrapping::TW_WRAP).SetData(noiseObjLoaded);
		PrintRenderError("Error occurred setting texture info for noise image.");

		mat->SetTexture(TwoTrianglesMaterial::NoiseSampler, noiseObjLoaded);
	}

	//Loaded image sampler texture.
	if (mat->UsesSampler(TwoTrianglesMaterial::FileSampler))
	{
		SetUpLoadedTexture(mat);
	}

	std::cout << "\nShader accepted.\n\n\n";

	
	shaderCam = MovingCamera();
	shaderCam.SetPosition(Vector3f());
	shaderCam.SetRotation(Vector3f(1.0, 0.0, 0.0), Vector3f(0.0, 1.0, 0.0), true);
}

void TTW::OnWorldEnd(void)
{
	DeleteAndSetToNull(mat);
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


bool pressedLastFrame_1 = false,
	 pressedLastFrame_2 = false,
	 pressedLastFrame_3 = false,
	 pressedLastFrame_4 = false;
void TTW::UpdateWorld(float elapsedSeconds)
{
	shaderCam.Window = (shouldCaptureMouse ? GetWindow() : 0);
	if (isInFocus) shaderCam.Update(elapsedSeconds);

	//React to player input.
	#define KeyPressed(key) (sf::Keyboard::isKeyPressed(sf::Keyboard::Key:: ## key))

	if (isInFocus)
	{
		if (KeyPressed(Num1))
		{
			if (!pressedLastFrame_1)
			{
				LoadTTFShader(mat);
				shaderLoadedTime = GetTotalElapsedSeconds();
				mat->SetResolution(Vector2f(GetWindow()->getSize().x, GetWindow()->getSize().y));
				mat->SetSamplerTextureUnit(0, TwoTrianglesMaterial::NoiseSampler);
				mat->SetTexture(TwoTrianglesMaterial::NoiseSampler, noiseObjLoaded);
				mat->SetTexture(TwoTrianglesMaterial::FileSampler, texObjLoaded);
			}

			pressedLastFrame_1 = true;
		}
		else pressedLastFrame_1 = false;

		if (KeyPressed(Num2))
		{
			if (!pressedLastFrame_2)
			{
				std::cout << "\n\n\n\n\n\n";
				shaderPath = "";
				LoadTTFShader(mat);
				shaderLoadedTime = GetTotalElapsedSeconds();
				mat->SetResolution(Vector2f(GetWindow()->getSize().x, GetWindow()->getSize().y));
				mat->SetResolution(Vector2f(GetWindow()->getSize().x, GetWindow()->getSize().y));
				mat->SetTexture(TwoTrianglesMaterial::NoiseSampler, noiseObjLoaded);
				mat->SetTexture(TwoTrianglesMaterial::FileSampler, texObjLoaded);
			}

			pressedLastFrame_2 = true;
		}
		else pressedLastFrame_2 = false;

		if (KeyPressed(Num3))
		{
			if (!pressedLastFrame_3)
			{
				SetUpLoadedTexture(mat);
			}

			pressedLastFrame_3 = true;
		}
		else pressedLastFrame_3 = false;

		if (KeyPressed(Num4))
		{
			if (!pressedLastFrame_4)
			{
				texPath = "";
				SetUpLoadedTexture(mat);
			}

			pressedLastFrame_4 = true;
		}
		else pressedLastFrame_4 = false;



		if (KeyPressed(Escape))
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


	

	//Update shader data.

	if (!mat->SetElapsedTime(GetTotalElapsedSeconds() - shaderLoadedTime))
	{
		std::cout << "Error setting elapsed time: " << mat->GetErrorMessage() << "\n";
		Pause();
	}
	if (!mat->SetCamera(shaderCam))
	{
		std::cout << "Error setting camera: " << mat->GetErrorMessage() << "\n";
		Pause();
	}
}

void TTW::RenderWorld(float elapsedSeconds)
{
	Matrix4f id;
	id.SetAsIdentity();

	ScreenClearer clearer;


	clearer.ClearScreen();
	if (!PrintRenderError("Error clearing back buffer of color and depth")) return;

	if (!mat->Render())
	{
		PrintData("Error drawing two triangles", mat->GetErrorMessage());
	}
	GetWindow()->display();
}

void TTW::OnWindowResized(unsigned int newWidth, unsigned int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
	if (!PrintRenderError("Error updating the OpenGL viewport size"))
	{
		EndWorld();
		return;
	}

	if (!mat->SetResolution(Vector2f(newWidth, newHeight)))
	{
		PrintData("Error setting resolution of TTM", mat->GetErrorMessage());
		EndWorld();
		return;
	}
}