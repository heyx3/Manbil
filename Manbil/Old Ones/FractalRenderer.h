#pragma once

#include "../Rendering/Rendering.hpp"


//Renders the "Old One" fractal using distance-field raymarching.
//Stores the body of the fragment shader in a text file.
class FractalRenderer
{
public:

    static const float AppearTime;


    FractalRenderer(std::string& outError);
    ~FractalRenderer(void);
    

    //Recreates the material's fragment shader from the text file.
    void RegenerateMaterial(std::string& outErr);

    void Update(float frameSeconds, float totalSeconds);
    void Render(RenderInfo& info);
    
    Vector3f GetFractalPos(void) const;
    float GetFractalSize(void) const;
    float GetFractalPower(void) const;


private:

    void SetFractalPos(Vector3f newPos);
    void SetFractalSize(float newSize);
    void SetFractalPower(float newPow);


    Material* mat;
    UniformDictionary params;

    float totalTime = 0.0f;
    bool appeared = false;
};