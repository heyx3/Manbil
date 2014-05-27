#include "MaterialData.h"

#include <assert.h>


const std::string MaterialConstants::ElapsedTimeName = "u_elapsed_seconds",
                  MaterialConstants::WorldMatName = "u_matWorld",
                  MaterialConstants::ViewMatName = "u_matView",
                  MaterialConstants::ProjMatName = "u_matProj",
                  MaterialConstants::WVPMatName = "u_matWVP",
                  MaterialConstants::CameraPosName = "u_cam_pos",
                  MaterialConstants::CameraForwardName = "u_cam_forward",
                  MaterialConstants::CameraUpName = "u_cam_upward",
                  MaterialConstants::CameraSideName = "u_cam_sideways",
                  MaterialConstants::CameraWidthName = "u_cam_width",
                  MaterialConstants::CameraHeightName = "u_cam_height",
                  MaterialConstants::CameraZNearName = "u_cam_zNear",
                  MaterialConstants::CameraZFarName = "u_cam_zFar",
                  MaterialConstants::CameraFovName = "u_cam_fov",

                  MaterialConstants::VertexInNameBase = "in_",
                  MaterialConstants::VertexOutNameBase = "out_",

                  MaterialConstants::FragmentOutName = "FinalOutput";

std::string MaterialConstants::GetVertexInputDeclarations(const VertexAttributes & attribs)
{
    std::string output;
    unsigned int nAttribs = attribs.GetNumbAttributes();
    for (unsigned int i = 0; i < nAttribs; ++i)
        output += "layout (location = " + std::to_string(i) + ") in " + VectorF((unsigned int)attribs.GetAttributeSize(i)).GetGLSLType() + " " + VertexInNameBase + std::to_string(i) + ";\n";
    return output;
}
RenderingState MaterialConstants::GetRenderingState(RenderingModes mode)
{
    switch (mode)
    {
        case RenderingModes::RM_Opaque:
            return RenderingState();
        case RenderingModes::RM_Transluscent:
            //TODO: I think transluscent shouldn't write to the depth buffer, because then it removes the need for depth sorting? Test this.
            return RenderingState(RenderingState::Cullables::C_NONE);
        case RenderingModes::RM_Additive:
            return RenderingState(RenderingState::C_NONE, RenderingState::BE_ONE, RenderingState::BE_ONE, true, false);

        default:
            assert(false);
            return RenderingState();
    }
}
std::string MaterialConstants::GetUniformDeclarations(const MaterialUsageFlags & flags)
{
    typedef MaterialUsageFlags::Flags FL;

    std::string uniformDecls = (flags.GetFlag(FL::DNF_USES_TIME) ? "uniform float " + ElapsedTimeName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_CAM_POS) ? "uniform vec3 " + CameraPosName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_CAM_FORWARD) ? "uniform vec3 " + CameraForwardName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_CAM_UPWARDS) ? "uniform vec3 " + CameraUpName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_CAM_SIDEWAYS) ? "uniform vec3 " + CameraSideName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_WIDTH) ? "uniform float " + CameraWidthName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_HEIGHT) ? "uniform float " + CameraHeightName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_ZNEAR) ? "uniform float " + CameraZNearName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_ZFAR) ? "uniform float " + CameraZFarName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_FOV) ? "uniform float " + CameraFovName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_WORLD_MAT) ? "uniform mat4 " + WorldMatName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_VIEW_MAT) ? "uniform mat4 " + ViewMatName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_PROJ_MAT) ? "uniform mat4 " + ProjMatName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_WVP_MAT) ? "uniform mat4 " + WVPMatName + ";\n" : "");
    if (!uniformDecls.empty())
        uniformDecls = std::string() + "//Uniforms.\n" + uniformDecls + "\n\n\n";

    return uniformDecls;
}

std::string MaterialConstants::GetVertexHeader(std::string outputDeclarations, const VertexAttributes & attribs, const MaterialUsageFlags & flags)
{
    return std::string() +
"#version 330                                                    \n\
                                                                 \n\
" + GetVertexInputDeclarations(attribs) + "                      \n\
                                                                 \n\
" + outputDeclarations + "                                       \n\
                                                                 \n\
" + GetUniformDeclarations(flags);
}
std::string MaterialConstants::GetGeometryHeader(std::string outputDeclarations, PrimitiveTypes input, PrimitiveTypes output, unsigned int maxVertices, const MaterialUsageFlags & flags)
{
    return std::string() +
"#version 330                                                   \n\
                                                                \n\
layout (" + PrimitiveTypeToGSInput(input) + ") in;              \n\
layout (" + PrimitiveTypeToGSOutput(output) + ") out;           \n\
layout (max_vertices = " + std::to_string(maxVertices) + ") out;\n\
                                                                \n\
" + outputDeclarations + "                                      \n\
                                                                \n\
" + GetUniformDeclarations(flags);
}
std::string MaterialConstants::GetFragmentHeader(std::string inputDeclarations, std::string outputDeclarations, const MaterialUsageFlags & flags)
{
    return std::string() +
"#version 330                                                    \n\
                                                                 \n\
" + inputDeclarations + "                                        \n\
                                                                 \n\
" + outputDeclarations + "                                       \n\
                                                                 \n\
" + GetUniformDeclarations(flags);
}