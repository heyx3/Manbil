#pragma once

/*

#include "SFMLOpenGLWorld.h"
#include "Math/HigherMath.hpp"
#include "MovingCamera.h"
#include "Mesh.h"
#include "RenderTarget.h"
#include "Materials.h"
#include "Rendering/Objects/Water.h"
#include "Foliage.h"
#include "Rendering/PostProcessing/PostProcessEffect.h"
#include "TerrainWalkCamera.h"


class OpenGLTestWorld : public SFMLOpenGLWorld
{
public:

	OpenGLTestWorld(void);
	~OpenGLTestWorld(void);

protected:

	virtual void InitializeWorld(void) override;

	virtual void UpdateWorld(float elapsedSeconds) override;

	virtual void RenderOpenGL(float elapsedSeconds) override;

	virtual void OnInitializeError(std::string errorMsg) override;

	virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;

	virtual void OnWorldEnd(void) override;

private:

    void InitializeTextures(void);
    void InitializeMaterials(void);
    void InitializeTerrain(void);
    void InitializeObjects(void);

	void RenderWorldGeometry(const RenderInfo & info);

	Material * testMat;
    PostProcessEffect * effect;
	Foliage * foliage;
    Water * water;
    Terrain * pTerr;

	Mesh testMesh;
	TerrainWalkCamera cam;
	Materials::LitTexture_DirectionalLight dirLight;

    RenderObjHandle grassImgH, normalMapImgH, shrubImgH, waterImgH;
};

*/