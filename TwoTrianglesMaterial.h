#pragma once

#include "Material.h"
#include <string>
#include <assert.h>
#include "MovingCamera.h"

#include "RenderDataHandler.h"

typedef std::string string;


/*
   Takes in a fragment shader (without any of the setup stuff before the "main" function, like declaring uniforms)
    that is intended to operate on a simple quad.
   Handles much of the fragment shader itself, including setup and uniforms. The fragment shader that is passed into
    the constructor should only contain the "main" function and any functions "main" will call.
   For example, the simplest-possible string to pass into this class's constructor is:
	string("void main() \n\
			{			\n") +
				TwoTrianglesMaterial::Out_Color + " = vec4(1.0, 1.0, 1.0, 1.0); \n\
			}"
*/
class TwoTrianglesMaterial
{
public:

	typedef RenderDataHandler DH;

	static const unsigned int AvailableSamplers = 4,
							  NoiseSampler = 0,
							  FileSampler = 1;

	static const string Uniform_Time, Uniform_Resolution,
						Uniform_Sampler[AvailableSamplers],
						Uniform_MousePos,
						Uniform_CamPos,
						Uniform_CamForward, Uniform_CamUp, Uniform_CamSideways,
						In_UV, Out_Color;


	TwoTrianglesMaterial(string fragmentShader);
	~TwoTrianglesMaterial(void) { glDeleteProgram(shaderProg); glDeleteBuffers(1, &vbo); }
	
	
	bool HasError(void) const { return !errorMsg.empty(); }
	const char * GetErrorMessage(void) const { return errorMsg.c_str(); }
	void ClearErrorMessage(void) const { errorMsg.clear(); }


	BufferObjHandle GetTexture(unsigned int texValue) { assert(texValue < AvailableSamplers); return textures[texValue]; }
	void SetTexture(unsigned int tex, BufferObjHandle texObj) { assert(tex < AvailableSamplers); textures[tex] = texObj; }


	//Each uniform setter function returns whether or not it succeeded.

	bool SetElapsedTime(float newElapsedTime) const { if (!UsesTime()) return true; glUniform1f(timeLoc, newElapsedTime); return CheckError("Error setting 'elapsed time' uniform"); }
	bool SetMousePos(Vector2f newPos) const { if (!UsesMousePos()) return true; glUniform2f(mPosLoc, newPos.x, newPos.y); return CheckError("Error setting 'mouse pos' uniform"); }
	bool SetResolution(Vector2f resolution) const { if (!UsesResolution()) return true; glUniform2f(resLoc, resolution.x, resolution.y); return CheckError("Error setting 'resolution' uniform"); }
	bool SetSamplerTextureUnit(unsigned int unit, unsigned int sampler) { if (!UsesSampler(sampler)) return true; assert(sampler < AvailableSamplers); glUniform1i(samplerLocs[sampler], unit); samplerTexUnits[sampler] = unit; return CheckError("Error setting a sampler"); }
	bool SetCamera(const MovingCamera & cam) const;

	bool UsesTime(void) const { return DH::UniformLocIsValid(timeLoc); }
	bool UsesMousePos(void) const { return DH::UniformLocIsValid(mPosLoc); }
	bool UsesResolution(void) const { return DH::UniformLocIsValid(resLoc); }
	bool UsesSampler(unsigned int sampler) const { return DH::UniformLocIsValid(samplerLocs[sampler]); }
	bool UsesCameraPos(void) const { return DH::UniformLocIsValid(camPosLoc); }
	bool UsesCameraForward(void) const { return DH::UniformLocIsValid(camFLoc); }
	bool UsesCameraSide(void) const { return DH::UniformLocIsValid(camSLoc); }
	bool UsesCameraUp(void) const { return DH::UniformLocIsValid(camULoc); }
	
	//Returns whether or not the render succeeded.
	bool Render(void) const;

private:

	BufferObjHandle vbo;

	bool CheckError(const char * errorIntro) const;

	mutable std::string errorMsg;

	BufferObjHandle shaderProg;

	UniformLocation timeLoc, mPosLoc, resLoc, samplerLocs[AvailableSamplers],
					camPosLoc, camFLoc, camULoc, camSLoc;

	unsigned int samplerTexUnits[AvailableSamplers];
	BufferObjHandle textures[AvailableSamplers];
};