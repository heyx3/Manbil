#pragma once

#include <string>
#include <memory>

#include "../Rendering/Rendering.hpp"
#include "FractalRenderer.h"


//Manages all the particle effects in the demo.
class DemoParticles
{
public:

    DemoParticles(FractalRenderer& oldOne, std::string& errorMsg);
    ~DemoParticles(void);


    void Update(float elapsedTime, float totalTime);
    void Render(const RenderInfo& info);


private:

    void SetBasePos(Vector3f newPos);
    void SetElapsedTime(float newTime);


    std::shared_ptr<Material> oldOneAppearParticles, oldOneAmbientParticles;
    Mesh oldOneAppearMesh, oldOneAmbientMesh;
    UniformDictionary oldOneAppearParams, oldOneAmbientParams;

    MTexture2D smokeTex;

    FractalRenderer& oldOne;
    float totalTime;
};