#pragma once

#include "../Rendering.hpp"
#include "../../Math/HigherMath.hpp"

//Represents a flowing body of water.
//A bumpmap can be stored in the first texture.
class Water
{
public:

    //Different kinds of water motion.
    enum WaterTypes
    {
        //Moves in a direction.
        Directed,
        //Ripples outward from several spots.
        Rippling,
    };

    //Data that only applies to directional water.
    struct DirectionalWaterArgs
    {
    public:
        //The direction and magnitude of the water flow.
        Vector2f Flow;
        float Amplitude;
        DirectionalWaterArgs(Vector2f flow, float amplitude) : Flow(flow), Amplitude(amplitude) { }
    };

    //Data that only applies to rippling water.
    struct RippleWaterArgs
    {
    public:
        //The position along the horizontal plane of the ripple's source.
        Vector2f Source;
        //The dropoff scaling, proportional to the distance from the source.
        //A value of 1.0 results in no dropoff.
        float DropoffScale;
        RippleWaterArgs(Vector2f source, float dropoffScale) : Source(source), DropoffScale(dropoffScale) { }
    };


    Material * Mat;


    //Makes a new Water object that uses circular ripples.
    Water(unsigned int width, unsigned int height, unsigned int maxRipples, Vector2f texturePanDirection);
    //Makes a new Water object that uses directional water.
    Water(Vector2f texturePanDirection, DirectionalWaterArgs mainFlow);


    WaterTypes GetWaterType(void) const { return waterType; }


    //Adds another ripple to the water.
    //This function only applies to rippling water.
    //Returns whether the ripple was created successfully.
    bool AddRipple(RippleWaterArgs args);
    //Adds a new flow to the water.
    //This function only applies to directional water.
    //Returns whether the flow was created successfully.
    bool AddFlow(DirectionalWaterArgs args);

    //Returns whether the uniform was set successfully.
    bool SetBumpmapHeight(float newHeight) { return Mat->SetUniformF("bumpmapHeight", &newHeight, 1); }


private:

    WaterTypes waterType;

    //Ripple stuff.
    int currentRippleIndex;
    int maxRipples;
    int nextRippleID;
    int totalRipples;
    std::vector<float> ripplesTimeSinceCreated; //Used to limit the distance the ripple reaches out from.

    //Flow stuff.
    int currentFlowIndex;

    //Gets the single RenderingPass needed to render rippling water.
    static RenderingPass GetRippleWaterRenderer(void);
    //Gets the single RenderingPass needed to render directional water.
    static RenderingPass GetDirectionalWaterRenderer(void);
};