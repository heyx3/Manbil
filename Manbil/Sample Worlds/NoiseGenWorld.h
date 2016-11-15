#pragma once

#include "../Game Loop/SFMLOpenGLWorld.h"
#include "../Rendering/GUI/GUI Elements/GUITexture.h"


//Allows the user to generate random noise and create a normal map from it.
//Use Space to re-generate the noise, Enter to convert it to a normalmap,
//    and Left/Right Arrows to change the slope of the normalmap.
class NoiseGenWorld : public SFMLOpenGLWorld
{
public:

    NoiseGenWorld(void);


protected:

    virtual sf::VideoMode GetModeToUse(unsigned int windowW, unsigned int windowH) override;
    virtual std::string GetWindowTitle(void) override;
    virtual sf::Uint32 GetSFStyleFlags(void) override;
    virtual sf::ContextSettings GenerateContext(void) override;

    virtual void InitializeWorld(void) override;
    virtual void OnWorldEnd(void);
    
    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;

    virtual void OnInitializeError(std::string errorMsg) override;
    virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;


private:

    void GenerateNoise(MTexture2D& outNoiseTex);
    void GenerateBumpMap(MTexture2D& outBumpTex);
    void GenerateGUI();

    //If the given value is "false", prints the given error message and ends the world.
    //Returns the given value.
    bool Assert(bool expr, const char* errIntro, std::string& err);


    Vector2u windowSize;

    Camera cam;
    GUITexture guiTexGreyscale, guiTexColor;

    FastRand rng;
    MTexture2D noiseTex;
    float bumpmapHeight = 20.0f;
};