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

    //TODO: Add "ViewProjMatName" and corresponding uniform -- useful for geometry shaders.
    static const std::string ElapsedTimeName,
                             WorldMatName, ViewMatName, ProjMatName, WVPMatName,
                             CameraPosName, CameraForwardName, CameraUpName, CameraSideName,
                             CameraZNearName, CameraZFarName, CameraWidthName, CameraHeightName, CameraFovName,
                             VertexInNameBase, VertexOutNameBase,
                             FragmentOutName;

    //Gets the vertex shader input declarations for vertices with the given attributes.
    static std::string GetVertexInputDeclarations(const VertexAttributes & attribs);
    //Gets the uniform declarations for a material with the given usage flags.
    static std::string GetUniformDeclarations(const MaterialUsageFlags & flags);

    //Gets everything that's needed in a vertex shader before the actual main function (or helper functions).
    static std::string GetVertexHeader(std::string outputDeclarations, const VertexAttributes & attribs, const MaterialUsageFlags & flags);
    //Gets everything that's needed in a geometry shader before the actual main function (or helper function).
    static std::string GetGeometryHeader(std::string outputDeclarations, PrimitiveTypes input, PrimitiveTypes output, unsigned int maxVertices, const MaterialUsageFlags & flags);
    //Gets everything that's needed in a fragment shader before the actual main function (or helper functions).
    static std::string GetFragmentHeader(std::string inputDeclarations, std::string outputDeclarations, const MaterialUsageFlags & flags);
};


//The different rendering passes.
//TODO: Change this (regular render pass, emissive render pass, surface normal render pass, etc).
enum RenderPasses
{
    //Render normals, depth, diffuse, etc. -- basic data, multiple render targets.
    BaseComponents = 0,
    //Render basic lighting, along with ambient occlusion -- two different render targets.
    //If ambient occlusion isn't enabled, the ambient occlusion render target won't be rendered to.
    CombineComponents,
    //Apply ambient occlusion. Obviously this pass is only done if ambient occlusion is done.
    ApplyOcclusion,

    //The total number of different render passes.
    Numb_Passes,
};