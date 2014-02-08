#include "OpenGLTestWorld.h"

#include "Material.h"
#include <iostream>
#include "Vertices.h"
#include "ScreenClearer.h"
#include "RenderingState.h"
#include "RenderDataHandler.h"
#include "TextureSettings.h"
#include "Input/Input Objects/MouseBoolInput.h"
#include "Math/Higher Math/BumpmapToNormalmap.h"
#include "Rendering/Texture Management/TextureConverters.h"
#include "Rendering/Helper Classes/DrawingQuad.h"

#include "Math/NoiseGeneration.hpp"

#include <assert.h>


namespace OGLTestPrints
{
	bool PrintRenderError(const char * errorIntro)
{
	const char * error = GetCurrentRenderingError();
	if (strcmp(error, "") != 0)
	{
		std::cout << errorIntro << ": " << error << "\n";
		ClearAllRenderingErrors();
		return false;
	}

	return true;
}

	void Pause(void)
	{
		char dummy;
		std::cin >> dummy;
	}
}
using namespace OGLTestPrints;



const Vector2i windowSize(1000, 700);
const RenderingState worldRenderState;

//Both "InitializeTerrain" and "InitializeObjects" need the terrain vertex positions.
Vector3f * terrainPoses = 0;

const int terrainSize = 300;
const float terrainBreadth = 5.0f, terrainHeight = 100.0f;
const float terrainTexScale = 50.0f;
void GenerateTerrainNoise(Noise2D & outNoise)
{
	Generator * finalG = 0;

	//WhiteNoise whiteNoise;
	//Interpolator interp(&whiteNoise, terrainSize, terrainSize, 5.0f);
	//finalG = &interp;

	Perlin per(60.0f, Perlin::Smoothness::Quintic);
	Perlin per2(30.0f, Perlin::Smoothness::Cubic);
	Perlin per3(5.0f, Perlin::Smoothness::Linear);

	float weights[3] = { 0.75f, 0.25f, 0.0425f };
	Generator * gens[3] = { &per, &per2, &per3 };
	LayeredOctave lay(3, weights, gens);
	finalG = &lay;

    NoiseFilterer filterer;
    filterer.FilterFunc = &NoiseFilterer::Increase;
    filterer.Increase_Amount = 1.0f;
    CircularFilterRegion circleReg(Vector2f(), 500.0f, 1.0f, 0.2f);
    filterer.FillRegion = &circleReg;
    filterer.NoiseToFilter = &lay;
    finalG = &filterer;

	if (finalG != 0) finalG->Generate(outNoise);
}


Water::RippleWaterArgs rippleArgs(Vector3f(), 50.0f, 0.85f, 0.5f, 20.0f);
void GenerateWaterNormalmap(Fake2DArray<Vector3f> & outHeight)
{
    outHeight.Fill(Vector3f(0.0f, 0.0f, 1.0f));
    Fake2DArray<float> heightmap(outHeight.GetWidth(), outHeight.GetHeight());

    //Load image.
    if (false)
    {
        sf::Image normalMap;
        if (!normalMap.loadFromFile("Normalmap.png"))
        {
            std::cout << "Error loading water normal map\n";
            Pause();
            return;
        }

        TextureConverters::ToArray(normalMap, outHeight);
        return;
    }
    else if (false)
    {
        sf::Image bumpMap;
        if (!bumpMap.loadFromFile("bumpy.png"))
        {
            std::cout << "Error loading water bumpmap\n";
            Pause();
            return;
        }
        if (bumpMap.getSize().x != outHeight.GetWidth() || bumpMap.getSize().y != outHeight.GetHeight())
        {
            std::cout << "The Fake2DArray<Vector3f> is the wrong size\n";
            Pause();
            return;
        }

        TextureConverters::ToArray(bumpMap, TextureConverters::Channels::Red, heightmap);
    }
    //Generate with noise.
    else
    {
        NoiseFilterer nf;
        MaxFilterRegion mfr;
        nf.FillRegion = &mfr;
        const int sed = 125;
        Perlin per1(32.0f, Perlin::Quintic, sed),
               per2(16.0f, Perlin::Quintic, sed + 262134),
               per3(8.0f, Perlin::Quintic, sed + 628331),
               per4(4.0f, Perlin::Quintic, sed + 278),
               per5(2.0f, Perlin::Quintic, sed + 2472);
        float weights[] = { 0.5f, 0.25f, 0.125f, 0.0625f, 0.03125f };
        Generator * noiseOctaves[] = { &per1, &per2, &per3, &per4, &per5 };

        LayeredOctave octaves(5, weights, noiseOctaves);

        nf.FilterFunc = &NoiseFilterer::UpContrast;
        nf.UpContrast_Power = NoiseFilterer::UpContrastPowers::QUINTIC;
        nf.NoiseToFilter = &octaves;

        nf.Generate(heightmap);
        nf.UpContrast(&heightmap);

    }

    BumpmapToNormalmap::Convert(heightmap, 5.0f, outHeight);
}



