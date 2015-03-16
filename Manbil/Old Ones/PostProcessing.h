#pragma once

#include "../Rendering/Rendering.hpp"


//Handles all the post-process effects.
class PostProcessing
{
public:

    PostProcessing(Vector2u windowSize, std::string& outError);
    ~PostProcessing(void);


    void Resize(Vector2u newWindowSize);

    //Renders post-processing effects given the rendered world.
    //Returns the handle to the final render texture.
    RenderObjHandle Render(float totalElapsedSeconds, ProjectionInfo projInfo,
                           RenderObjHandle worldColorTex, RenderObjHandle worldDepthTex);


private:

    RenderTarget *rt1, *rt2;
    MTexture2D col1, col2;

    std::vector<std::shared_ptr<Material>> ppMats;
    std::vector<UniformDictionary> ppParams;
};