#pragma once

#include <string>

#include "RenderInfo.h"
#include "RenderDataHandler.h"
#include "ShaderHandler.h"
#include "Vertex.h"
#include "Rendering/Materials/MaterialData.h"
#include "Rendering/Texture Management/TextureChannels.h"
#include "Rendering/Materials/UniformCollections.h"
#include "RenderTarget.h"


class Mesh;


//Represents some kind of surface to be drawn on.
class Material
{
public:

    Material(const Material & cpy); //Intentionally not implemented.

    Material(const std::string & vShader, const std::string & fShader, UniformDictionary & uniforms,
             RenderingModes mode, bool isLit, LightSettings lightSettings);
    ~Material(void) { glDeleteProgram(shaderProg); ClearAllRenderingErrors(); }


    RenderObjHandle GetShaderProgram(void) const { return shaderProg; }

    bool HasError(void) const { return !errorMsg.empty(); }
    std::string GetErrorMsg(void) const { return errorMsg; }
    void ClearErrorMsg(void) { errorMsg.clear(); }

    RenderingModes GetMode(void) const { return mode; }
    bool GetIsLit(void) const { return isLit; }
    const LightSettings & GetLightSettings(void) const { return lightSettings; }

    const UniformList & GetUniforms(RenderPasses pass) const { return uniforms; }

    int GetHashCode(void) const { return shaderProg; }

    //Returns whether thhe render was successful.
    bool Render(RenderPasses pass, const RenderInfo & info, const std::vector<const Mesh*> & meshes);


private:

    void RenderBaseComponents(const RenderInfo & info, const std::vector<const Mesh*> & meshes);
    void RenderCombineComponents(const RenderInfo & info, const std::vector<const Mesh*> & meshes);
    void RenderApplyOcclusion(const RenderInfo & info, const std::vector<const Mesh*> & meshes);


    std::string errorMsg;

    bool isLit;
    LightSettings lightSettings;
    RenderingModes mode;

    UniformList uniforms;

    RenderObjHandle shaderProg;

    UniformLocation camPosL, camForwardL, camUpL, camSideL,
                    camZNearL, camZFarL, camWidthL, camHeightL, camFovL,
                    wvpMatL, worldMatL, viewMatL, projMatL,
                    timeL;
};