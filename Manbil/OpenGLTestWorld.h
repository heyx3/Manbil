#pragma once


#include "SFMLOpenGLWorld.h"
#include "Math/HigherMath.hpp"
#include "MovingCamera.h"
#include "Mesh.h"
#include "Math/Higher Math/Lighting.h"
#include "Rendering/Water/Water.h"
#include "Rendering/PostProcessing/PostProcessChain.h"
#include "Rendering/GPU Particles/High-level GPU Particles/HGPComponentManager.h"
#include "Rendering/GUI/TextRenderer.h"


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
    void InitializeObjects(void);

	void RenderWorldGeometry(const RenderInfo & info);

    Water * water;
    Material * waterMat;
    std::unordered_map<RenderingChannels, DataLine> channels;

    Mesh gsMesh;
    Material * gsTestMat;
    UniformDictionary gsTestParams;

    HGPComponentManager particleManager;
    Mesh particleMesh;
    Material * particleMat;
    UniformDictionary particleParams;

	MovingCamera cam;
    RenderTargetManager manager;

    PostProcessChain * ppc;
    unsigned int worldRenderID;
    std::vector<std::shared_ptr<PostProcessEffect>> ppcChain;

    DrawingQuad * finalScreenQuad;
    std::unordered_map<RenderingChannels, DataLine> finalScreenMatChannels;
    Material * finalScreenMat;
    UniformDictionary finalScreenQuadParams;

    TextRenderer::FontSlot testFontSlot;

    MTexture waterNormalTex;
};