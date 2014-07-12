#pragma once

#include <string>
#include <iostream>
#include <assert.h>

#include "Math/LowerMath.hpp"
#include "Math/Shapes.hpp"
#include "Rendering/GPU Particles/GPUParticleDefines.h"
#include "Rendering/Texture Management/TextureSettings.h"



//Helpful debugging stuff.
//TODO: Move other ToString() stuff into here, such as VectorF, VectorI, various Uniform value structs, and various enums.
struct DebugAssist
{
public:

    //A global debut output string so that debug outputs don't have to be passed between functions.
    static std::string STR;


    static std::string ToString(Vector2b v) { return std::string() + "{ " + std::to_string(v.x) + ", " + std::to_string(v.y) + " }"; }
    static std::string ToString(Vector3b v) { return std::string() + "{ " + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + " }"; }
    static std::string ToString(Vector4b v) { return std::string() + "{ " + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + " }"; }

    static std::string ToString(Vector2i v) { return std::string() + "{ " + std::to_string(v.x) + ", " + std::to_string(v.y) + " }"; }
    static std::string ToString(Vector3i v) { return std::string() + "{ " + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + " }"; }
    static std::string ToString(Vector4i v) { return std::string() + "{ " + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + " }"; }

    static std::string ToString(Vector2f v) { return std::string() + "{ " + std::to_string(v.x) + ", " + std::to_string(v.y) + " }"; }
    static std::string ToString(Vector3f v) { return std::string() + "{ " + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + " }"; }
    static std::string ToString(Vector4f v) { return std::string() + "{ " + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + " }"; }

    static std::string ToString(Quaternion q) { return std::string() + "{ (" + std::to_string(q.x) + ", " + std::to_string(q.y) + ", " + std::to_string(q.z) + "), " + std::to_string(q.w) + " }"; }

    static std::string ToString(Interval i) { return std::string() + "[" + std::to_string(i.GetStart()) + ", " + std::to_string(i.GetEnd()) + "]"; }

    static std::string ToString(Shape::RayTraceResult res)
    {
        return (!res.DidHitTarget ?
                    "{ Didn't hit anything }" :
                    std::string() + "{ Hit pos: " + ToString(res.HitPos) + "; Hit normal: " + ToString(res.ReflectNormal) + "; t: " + std::to_string(res.HitT)) + " }"; }

    static std::string ToString(Box2D box) { return std::string() + "[ A box from " + ToString(box.GetMinCorner()) + " to " + ToString(box.GetMaxCorner()) + "]"; }
    static std::string ToString(Box3D box) { return std::string() + "[ A box from " + ToString(box.GetMinCorner()) + " to " + ToString(box.GetMaxCorner()) + "]"; }

    static std::string ToString(PixelSizes pixelSize)
    {
        switch (pixelSize)
        {
            case PS_8U: return "RGBA_8U";
            case PS_16U: return "RGBA_16U";
            case PS_16F: return "RGBA_16F";
            case PS_32F: return "RGBA_32F";

            case PS_8U_GREYSCALE: return "R_8U";
            case PS_16U_GREYSCALE: return "R_16U";
            case PS_32F_GREYSCALE: return "R_32F";

            case PS_16U_DEPTH: return "Depth_16U";
            case PS_24U_DEPTH: return "Depth_24U";
            case PS_32F_DEPTH: return "Depth_32F";

            default: assert(false); return "ERROR_UNKNOWN_SIZE:" + std::to_string(pixelSize);
        }
    }

    static std::string ToString(GPUPOutputs output)
    {
        switch (output)
        {
            case GPUPOutputs::GPUP_WORLDPOSITION: return "GPUP_WORLDPOSITION";
            case GPUPOutputs::GPUP_SIZE: return "GPUP_SIZE";
            case GPUPOutputs::GPUP_QUADROTATION: return "GPUP_QUADROTATION";
            case GPUPOutputs::GPUP_COLOR: return "GPUP_COLOR";
            default: assert(false); return std::string() + "GPUP_ERROR_UNKNOWN_VALUE_OF_" + std::to_string(output);
        }
    }


    static void PauseConsole(std::string toPrint = "Enter any character to continue") { std::cout << toPrint; char dummy; std::cin >> dummy; }
};