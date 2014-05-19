#pragma once

#include <string>
#include <iostream>

#include "Math/LowerMath.hpp"
#include "Math/Shapes.hpp"


//Helpful debugging stuff.
struct DebugAssist
{
public:

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

    static std::string ToString(Shape::RayTraceResult res) { return (!res.DidHitTarget ? "{ Didn't hit anything }" : std::string() + "{ Hit pos: " + ToString(res.HitPos) + "; Hit normal: " + ToString(res.ReflectNormal) + "; t: " + std::to_string(res.HitT)) + " }"; }

    static std::string ToString(Box2D box) { return std::string() + "[ A box from " + ToString(box.GetMinCorner()) + " to " + ToString(box.GetMaxCorner()) + "]"; }
    static std::string ToString(Box3D box) { return std::string() + "[ A box from " + ToString(box.GetMinCorner()) + " to " + ToString(box.GetMaxCorner()) + "]"; }


    static void PauseConsole(std::string toPrint = "Enter any character to continue") { std::cout << toPrint; char dummy; std::cin >> dummy; }
};