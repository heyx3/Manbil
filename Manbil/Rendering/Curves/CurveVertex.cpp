#include "CurveVertex.h"

#include <assert.h>


void CurveVertex::GenerateVertices(std::vector<CurveVertex> & outVerts, unsigned int nLineSegments)
{
    assert(nLineSegments > 1);

    float increment = 1.0f / (float)(nLineSegments - 1);
    for (unsigned int seg = 0; seg < nLineSegments; ++seg)
    {
        float x = (float)seg * increment;
        outVerts.insert(outVerts.end(), CurveVertex(Vector2f(x, -1.0f)));
        outVerts.insert(outVerts.end(), CurveVertex(Vector2f(x, 1.0f)));
    }
}