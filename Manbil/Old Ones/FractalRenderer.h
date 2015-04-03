#pragma once

#include "../Rendering/Rendering.hpp"


//Renders the "Old One" fractal using distance-field raymarching.
//Stores the body of the fragment shader in a text file.
class FractalRenderer
{
public:

    FractalRenderer(std::string& outError);
    ~FractalRenderer(void);
    
    //Recreates the material's fragment shader from the text file.
    void RegenerateMaterial(std::string& outErr);

    void Update(float frameSeconds);
    void Render(RenderInfo& info);


private:


    Vector3f GetFractalPos(void) const;
    float GetFractalSize(void) const;
    float GetFractalPower(void) const;

    void SetFractalPos(Vector3f newPos);
    void SetFractalSize(float newSize);
    void SetFractalPower(float newPow);


    Material* mat;
    UniformDictionary params;
};