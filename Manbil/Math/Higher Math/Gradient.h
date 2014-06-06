#pragma once

#include <vector>


//Represents a single node in a gradient.
struct GradientNode
{
public:

    //The value at this node.
    float Value;
    //The position of this node, from 0 to 1.
    float T;

    GradientNode(float value, float t) : Value(value), T(t) { }
};



//Represents some kind of gradient for a value.
class Gradient
{
public:

    std::vector<GradientNode> Nodes;


    Gradient(const std::vector<GradientNode> & nodes) : Nodes(nodes) { }
};