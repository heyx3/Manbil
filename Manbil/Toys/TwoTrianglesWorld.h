#pragma once

#include "../Game Loop/SFMLOpenGLWorld.h"
#include "../Rendering/Basic Rendering/Material.h"
#include "../Rendering/Basic Rendering/MaterialConstants.h"
#include "../Rendering/Primitives/DrawingQuad.h"


//TODO: Move TwoTriangles files into "Toys" folder to match filter.

//A world that draws a quad using a fragment shader specified by the user.
//Refer to "QuadWorld shader constants.txt" for more info.
class TwoTrianglesWorld : public SFMLOpenGLWorld
{
public:

    static const std::string CustomSamplerName, NoiseSamplerName, ShaderElapsedName;

	TwoTrianglesWorld(void);

protected:

	virtual void InitializeWorld(void) override;

	virtual void UpdateWorld(float elapsedSeconds) override;
	virtual void RenderOpenGL(float elapsedSeconds) override;

	virtual void OnInitializeError(std::string errorMsg) override;

	virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;

    virtual void OnWorldEnd(void) override;

	virtual void OnOtherWindowEvent(sf::Event& event) override;
};