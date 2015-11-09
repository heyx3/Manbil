#pragma once

#include <SFML/Graphics.hpp>
#include "../Game Loop/SFMLWorld.h"
#include "../Math/Lower Math/Array2D.h"


//Tests random noise generation. Uses mostly SFML stuff instead of Manbil's rendering stuff.
class NoiseTest : public SFMLWorld
{
public:

	NoiseTest(void)
        : SFMLWorld(500, 500),
          renderedNoise(0), renderedNoiseTex(0) { }

protected:
    
    virtual sf::ContextSettings GenerateContext(void) override;

	virtual void InitializeWorld(void) override;

	virtual void UpdateWorld(float elapsedSeconds) override;
	virtual void RenderWorld(float elapsedSeconds) override;

	virtual void OnWorldEnd(void) override;

private:

	void ReGenerateNoise(bool newSeeds);
	void InterpretNoise(const Array2D<float>& noise);
	

	sf::Texture* renderedNoiseTex;
	sf::Sprite* renderedNoise;
    sf::Font guiFont;

    FastRand fr = FastRand(255);

    float bumpHeight = 1.0f;
    bool pressedLast = false;
};