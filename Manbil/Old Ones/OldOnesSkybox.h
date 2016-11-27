#pragma once

#include "../Rendering/Rendering.hpp"


//The skybox for the Old Ones demo.
class OldOnesSkybox
{
public:

    //Outputs an error into the given string if something goes wrong during construction.
    OldOnesSkybox(std::string& outError);


    void Render(RenderInfo& worldRenderInfo);


private:

    MTextureCubemap tex;
    std::unique_ptr<Material> cubeMat;
    Mesh cubeMesh;
    UniformDictionary cubeParams;
};