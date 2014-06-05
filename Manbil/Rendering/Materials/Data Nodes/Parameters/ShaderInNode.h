#pragma once

#include "../DataNode.h"


//Handles an identical shader input across different kinds of shaders.
class ShaderInNode : public DataNode
{
public:

    bool HasVertexInput(void) const { return vertexInputIndex >= 0; }
    bool HasGeometryInput(void) const { return geoInputIndex >= 0; }
    bool HasFragmentInput(void) const { return fragInputIndex >= 0; }





private:

    int vertexInputIndex, geoInputIndex, fragInputIndex;
};