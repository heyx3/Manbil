#pragma once

#include <SFML/Audio.hpp>
#include "../Rendering/Rendering.hpp"
#include "OldOneEditableData.h"


//Renders the "Old One" fractal using distance-field raymarching.
//Stores the body of the fragment shader in a text file.
//Has two different materials: a low-quality one for shadow maps
//    and a high-quality one for the actual render.
class FractalRenderer
{
public:

    static const float AppearTime;


    FractalRenderer(std::string& outError);
    ~FractalRenderer(void);
    

    //Recreates the material's fragment shader from the text file.
    void RegenerateMaterial(std::string& outErr, bool shadowMat);

    void Update(const OldOneEditableData& data, float frameSeconds);
    void Render(const OldOneEditableData& data, bool shadowRender, RenderInfo& info);
    
    Vector3f GetFractalPos(void) const;
    float GetFractalSize(void) const;
    float GetFractalPower(void) const;

    bool IsEditable(void) const;

    //Resets this entity's timing stuff.
    void Reset(void) { totalTime = 0.0f; appeared = false; }


private:

    void SetFractalPos(Vector3f newPos);
    void SetFractalSize(float newSize);
    void SetFractalPower(float newPow);
    void SetFractalRoundness(float newVal);
    void SetFractalAngriness(float newVal);
    void SetFractalSpikyness(float left, float right);
    void SetFractalColor(Vector3f col1, Vector3f col2);


    Material *mat, *matShadow;
    UniformDictionary params;

    sf::Sound appearSound, ambientSound;
    sf::SoundBuffer appearSndBuff, ambientSndBuff;

    float totalTime = 0.0f;
    bool appeared = false;
};