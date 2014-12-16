#include "OculusDevice.h"


/*
OculusDevice::OculusDevice(int deviceIndex, std::string& outErrorMsg)
{
    hmd = ovrHmd_Create(deviceIndex);
    if (hmd == 0)
    {
        outErrorMsg = "No device available";
        return;
    }

    unsigned int supportedFlags = ovrTrackingCap_Orientation |
                                  ovrTrackingCap_MagYawCorrection |
                                  ovrTrackingCap_Position;
    ovrBool tryInit = ovrHmd_ConfigureTracking(hmd, supportedFlags, 0);
    if (!tryInit)
    {
        outErrorMsg = "Error configuring tracking for this HMD: " +
                          std::string(ovrHmd_GetLastError(hmd));
        return;
    }


}
OculusDevice::OculusDevice(int deviceIndex, std::string& outErrorMsg,
                           bool requireRotInput, bool requirePosInput, bool requireYawCorrection)
{
    hmd = ovrHmd_Create(deviceIndex);
    
}


void OculusDevice::UpdateDevice(void)
{
    state = ovrHmd_GetTrackingState(hmd, ovr_GetTimeInSeconds());


}
*/