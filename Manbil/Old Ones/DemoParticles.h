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


    void Update(float elapsedTime);
    void Render(const RenderInfo& info);

    void Reset(void) { totalTime = 0.0f; }


private:

    void SetBasePos(Vector3f newPos);
    void SetElapsedTime(float newTime);


    std::unique_ptr<Material> oldOneAppearParticles, oldOneAmbientParticles;
    Mesh oldOneAppearMesh, oldOneAmbientMesh;
    UniformDictionary oldOneAppearParams, oldOneAmbientParams;

    MTexture2D smokeTex;

    FractalRenderer& oldOne;
    float totalTime = 0.0f;
};