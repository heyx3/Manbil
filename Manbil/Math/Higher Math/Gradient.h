#pragma once

#include <vector>

#include "../BasicMath.h"


//Should be an int in the range [1, 4].
//The number of different float values in a single point on the gradient
//    (float, vec2, vec3, or vec4).
template<unsigned int Components>
//A single node in a gradient.
struct GradientNode
{
public:

    //TODO: Split "Value" into "LeftValue" and "RightValue" so that the gradient no longer has to be continuous.

    //The position of this node, from 0 to 1.
    float T;
    //The value at this node.
    float Value[Components];

    GradientNode(float t, const float value[Components]) : T(t) { memcpy(Value, value, sizeof(float) * Components); }
    GradientNode(const GradientNode & cpy) : GradientNode(cpy.T, cpy.Value) { } // TODO: Test that using "memcpy(this, &cpy, sizeof(GradientNode<Components>))" would work instead.
};



//Should be an int in the range [1, 4].
//The number of different float values in a single point on the gradient.
//    (float, vec2, vec3, or vec4).
template<unsigned int Components>
//Represents some kind of smooth gradient for a value.
//Can use linear, cubic, or quintic interpolation.
class Gradient
{
public:

    typedef GradientNode<Components> GNode;

    enum Smoothness
    {
        SM_LINEAR,
        SM_CUBIC,
        SM_QUINTIC,
    };


    Smoothness SmoothQuality;
    std::vector<GradientNode<Components>> Nodes;


    Gradient(const std::vector<GNode> & nodes, Smoothness smoothQuality) : SmoothQuality(smoothQuality), Nodes(nodes) { }
    Gradient(GNode startVal, GNode endVal, Smoothness smoothQuality) : Gradient(MakeVector(startVal, endVal), smoothQuality) { }
    Gradient(GNode startVal, GNode endVal, GNode mid1, Smoothness smoothQuality) : Gradient(MakeVector(startVal, mid1, endVal, smoothQuality)) { }
    Gradient(GNode startVal, GNode endVal, GNode mid1, GNode mid2, Smoothness smoothQuality) : Gradient(MakeVector(startVal, mid1, mid2, endVal, smoothQuality)) { }


    //Whether this gradient has at least one point, making it valid for use.
    bool IsValidGradient(void) const { return Nodes.size() > 0; }

    //Gets the gradient value at the given t.
    //'t' will be clamped to be inside the range this gradient covers.
    //Assumes that this gradient is valid.
    void GetValue(float t, float outVals[Components]) const
    {
        assert(IsValidGradient());

        //Check edge-cases.
        if (Nodes.size() == 1 || t <= Nodes[0].T)
        {
            Set(Nodes[0].Value, outVals);
            return;
        }
        if (t >= Nodes[Nodes.size() - 1].T)
        {
            Set(Nodes[Nodes.size() - 1].Value, outVals);
            return;
        }

        //Get the two nodes the given t value is between.
        unsigned int topBound = 1;
        while (Nodes[topBound].T < t)
            topBound += 1;
        const GNode & start = Nodes[topBound - 1];
        const GNode & end = Nodes[topBound];

        //Use a lerp between the start and end, but first smooth the "t" component to create a smooth curve.
        float remappedT = BasicMath::LerpComponent(start.T, end.T, t);
        switch (SmoothQuality)
        {
            case SM_LINEAR: break;
            case SM_CUBIC: remappedT = BasicMath::Smooth(remappedT); break;
            case SM_QUINTIC: remappedT = BasicMath::Supersmooth(remappedT); break;
            default: assert(false);
        }
        for (unsigned int i = 0; i < Components; ++i)
            outVals[i] = BasicMath::Lerp(start.Value[i], end.Value[i], remappedT);
    }


private:

    static void Set(const float src[Components], float dest[Components])
    {
        memcpy(dest, src, sizeof(float) * Components);
    }

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