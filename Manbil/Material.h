#pragma once

#include <string>

#include "RenderInfo.h"
#include "Rendering/Basic Rendering/BlendMode.h"
#include "Rendering/Texture Management/TextureChannels.h"
#include "Rendering/Basic Rendering/RenderIOAttributes.h"
#include "Rendering/Materials/UniformCollections.h"
#include "RenderTarget.h"



class Mesh;


//Represents some kind of surface to be drawn on.
class Material
{
public:

    //An invalid pointer to the location of a uniform in shader code.
    static const UniformLocation INVALID_UNIFORM_LOCATION;

    //The maximum number of textures that can be bound at once.
    static unsigned int GetMaxTextureUnits(void);


    //If an error occurred, outputs the error into the given error message string.
    //Otherwise, that string will be left alone.
    Material(const std::string& vShader, const std::string& fShader, UniformDictionary& uniforms,
             const RenderIOAttributes& attributes, BlendMode mode, std::string& outError,
             std::string geometryShader = "");
    ~Material(void);


    const RenderIOAttributes GetExpectedVertexData(void) const { return attributes; }
    const BlendMode& GetBlendMode(void) const { return mode; }
    const UniformList& GetUniforms(void) const { return uniforms; }

    //Gets the OpenGL handle to this material's shader program. Should generally never be needed.
    RenderObjHandle GetShaderProgram(void) const { return shaderProg; }

    void SetBlendMode(BlendMode newMode) { mode = newMode; }

    
    void Render(const RenderInfo& info, const Mesh* toRender, const UniformDictionary& params);
    void Render(const RenderInfo& info, const std::vector<const Mesh*>& meshes,
                const UniformDictionary& params);
    void Render(const RenderInfo& info, const UniformDictionary& params,
                const Mesh*const* meshPtrArray, unsigned int nMeshes);


private:

    void RenderBaseComponents(const RenderInfo& info, const std::vector<const Mesh*>& meshes);
    void RenderCombineComponents(const RenderInfo& info, const std::vector<const Mesh*>& meshes);
    void RenderApplyOcclusion(const RenderInfo& info, const std::vector<const Mesh*>& meshes);


    Material(const Material& cpy) = delete;


    BlendMode mode;

    UniformList uniforms;
    RenderIOAttributes attributes;

    RenderObjHandle shaderProg;

    UniformLocation camPosL, camForwardL, camUpL, camSideL,
                    camZNearL, camZFarL, camWidthL, camHeightL, camFovL,
                    camOrthoMinL, camOrthoMaxL,
                    wvpMatL, worldMatL, viewMatL, projMatL, viewProjMatL,
                    timeL;

    //All subroutine uniforms in a shader have to be set at once,
    //    so keep track of the order they will be organized into.
    std::vector<UniformLocation> vertexShaderSubroutines,
                                 geometryShaderSubroutines,
                                 fragmentShaderSubroutines;
    std::vector<RenderObjHandle> vertexShaderSubroutineValues,
                                 geometryShaderSubroutineValues,
                                 fragmentShaderSubroutineValues;
    

    static RenderObjHandle CreateShader(RenderObjHandle shaderProg, const GLchar* shaderText,
                                        Shaders shaderType, std::string& outErrorMsg);

    static UniformLocation GetUniformLoc(RenderObjHandle shaderProgram, const GLchar* name),
                           GetSubroutineUniformLoc(RenderObjHandle shaderProgram, Shaders shaderType,
                                                   const GLchar* name);
    static RenderObjHandle GetSubroutineID(RenderObjHandle shaderProgram, Shaders shaderType,
                                           const GLchar* name);

    static void SetUniformValueF(UniformLocation loc, unsigned int nComponents, const float* components),
                SetUniformValueArrayF(UniformLocation loc, unsigned int nArrayElements,
                                      unsigned int nComponents, const float* elements),
                SetUniformValueI(UniformLocation loc, unsigned int nComponents, const int* components),
                SetUniformValueArrayI(UniformLocation loc, unsigned int nArrayElements,
                                      unsigned int nComponents, const int* elements),
                SetUniformValueMatrix4f(UniformLocation loc, const Matrix4f& mat);
    //The subroutine function has to be a member function because subroutines are weird as hell.
    void SetUniformValueSubroutine(Shaders shaderType, RenderObjHandle* valuesForAllSubroutines);

    static void ActivateTextureUnit(unsigned int unitIndex);

    static unsigned int maxTexUnits; 
};