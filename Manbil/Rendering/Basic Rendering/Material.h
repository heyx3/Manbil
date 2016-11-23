#pragma once

#include <string>

#include "../../Math/Higher Math/Transform.h"
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


	//Gets the attributes that mesh vertices should have when using this material.
    const RenderIOAttributes GetVetexAttributes(void) const { return attributes; }

	//Gets the custom uniforms this material has.
    const UniformList& GetUniforms(void) const { return uniforms; }
	//Gets the handle of the given custom uniform.
	UniformLocation GetUniformLoc(const std::string& name) const;

    //Gets the OpenGL handle to this material's shader program.
    RenderObjHandle GetShaderProgram(void) const { return shaderProg; }

    void SetBlendMode(BlendMode newMode) { mode = newMode; }
    const BlendMode& GetBlendMode(void) const { return mode; }

	void Render(const Mesh& mesh, const Transform& transform,
				const RenderInfo& cameraInfo, const UniformDictionary& params) const;


private:


    void SetUniforms(const UniformDictionary& params) const;


    Material(const Material& cpy) = delete;


    BlendMode mode;

    UniformList uniforms;
    RenderIOAttributes attributes;

    RenderObjHandle shaderProg;

	//The location of important, built-in uniforms.
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
    mutable std::vector<RenderObjHandle> vertexShaderSubroutineValues,
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
                SetUniformValueMatrix4f(UniformLocation loc, const Matrix4f& mat),
				SetUniformValueSubroutine(Shaders shaderType, unsigned int nValues,
                                          const RenderObjHandle* valuesForAllSubroutines);

    static void ActivateTextureUnit(unsigned int unitIndex);

    static unsigned int maxTexUnits; 
};