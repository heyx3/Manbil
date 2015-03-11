#pragma once

#include "../Basic Rendering/RenderIOAttributes.h"
#include "../../Math/Lower Math/Vectors.h"


//TODO: Add similar "SplineCurve" and "SmoothedCurve" (i.e. smoothstep/supersmoothstep) classes.

//The vertex used for Bezier curves.
struct CurveVertex
{
public:

    //Ranges from {0, -1} to {1, 1}.
    //Represents where on the line this point is.
    //X is line length [0, 1], Y is line thickness [-1, 1]).
    Vector2f LinePosLerp;

    CurveVertex(Vector2f linePosLerp = Vector2f()) : LinePosLerp(linePosLerp) { }

    static RenderIOAttributes GetVertexAttributes(void);
    static void GenerateVertices(std::vector<CurveVertex>& outVertices, unsigned int nLineSegments);
};