#pragma once

#include <string>
#include "../../RenderingState.h"
#include "RenderingModes.h"
#include "../../ShaderHandler.h"
#include "UniformCollections.h"
#include "MaterialUsageFlags.h"
#include "../../Vertices.h"



//Constants/information used for the Material system.
struct MaterialConstants
{
public:
    static RenderingState GetRenderingState(RenderingModes mode);

    static const std::string ElapsedTimeName,
                             WorldMatName, ViewMatName, ProjMatName, ViewProjMatName, WVPMatName,
                             CameraPosName, CameraForwardName, CameraUpName, CameraSideName,
                             CameraZNearName, CameraZFarName, CameraWidthName, CameraHeightName, CameraFovName;

    //Gets whether the given string is a valid GLSL variable name.
    //Fails if there is any whitespace on either side of the name.
    static bool IsValidGLSLName(const std::string & name);

    //Gets the vertex shader input declarations for vertices with the given attributes.
    static std::string GetVertexInputDeclarations(const ShaderInOutAttributes & attribs);
    //Gets the uniform declarations for a material with the given usage flags.
    static std::string GetUniformDeclarations(const MaterialUsageFlags & flags);

    //Gets everything that's needed in a vertex shader before the actual main function (or helper functions).
    static std::string GetVertexHeader(std::string outputDeclarations, const ShaderInOutAttributes & attribs, const MaterialUsageFlags & flags);
    //Gets everything that's needed in a geometry shader before the actual main function (or helper function).
    static std::string GetGeometryHeader(std::string outputDeclarations, PrimitiveTypes input, PrimitiveTypes output, unsigned int maxVertices, const MaterialUsageFlags & flags);
    //Gets everything that's needed in a fragment shader before the actual main function (or helper functions).
    static std::string GetFragmentHeader(std::string inputDeclarations, std::string outputDeclarations, const MaterialUsageFlags & flags);
};