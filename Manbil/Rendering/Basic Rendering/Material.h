#pragma once

#include <string>

#include "RenderInfo.h"
#include "BlendMode.h"
#include "RenderIOAttributes.h"
#include "UniformCollections.h"
#include "Mesh.h"


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

    
    //Renders the given mesh.
    void Render(const RenderInfo& info, const Mesh* toRender, const UniformDictionary& params);
    //Renders the given meshes.
    void Render(const RenderInfo& info, const std::vector<const Mesh*>& meshes,
                const UniformDictionary& params);
    //Renders the given meshes.
    void Render(const RenderInfo& info, const UniformDictionary& params,
                const Mesh*const* meshPtrArray, unsigned int nMeshes);

    //Renders the given vertices with the given world matrix.
    void Render(const RenderInfo& info, const MeshData& toRender,
                const Matrix4f& worldMat, const UniformDictionary& params);
    //Renders the given vertex buffers with the given world matrices.
    void Render(const RenderInfo& info, const MeshData* toRender, const Matrix4f* worldMats,
                unsigned int nToRender, const UniformDictionary& params);


private:


    void SetUniforms(const UniformDictionary& params);


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
    static void SetUniformValueSubroutine(Shaders shaderType, unsigned int nValues,
                                          RenderObjHandle* valuesForAllSubroutines);

    static void ActivateTextureUnit(unsigned int unitIndex);

    static unsigned int maxTexUnits; 
};