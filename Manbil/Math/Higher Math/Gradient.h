#pragma once

#include <vector>

#include "../BasicMath.h"


//The number of different float values in a single point on the gradient.
template<unsigned int Components>
//Represents a single node in a gradient.
struct GradientNode
{
public:

    //TODO: Split "Value" into "LeftValue" and "RightValue" so that the gradient no longer has to be continuous.

    //The position of this node, from 0 to 1.
    float T;
    //The value at this node.
    float Value[Components];
    //The derivative of the gradient value at this node's position.
    float Slope[Components];

    GradientNode(float t, const float value[Components], const float slope[Components])
        : T(t)
    {
        for (unsigned int i = 0; i < Components; ++i)
        {
            Value[i] = value[i];
            Slope[i] = slope[i];
        }
    }
    GradientNode(const GradientNode & cpy) : GradientNode(cpy.T, cpy.Value, cpy.Slope) { }
};



//The number of different float values in a single point on the gradient.
template<unsigned int Components>
//Represents some kind of gradient for a value.
class Gradient
{
public:

    typedef GradientNode<Components> GNode;

    std::vector<GradientNode<Components>> Nodes;

    Gradient(GNode startVal, GNode endVal) : Nodes(MakeVector(startVal, endVal)) { }
    Gradient(GNode startVal, GNode endVal, GNode mid1) : Nodes(MakeVector(startVal, mid1, endVal)) { }
    Gradient(GNode startVal, GNode endVal, GNode mid1, GNode mid2)
        : Nodes(MakeVector(startVal, mid1, mid2, endVal))
    {

    }
    Gradient(const std::vector<GNode> & nodes) : Nodes(nodes) { }

    bool IsValidGradient(void) const { return Nodes.size() > 0; }

    //Clamps the given t value to the bounds of this gradient's nodes and gets the gradient value at that t.
    //Assumes that this gradient is valid.
    void GetValue(float t, float outVals[Components]) const
    {
        //Check edge-cases.
        if (Nodes.size() == 1 || t <= Nodes[0].T)
        {
            for (unsigned int i = 0; i < Components; ++i)
                outVals[i] = Nodes[0].Value[i];
            return;
        }
        if (t >= Nodes[Nodes.size() - 1].T)
        {
            for (unsigned int i = 0; i < Components; ++i)
                outVals[i] = Nodes[Nodes.size() - 1].Value[i];
            return;
        }

        //Get the two nodes the given t value is between.
        unsigned int topBound = 1;
        while (Nodes[topBound].T < t)
            topBound += 1;
        const GNode & start = Nodes[topBound - 1];
        const GNode & end = Nodes[topBound];

        //Plug in a formula for splines.
        for (unsigned int i = 0; i < Components; ++i)
        {
            float remappedT = BasicMath::LerpComponent(start.T, end.T, t),
                  oneMinusRT = 1.0f - remappedT;
            float tRange = end.T - start.T,
                  valRange = end.Value[i] - start.Value[i];
            float a = (start.Slope[i] * tRange) - valRange,
                  b = (-end.Slope[i] * tRange) + valRange;

            outVals[i] = (oneMinusRT * start.Value[i]) + (remappedT * end.Value[i]) + (remappedT * oneMinusRT * ((a * oneMinusRT) + (b * remappedT)));
        }
    }


private:

    static std::vector<GNode> MakeVector(GNode n1)
    {
        std::vector<GNode> ret;
        ret.insert(ret.end(), n1);
        return ret;
    }
    static std::vector<GNode> MakeVector(GNode n1, GNode n2)
    {
        std::vector<GNode> ret;
        ret.insert(ret.end(), n1);
        ret.insert(ret.end(), n2);
        return ret;
    }
    static std::vector<GNode> MakeVector(GNode n1, GNode n2, GNode n3)
    {
        std::vector<GNode> ret;
        ret.insert(ret.end(), n1);
        ret.insert(ret.end(), n2);
        ret.insert(ret.end(), n3);
        return ret;
    }
    static std::vector<GNode> MakeVector(GNode n1, GNode n2, GNode n3, GNode n4)
    {
        std::vector<GNode> ret;
        ret.insert(ret.end(), n1);
        ret.insert(ret.end(), n2);
        ret.insert(ret.end(), n3);
        ret.insert(ret.end(), n4);
        return ret;
    }
};