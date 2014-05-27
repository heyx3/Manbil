#pragma once

#include <string>

#include "RenderInfo.h"
#include "RenderDataHandler.h"
#include "ShaderHandler.h"
#include "Rendering/Materials/MaterialData.h"
#include "Rendering/Texture Management/TextureChannels.h"
#include "Rendering/Materials/UniformCollections.h"
#include "RenderTarget.h"


class Mesh;


//Represents some kind of surface to be drawn on.
class Material
{
public:

    //Gets the vertex data for this material.
    const VertexAttributes GetAttributeData(void) const { return attributes; }


    Material(const Material & cpy); //Intentionally not implemented.

    //Geometry shader is optional.
    Material(const std::string & vShader, const std::string & fShader, UniformDictionary & uniforms,
             const VertexAttributes & attributes, RenderingModes mode, bool isLit, LightSettings lightSettings,
             std::string geometryShader = "");
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

    //Returns whether the render was successful.
    bool Render(RenderPasses pass, const RenderInfo & info, const std::vector<const Mesh*> & meshes, const UniformDictionary & unifDict);


private:

    void RenderBaseComponents(const RenderInfo & info, const std::vector<const Mesh*> & meshes);
    void RenderCombineComponents(const RenderInfo & info, const std::vector<const Mesh*> & meshes);
    void RenderApplyOcclusion(const RenderInfo & info, const std::vector<const Mesh*> & meshes);


    std::string errorMsg;

    bool isLit;
    LightSettings lightSettings;
    RenderingModes mode;

    UniformList uniforms;
    VertexAttributes attributes;

    RenderObjHandle shaderProg;

    UniformLocation camPosL, camForwardL, camUpL, camSideL,
                    camZNearL, camZFarL, camWidthL, camHeightL, camFovL,
                    wvpMatL, worldMatL, viewMatL, projMatL,
                    timeL;
};