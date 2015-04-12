#pragma once

#include "OpenGLIncludes.h"


//Represents a 2D texture sampler.
struct UniformValueSampler2D
{
public:

    RenderObjHandle Texture;
    UniformLocation Location;
    std::string Name;

    UniformValueSampler2D(RenderObjHandle textureHandle, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), Texture(textureHandle) { }
    UniformValueSampler2D(std::string name = "") : Name(name), Location(-1), Texture(0) { }

    std::string GetDeclaration(void) const { return "uniform sampler2D " + Name + ";"; }
};


//Represents a 3D texture sampler.
struct UniformValueSampler3D
{
public:

    RenderObjHandle Texture;
    UniformLocation Location;
    std::string Name;

    UniformValueSampler3D(RenderObjHandle textureHandle, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), Texture(textureHandle) { }
    UniformValueSampler3D(std::string name = "") : Name(name), Location(-1), Texture(0) { }

    std::string GetDeclaration(void) const { return "uniform sampler3D " + Name + ";"; }
};


//Represents a cubemap texture sampler.
struct UniformValueSamplerCubemap
{
public:

    RenderObjHandle Texture;
    UniformLocation Location;
    std::string Name;

    UniformValueSamplerCubemap(RenderObjHandle textureHandle, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), Texture(textureHandle) { }
    UniformValueSamplerCubemap(std::string name = "") : Name(name), Location(-1), Texture(0) { }

    std::string GetDeclaration(void) const { return "uniform samplerCube " + Name + ";"; }
};