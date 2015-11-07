#pragma once

#include "../Game Loop/SFMLOpenGLWorld.h"
#include "../Input/MovingCamera.h"
#include "../Rendering/Rendering.hpp"


//Shows an example of serializing/deserializing data to/from a file.
//The file contains vertex data for the world geometry and the shaders used to render it.
class SerializationWorld : public SFMLOpenGLWorld
{
public:

    SerializationWorld(void);


protected:

    virtual sf::VideoMode GetModeToUse(unsigned int windowW, unsigned int windowH) override;
    virtual std::string GetWindowTitle(void) override;
    virtual sf::Uint32 GetSFStyleFlags(void) override;

    virtual void InitializeWorld(void) override;
    virtual void OnWorldEnd(void) override;

    virtual void OnInitializeError(std::string errorMsg) override;
    virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;

    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;


private:

    Vector2u windowSize;

    MovingCamera gameCam;

    Mesh mesh;
    Material* material;
    UniformDictionary params;
};