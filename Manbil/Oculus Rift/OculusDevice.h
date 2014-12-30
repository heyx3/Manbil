#pragma once

#include "../Math/LowerMath.hpp"

#include <OVR_CAPI.h>
//#include <OVR_CAPI_GL.h>

//Represents an Oculus Rift.
class OculusDevice
{
public:

    static void InitializeSystem(void) { ovr_Initialize(); }
    static void DestroySystem(void) { ovr_Shutdown(); }

    //Finds how many HMD's are currently connected.
    static int DetectCurrentHMDs(void) { return ovrHmd_Detect(); }


    /*
    //If there was an error, sets the given string to an error message.
    OculusDevice(int deviceIndex, std::string& outErrorMsg);
    //If there was an error, sets the given string to an error message.
    OculusDevice(int deviceIndex, std::string& outErrorMsg,
                 bool requireRotInput, bool requirePosInput, bool requireYawCorrection);
    ~OculusDevice(void) { ovrHmd_Destroy(hmd); }


    OculusDevice(const OculusDevice& cpy) = delete;
    OculusDevice& operator=(const OculusDevice& cpy) = delete;


    //Updates this device's position and rotation.
    //Should be done as late as possible in the rendering pipeline to get up-to-date values.
    void UpdateDevice(void);

    
    //Gets whether this device represents a valid HMD.
    bool IsValid(void) const { return hmd != 0; }

    //Gets whether the HMD is currently connected.
    bool IsHMDConnected(void) const { return state.StatusFlags & ovrStatus_HmdConnected; }
    //Gets whether the position-tracking device is currently connected.
    bool IsPosTrackerConnected(void) const { return state.StatusFlags & ovrStatus_PositionConnected; }

    //Gets whether the HMD is currently tracking rotation.
    bool IsTrackingRotation(void) const { return state.StatusFlags & ovrStatus_OrientationTracked; }
    //Gets whether the HMD is currently tracking position.
    bool IsTrackingPosition(void) const { return state.StatusFlags & ovrStatus_PositionTracked; }


    //Gets info about this device.
    const ovrHmd& GetHMDData(void) const { return hmd; }
    //Gets info about this device's current pose.
    const ovrPoseStatef& GetPose(void) const { return state.HeadPose; }


private:

    ovrHmd hmd;
    ovrTrackingState state;

    */
};