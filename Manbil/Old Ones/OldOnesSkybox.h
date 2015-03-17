#pragma once

#include "../Rendering/Rendering.hpp"


//The skybox for the Old Ones demo.
class OldOnesSkybox
{
public:

    //Outputs an error into the given string if something goes wrong in the construction.
    OldOnesSkybox(std::string& outError);
    ~OldOnesSkybox(void);


    void Render(RenderInfo& worldRenderInfo);


private:

    MTextureCubemap tex;
    Material* cubeMat;
    Mesh cubeMesh;
    UniformDictionary cubeParams;
};