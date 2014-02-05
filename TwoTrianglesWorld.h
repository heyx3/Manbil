#pragma once

#include "sfmlopenglworld.h"
#include "TwoTrianglesMaterial.h"

//TODO: Rewrite all the rendering to use the new rendering system.

class TwoTrianglesWorld : public SFMLOpenGLWorld
{
public:

	TwoTrianglesWorld(void);
	~TwoTrianglesWorld(void) { DeleteAndSetToNull(mat); }

protected:

	virtual void InitializeWorld(void) override;

	virtual void UpdateWorld(float elapsedSeconds) override;
	virtual void RenderWorld(float elapsedSeconds) override;

	virtual void OnInitializeError(std::string errorMsg) override;

	virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;

	virtual void OnWorldEnd(void) override;

	virtual void OnOtherWindowEvent(sf::Event & event) override;

private:

	TwoTrianglesMaterial * mat;
};