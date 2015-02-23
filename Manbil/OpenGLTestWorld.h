#pragma once


#include "SFMLOpenGLWorld.h"
#include "Math/HigherMath.hpp"
#include "MovingCamera.h"
#include "Mesh.h"
#include "Rendering/Water/Water.h"
#include "Rendering/PostProcessing/PostProcessChain.h"
#include "Rendering/GPU Particles/GPUParticleGenerator.h"
#include "Rendering/Texture Management/RenderTargetManager.h"
#include "Rendering/GUI/TextRenderer.h"
#include "Rendering/Texture Management/MTextureCubemap.h"
#include "Rendering/Texture Management/MTexture3D.h"


class OpenGLTestWorld : public SFMLOpenGLWorld
{
public:

    RenderTargetManager* RenderTargets;


	OpenGLTestWorld(void);


protected:

	virtual void InitializeWorld(void) override;
	virtual void OnWorldEnd(void) override;

	virtual void UpdateWorld(float elapsedSeconds) override;
	virtual void RenderOpenGL(float elapsedSeconds) override;

	virtual void OnInitializeError(std::string errorMsg) override;
	virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;


private:

    void InitializeTextures(void);
    void InitializeMaterials(void);
    void InitializeObjects(void);

	void RenderWorldGeometry(const RenderInfo& info);


    Water* water;
    Material* waterMat;

    Mesh particleMesh;
    Material* particleMat;
    UniformDictionary particleParams;

	MovingCamera cam;

    unsigned int worldRenderID;
    MTexture2D worldColorTex1, worldColorTex2, worldDepthTex;

    Mesh cubemapMesh;
    UniformDictionary cubemapParams;
    Material* cubemapMat;
    MTextureCubemap cubemapTex;

    Material* finalScreenMat;
    UniformDictionary finalScreenQuadParams;

    MTexture2D waterNormalTex1, waterNormalTex2;
};