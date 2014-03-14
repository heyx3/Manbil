#pragma once

#include "sfmlopenglworld.h"
#include "Material.h"
#include "Rendering/Materials/MaterialData.h"
#include "Rendering/Helper Classes/DrawingQuad.h"


//A world that draws a quad.
class TwoTrianglesWorld : public SFMLOpenGLWorld
{
public:

	TwoTrianglesWorld(void);
	~TwoTrianglesWorld(void) { DeleteAndSetToNull(mat); DeleteAndSetToNull(quad); }

protected:

	virtual void InitializeWorld(void) override;

	virtual void UpdateWorld(float elapsedSeconds) override;
	virtual void RenderWorld(float elapsedSeconds) override;

	virtual void OnInitializeError(std::string errorMsg) override;

	virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;

	virtual void OnWorldEnd(void) override;

	virtual void OnOtherWindowEvent(sf::Event & event) override;
};