bool ShouldUseFramebuffer(void) { return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift); }


sf::Sprite sprToDraw;
sf::Texture tex;

DrawingQuad * quad, * quad2;
Material * unlitMat;


void OpenGLTestWorld::InitializeTextures(void)
{

    sf::Image img, shrubImg, waterImg, normalMapImg;

    //Grass texture.
    if (!img.loadFromFile("Content/Textures/Grass.png"))
    {
        std::cout << "Failed to load grass texture.\n";
        Pause();
        EndWorld();
        return;
    }
    RenderDataHandler::CreateTexture2D(grassImgH, img, true);
    TextureSettings(TextureSettings::TextureFiltering::TF_LINEAR, TextureSettings::TextureWrapping::TW_WRAP).SetData(grassImgH);
    if (!PrintRenderError("Error setting up grass texture"))
    {
        Pause();
        EndWorld();
        return;
    }

    //Shrub texture.
    if (!shrubImg.loadFromFile("Content/Textures/shrub.png"))
    {
        std::cout << "Failed to load shrub texture.\n";
        Pause();
        EndWorld();
        return;
    }
    RenderDataHandler::CreateTexture2D(shrubImgH, shrubImg);
    TextureSettings(TextureSettings::TextureFiltering::TF_LINEAR, TextureSettings::TextureWrapping::TW_WRAP).SetData(shrubImgH);
    if (!PrintRenderError("Error setting up shrub texture"))
    {
        Pause();
        EndWorld();
        return;
    }

    //Water color texture.
    if (!waterImg.loadFromFile("Content/Textures/Water.png"))
    {
        std::cout << "Failed to load water texture.\n";
        Pause();
        EndWorld();
        return;
    }
    RenderDataHandler::CreateTexture2D(waterImgH, waterImg);
    TextureSettings(TextureSettings::TextureFiltering::TF_LINEAR, TextureSettings::TextureWrapping::TW_WRAP).SetData(waterImgH);
    if (!PrintRenderError("Error setting up water texture"))
    {
        Pause();
        EndWorld();
        return;
    }

    //Water normal map texture.
    if (false)
    {
        if (!normalMapImg.loadFromFile("Content/Textures/Normalmap.png"))
        {
            std::cout << "Failed to load normal map texture.\n";
            Pause();
            EndWorld();
            return;
        }
    }
    else
    {
        Fake2DArray<Vector3f> normalMap(512, 512);
        GenerateWaterNormalmap(normalMap);

        TextureConverters::ToImage(normalMap, normalMapImg);
    }

    RenderDataHandler::CreateTexture2D(normalMapImgH, normalMapImg);
    TextureSettings(TextureSettings::TextureFiltering::TF_LINEAR, TextureSettings::TextureWrapping::TW_WRAP).SetData(normalMapImgH);
    if (!PrintRenderError("Error setting up normal map texture"))
    {
        Pause();
        EndWorld();
        return;
    }

    tex.loadFromImage(img);
    sprToDraw.setTexture(tex);
}
void OpenGLTestWorld::InitializeMaterials(void)
{
    //Terrain material.

    std::vector<RenderingPass> mats;
    mats.insert(mats.end(), Materials::LitTexture);
    testMat = new Material(mats);
    if (testMat->HasError())
    {
        std::cout << "Lit Texture material error: " << testMat->GetErrorMessage() << "\n";
        Pause();
        EndWorld();
        return;
    }

    if (!Materials::LitTexture_GetUniforms(*testMat))
    {
        std::cout << "Lit texture directional light uniform get location error.\n";
    }


    //Create post-process effect.
    ClearAllRenderingErrors();
    std::vector<RenderingPass> passes;
    passes.insert(passes.end(), Materials::EmptyPostProcess);
    effect = new PostProcessEffect(windowSize.x, windowSize.y, passes);
    if (effect->HasError())
    {
        std::cout << "Error creating render target: " << effect->GetErrorMessage() << "\n";
        Pause();
        EndWorld();
        return;
    }


    //Create test quad material.
    unlitMat = new Material(Materials::UnlitTexture);
    unlitMat->AddUniform("brightness");
    if (unlitMat->HasError())
    {
        std::cout << "Error creating unlit material: " << unlitMat->GetErrorMessage() << "\n";
        Pause();
        EndWorld();
        return;
    }
}
void OpenGLTestWorld::InitializeTerrain(void)
{
    //Gnerate the heightmap.

    Noise2D noise(terrainSize, terrainSize);
    GenerateTerrainNoise(noise);

    pTerr = new Terrain(terrainSize);
    Terrain & terr = *pTerr;
    terr.SetHeightmap(noise);


    //Create vertices.

    RenderObjHandle vs, is;
    VertexIndexData vid;

    const int size = terrainSize * terrainSize;
    assert(size == terr.GetVerticesCount());
    Vertex * vertices = new Vertex[size];
    terrainPoses = new Vector3f[size];
    Vector3f * vertexNormals = new Vector3f[size];
    Vector2f * vertexTexCoords = new Vector2f[size];
    unsigned int * indices = new unsigned int[terr.GetIndicesCount()];

    terr.CreateVertexPositions(terrainPoses, Vector2i(0, 0), Vector2i(terrainSize - 1, terrainSize - 1));
    for (int i = 0; i < size; ++i)
    {
        terrainPoses[i].x *= terrainBreadth;
        terrainPoses[i].y *= terrainBreadth;
        terrainPoses[i].z *= terrainHeight;
    }
    terr.CreateVertexNormals(vertexNormals, terrainPoses, Vector3f(1.0f, 1.0f, 1.0f), Vector2i(0, 0), Vector2i(terrainSize - 1, terrainSize - 1));
    terr.CreateVertexTexCoords(vertexTexCoords, Vector2i(0, 0), Vector2i(terrainSize - 1, terrainSize - 1));
    terr.CreateVertexIndices(indices, Vector2i(0, 0), Vector2i(terrainSize - 1, terrainSize - 1));

    for (int i = 0; i < size; ++i)
    {
        vertices[i] = Vertex(terrainPoses[i], vertexTexCoords[i], vertexNormals[i]);
    }

    RenderDataHandler::CreateVertexBuffer(vs, vertices, size, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
    RenderDataHandler::CreateIndexBuffer(is, indices, terr.GetIndicesCount(), RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
    vid = VertexIndexData(size, vs, terr.GetIndicesCount(), is);


    if (!PrintRenderError("Error creating vertex/index buffer for terrain"))
    {
        Pause();
        EndWorld();
        return;
    }

    delete[] vertices, vertexNormals, vertexTexCoords, indices;

    //Create terrain mesh.
    testMesh = Mesh(PrimitiveTypes::Triangles, 1, &vid);
    Materials::LitTexture_SetUniforms(testMesh, dirLight);
    PassSamplers terrainSamplers;
    terrainSamplers[0] = grassImgH;
    terrainSamplers.Scales[0] = Vector2f(terrainTexScale, terrainTexScale);
    testMesh.TextureSamplers.insert(testMesh.TextureSamplers.end(), terrainSamplers);
}
void OpenGLTestWorld::InitializeObjects(void)
{
    //Create the foliage before deleting the vertex data.
    std::vector<Vector3f> poses;
    FastRand fr;
    for (int i = terrainSize; i < terrainSize * terrainSize; i += 1 + (BasicMath::Abs(fr.GetRandInt()) % 200))
    {
        poses.insert(poses.end(), terrainPoses[i]);
    }
    foliage = new Foliage(poses, Vector2f(10.0f, 4.0f));
    if (foliage->HasError())
    {
        std::cout << "Error creating foliage: " << foliage->GetError();
        Pause();
        EndWorld();
        return;
    }
    foliage->SetTexture(shrubImgH);
    foliage->SetWaveSpeed(0.5f);
    foliage->SetWaveScale(2.0f);
    foliage->SetLeanAwayMaxDist(10.0f);
    foliage->SetBrightness(0.5f);

    delete[] terrainPoses;



    //Create water.

    water = new Water(300, 4, Vector3f());
    if (water->HasError())
    {
        std::cout << "Error creating water: " << water->GetErrorMessage();
        Pause();
        EndWorld();
        return;
    }

    water->Transform.SetScale(Vector3f(8.0f, 8.0f, 1.0f));
    water->Transform.IncrementPosition(Vector3f(0, 0, -30));

    Materials::LitTexture_GetUniforms(*water->Mat);
    Materials::LitTexture_SetUniforms(water->GetMesh(), dirLight);

    float amb = 0.7f, diff = 1.0f - amb;
    water->GetMesh().FloatUniformValues["DirectionalLight.Ambient"].SetData(&amb);
    water->GetMesh().FloatUniformValues["DirectionalLight.Diffuse"].SetData(&diff);

    water->GetMesh().TextureSamplers[0][0] = waterImgH;
    water->GetMesh().TextureSamplers[0].Panners[0] = Vector2f(0.0f, 0.0f);
    water->GetMesh().TextureSamplers[0].Scales[0] = Vector2f(50.0f, 50.0f);

    water->GetMesh().TextureSamplers[0][1] = normalMapImgH;
    water->GetMesh().TextureSamplers[0].Panners[1] = Vector2f(-0.005f, -0.004f);
    water->GetMesh().TextureSamplers[0].Scales[1] = Vector2f(10.0f, 10.0f);
}


OpenGLTestWorld::OpenGLTestWorld(void)
: SFMLOpenGLWorld(windowSize.x, windowSize.y, sf::ContextSettings(24, 0, 4, 3, 3)),
  testMat(0), testMesh(PrimitiveTypes::Triangles), foliage(0), pTerr(0)
{
	dirLight.Dir = Vector3f(-1.0f, -1.0f, -1.0f).Normalized();
	dirLight.Col = Vector3f(1.0f, 1.0f, 1.0f);

	dirLight.Ambient = 0.3f;
	dirLight.Diffuse = 0.7f;
	dirLight.Specular = 2.5f;
	
	dirLight.SpecularIntensity = 128.0f;
}
void OpenGLTestWorld::InitializeWorld(void)
{
	SFMLOpenGLWorld::InitializeWorld();
	if (IsGameOver()) return;
	
    Input.AddBoolInput(666, BoolInputPtr((BoolInput*)(new MouseBoolInput(sf::Mouse::Button::Left, BoolInput::ValueStates::JustPressed))));



	GetWindow()->setVerticalSyncEnabled(true);
	GetWindow()->setMouseCursorVisible(true);

    InitializeTextures();
    InitializeMaterials();
    InitializeTerrain();
    InitializeObjects();

    quad = new DrawingQuad();
    quad->SetPos(Vector2f());
    quad->SetSize(Vector2f(10.0f, 10.0f));
    quad->SetDepth(1.0f);
    quad->GetMesh().FloatUniformValues["brightness"] = Mesh::UniformValue<float>(1.0f);
    quad2 = new DrawingQuad();
    quad2->SetPos(Vector2f(0.0f, 0.0f));
    quad2->SetSize(Vector2f(10.0f, 10.0f));
    quad2->SetDepth(-1.0f);
    quad2->GetMesh().FloatUniformValues["brightness"] = Mesh::UniformValue<float>(1.0f);

    PassSamplers samplers;
    samplers[0] = grassImgH;
    quad->GetMesh().TextureSamplers.insert(quad->GetMesh().TextureSamplers.begin(), samplers);
    quad2->GetMesh().TextureSamplers.insert(quad2->GetMesh().TextureSamplers.begin(), samplers);


    //Camera.
    Vector3f pos(0, 0, terrainHeight);
    cam = TerrainWalkCamera(pos, Vector3f(1.0f, 1.0f, -0.30f), Vector3f(0, 0, 1), pTerr, 60.0f, 0.05f, GetWindow());
    cam.TerrainScale = Vector3f(terrainBreadth, terrainBreadth, terrainHeight);
    cam.Info.FOV = ToRadian(55.0f);
    cam.Info.zFar = 10000.0f;
    cam.Info.zNear = 1.0f;
    cam.Info.Width = windowSize.x;
    cam.Info.Height = windowSize.y;
}

OpenGLTestWorld::~OpenGLTestWorld(void)
{
	DeleteAndSetToNull(effect);
	DeleteAndSetToNull(testMat);
	DeleteAndSetToNull(foliage);
    DeleteAndSetToNull(pTerr);

    DeleteAndSetToNull(quad);
    DeleteAndSetToNull(quad2);

    RenderObjHandle textures[] = { grassImgH, waterImgH, normalMapImgH, shrubImgH };
    glDeleteTextures(4, textures);
}
void OpenGLTestWorld::OnWorldEnd(void)
{
	DeleteAndSetToNull(effect);
	DeleteAndSetToNull(testMat);
    DeleteAndSetToNull(foliage);
    DeleteAndSetToNull(pTerr);

    DeleteAndSetToNull(quad);
    DeleteAndSetToNull(unlitMat);

    DeleteAndSetToNull(quad);
    DeleteAndSetToNull(quad2);

    RenderObjHandle textures[] = { grassImgH, waterImgH, normalMapImgH, shrubImgH };
    glDeleteTextures(4, textures);
}

void OpenGLTestWorld::OnInitializeError(std::string errorMsg)
{
	EndWorld();

	SFMLOpenGLWorld::OnInitializeError(errorMsg);

	std::cout << "Enter any key to continue:\n";
	Pause();
}


void OpenGLTestWorld::UpdateWorld(float elapsedSeconds)
{
    quad->SetSize(Vector2f(3.0f, 10.0f * sinf(GetTotalElapsedSeconds())));
    quad2->SetSize(Vector2f(1.0f, 10.0f * sinf(GetTotalElapsedSeconds())));

	if (cam.Update(elapsedSeconds, std::shared_ptr<OculusDevice>(0)))
	{
		EndWorld();
	}

    if (Input.GetBoolInputValue(666))
    {
        Water::RippleWaterArgs argsTemp(rippleArgs);
        argsTemp.Source = cam.GetPosition();
        water->AddRipple(argsTemp);
    }

    water->Update(elapsedSeconds);
}

void OpenGLTestWorld::RenderWorldGeometry(const RenderInfo & info)
{
    if (false)
    {
        if (!quad->Render(info, *unlitMat))
        {
            std::cout << "Error rendering quad: " << unlitMat->GetErrorMessage() << "\n";
            Pause();
            EndWorld();
            return;
        }
        if (!quad2->Render(info, *unlitMat))
        {
            std::cout << "Error rendering quad2: " << unlitMat->GetErrorMessage() << "\n";
            Pause();
            EndWorld();
            return;
        }
    }

    if (false)
    {
        std::vector<const Mesh *> meshes;
        meshes.insert(meshes.begin(), &testMesh);
        if (!testMat->Render(info, meshes))
        {
            std::cout << "Error rendering world: " << testMat->GetErrorMessage() << "\n";
            Pause();
            EndWorld();
            return;
        }
    }
	
    if (false)
    {
        if (!foliage->Render(info))
        {
            std::cout << "Error rendering foliage: " << foliage->GetError() << "\n";
            Pause();
            EndWorld();
            return;
        }
    }

    if (true)
    {
        if (!water->Render(info))
        {
            std::cout << "Error rendering water: " << water->GetErrorMessage() << "\n";
            Pause();
            EndWorld();
            return;
        }
    }
}

void OpenGLTestWorld::RenderOpenGL(float elapsedSeconds)
{
	Matrix4f worldM, viewM, projM;
	TransformObject dummy;

	worldM.SetAsIdentity();
	cam.GetViewTransform(viewM);
	projM.SetAsPerspProj(cam.Info);
    //cam.GetOrthoProjection(projM);

	RenderInfo info((SFMLOpenGLWorld*)this, (Camera*)&cam, &dummy, &worldM, &viewM, &projM);
	
    //Set up Post-Process effect if necessary.
	bool should = ShouldUseFramebuffer();
	if (should)
	{
		effect->GetRenderTarget()->EnableDrawingInto();
		if (!effect->GetRenderTarget()->IsValid())
		{
			std::cout << "Error setting up render target.\n";
			Pause();
			EndWorld();
			return;
		}
	}
	
    //Draw the world.
	ScreenClearer().ClearScreen();
	RenderWorldGeometry(info);

    //Now draw the Post-Processed world render if necessary.
	if (should)
	{
        effect->GetRenderTarget()->DisableDrawingInto(windowSize.x, windowSize.y);	
		ScreenClearer().ClearScreen();
        effect->RenderEffect(info);
        if (effect->HasError())
		{
			std::cout << "Error rendering post-process effect.\n";
			Pause();
			EndWorld();
			return;
		}
	}
}


void OpenGLTestWorld::OnWindowResized(unsigned int newW, unsigned int newH)
{
	ClearAllRenderingErrors();

	glViewport(0, 0, newW, newH);
	if (!PrintRenderError("Error updating the OpenGL viewport size"))
	{
		return;
	}

    effect->GetRenderTarget()->ChangeSize(newW, newH);
    if (!effect->GetRenderTarget()->IsValid())
	{
        std::cout << "Error changing render target size: " << effect->GetRenderTarget()->GetErrorMessage() << "\n";
		Pause();
		EndWorld();
		return;
	}

	cam.Info.Width = newW;
	cam.Info.Height = newH;
}