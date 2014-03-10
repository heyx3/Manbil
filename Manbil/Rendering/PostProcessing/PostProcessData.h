#pragma once

#include <string>
#include "../../Math/Vectors.h"
#include "../Materials/Data Nodes/DataNode.h"



//Abstract class representing a kind of post-process effect.
class PostProcessEffect : public DataNode
{
public:

    //Returns whether or not this struct's effect results in any visible effect on the screen.
    virtual bool EffectsScreen(void) const = 0;
};



//Represents data about what kinds of post-processing to do.


class ColorTintArgs : public PostProcessEffect
{
public:

    //Returns whether or not this set of arguments results in any visible effect.
    bool EffectsScreen(void) const { return ColorScales != Vector3f(1.0f, 1.0f, 1.0f); }


    Vector3f ColorScales;
    ColorTintArgs(Vector3f colorScales = Vector3f(1.0f, 1.0f, 1.0f)) : ColorScales(colorScales) { }
};

class ContrastArgs : public PostProcessEffect
{
public:

    //Returns whether or not this set of arguments results in any visible effect.
    bool EffectsScreen(void) const { return (Iterations > 0); }


    //Different kinds of contrast amounts.
    enum Strengths
    {
        S_Light,
        S_Heavy,
    };
    
    //Stronger contrast generally means a larger performance overhead.
    Strengths Strength;
    //More iterations results in a stronger contrast but also more performance overhead.
    unsigned int Iterations;

    ContrastArgs(Strengths strength = Strengths::S_Light, unsigned int iterations = 0)
        : Strength(strength), Iterations(iterations) { }
};

class FogArgs : public PostProcessEffect
{
public:

    //Returns whether or not this set of arguments results in any visible effect.
    bool EffectsScreen(void) const { return  }

    
    float Dropoff;
    Vector3f FogColor;
};