#pragma once


//Information about a perspective projection.
struct ProjectionInfo
{
public:

    //Vertical field of view, measured in radians.
    float FOV;
    //Width of the viewport. Should be in pixels. Used to calculate aspect ratio.
    float Width;
    //Height of the viewport. Should be in pixels. Used to calculate aspect ratio.
    float Height;
    //The near Z plane. Anything closer than this to the camera will not be rendered.
    float zNear;
    //The far Z plane. Anything farther than this from the camera will not be rendered.
    float zFar;


    //Creates a ProjectionInfo with some basic, sane values.
	ProjectionInfo(void) : FOV(2.3561945f), Width(800.0f), Height(600.0f), zNear(0.1f), zFar(1000.0f) { }
    //Creates a ProjectionInfo with the given values.
	ProjectionInfo(float fov, float width, float height, float _zNear, float _zFar)
        : FOV(fov), Width(width), Height(height), zNear(_zNear), zFar(_zFar) { }


    //Gets the aspect ratio (the ratio of viewport width / height).
	float GetAspectRatio(void) const { return Width / Height; }

    //Sets the vertical FOV to the given value in degrees.
    void SetFOVDegrees(float degrees) { FOV = degrees * 0.0174532925f; }
};