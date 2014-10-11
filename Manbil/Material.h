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
    const ShaderInOutAttributes GetAttributeData(void) const { return attributes; }


    Material(const Material & cpy); //Intentionally not implemented.

    //Geometry shader is optional.
    Material(const std::string & vShader, const std::string & fShader, UniformDictionary & uniforms,
             const ShaderInOutAttributes & attributes, RenderingModes mode,
             std::string geometryShader = "");
    ~Material(void) { glDeleteProgram(shaderProg); ClearAllRenderingErrors(); }


    RenderObjHandle GetShaderProgram(void) const { return shaderProg; }

    bool HasError(void) const { return !errorMsg.empty(); }
    std::string GetErrorMsg(void) const { return errorMsg; }
    void ClearErrorMsg(void) { errorMsg.clear(); }

    RenderingModes GetMode(void) const { return mode; }

#pragma warning(disable: 4100)
    const UniformList & GetUniforms(void) const { return uniforms; }
#pragma warning(default: 4100)

    int GetHashCode(void) const { return shaderProg; }

    //Returns whether the render was successful.
    bool Render(const RenderInfo & info, const std::vector<const Mesh*> & meshes, const UniformDictionary & unifDict);


private:

    void RenderBaseComponents(const RenderInfo & info, const std::vector<const Mesh*> & meshes);
    void RenderCombineComponents(const RenderInfo & info, const std::vector<const Mesh*> & meshes);
    void RenderApplyOcclusion(const RenderInfo & info, const std::vector<const Mesh*> & meshes);


    std::string errorMsg;

    RenderingModes mode;

    UniformList uniforms;
    ShaderInOutAttributes attributes;

    RenderObjHandle shaderProg;

    UniformLocation camPosL, camForwardL, camUpL, camSideL,
                    camZNearL, camZFarL, camWidthL, camHeightL, camFovL,
                    camOrthoMinL, camOrthoMaxL,
                    wvpMatL, worldMatL, viewMatL, projMatL, viewProjMatL,
                    timeL;
};