#pragma once

#include "SFMLOpenGLWorld.h"
#include "Math/HigherMath.hpp"
#include "MovingCamera.h"
#include "Mesh.h"
#include "RenderTarget.h"
#include "Materials.h"
#include "Foliage.h"
#include "Rendering/PostProcessing/PostProcessEffect.h"


class OpenGLTestWorld : public SFMLOpenGLWorld
{
public:

	OpenGLTestWorld(void);
	~OpenGLTestWorld(void);

protected:

	virtual void InitializeWorld(void) override;

	virtual void UpdateWorld(float elapsedSeconds) override;
	virtual void RenderWorld(float elapsedSeconds) override;

	virtual void OnInitializeError(std::string errorMsg) override;

	virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;

	virtual void OnWorldEnd(void) override;

private:

	void RenderWorldGeometry(const RenderInfo & info);

	Material * testMat;
    PostProcessEffect * effect;
	Foliage * foliage;

	Mesh testMesh;
	MovingCamera cam;
	Materials::LitTexture_DirectionalLight dirLight;
};