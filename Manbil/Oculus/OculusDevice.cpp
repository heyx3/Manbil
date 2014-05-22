#include "OculusDevice.h"

#include <assert.h>
#include <unordered_map>
#include <OVR_CAPI.h>
#include <OVR.h>


//Rift devices will be stored statically, since there should ever only be one instance per device.
//Additionally, put them in a namespace here instead of statically in OculusDevice or OculusSystem so that the SDK API isn't exposed publically.
namespace ManbilOVR
{
    std::unordered_map<int, ovrHmd> Devices;
    std::unordered_map<int, ovrHmdDesc> Descriptions;

    //"assert()"'s that the given device index does or doesn't exist already.
    void AssertDeviceExistence(int deviceIndex, bool shouldExist)
    {
        auto lookup1 = Devices.find(deviceIndex);
        auto lookup2 = Descriptions.find(deviceIndex);

        assert((lookup1 != Devices.end()) == shouldExist);
        assert((shouldExist && (lookup1->second == 0 || lookup2 != Descriptions.end())) ||
               (!shouldExist && lookup2 == Descriptions.end()));
    }


    void CreateDevice(int deviceIndex)
    {
        AssertDeviceExistence(deviceIndex, false);

        ovrHmd hmd = ovrHmd_Create(deviceIndex);
        Devices[deviceIndex] = hmd;

        if (hmd != 0)
        {
            ovrHmdDesc desc;
            ovrHmd_GetDesc(hmd, &desc);
            Descriptions[deviceIndex] = desc;
        }
    }
    void DestroyDevice(int deviceIndex)
    {
        AssertDeviceExistence(deviceIndex, true);

        auto lookup1 = Devices.find(deviceIndex);
        ovrHmd_Destroy(lookup1->second);
        Devices.erase(lookup1);

        Descriptions.erase(deviceIndex);
    }

    ovrHmd GetDevice(int deviceIndex)
    {
        AssertDeviceExistence(deviceIndex, true);
        return Devices[deviceIndex];
    }
    ovrHmdDesc GetDeviceDesc(int deviceIndex)
    {
        AssertDeviceExistence(deviceIndex, true);
        return Descriptions[deviceIndex];
    }
}



bool OculusDevice::initialized = false;

void OculusDevice::InitializeSystem(void)
{
    assert(!initialized);
    ovr_Initialize();
    initialized = true;
}
void OculusDevice::DestroySystem(void)
{
    assert(initialized);
    ovr_Shutdown();
    initialized = false;
}


OculusDevice::OculusDevice(int _index)
    : index(_index), pos(), rot(Vector3f(0.0f, 0.0f, 1.0f), 0.0f)
{
    assert(initialized);
    ManbilOVR::CreateDevice(index);
}
OculusDevice::~OculusDevice(void)
{
    assert(initialized);
    ManbilOVR::DestroyDevice(index);
}

bool OculusDevice::IsValid(void) const
{
    return ManbilOVR::GetDevice(index) != 0;
}

void OculusDevice::Update(void)
{
    assert(initialized);

    ovrSensorState sensors = ovrHmd_GetSensorState(ManbilOVR::GetDevice(index), 0.0f);
    ovrPosef pose = sensors.Predicted.Pose;

    if (sensors.StatusFlags & ovrStatus_OrientationTracked)
    {
        rot = Quaternion(pose.Orientation.x, pose.Orientation.y, pose.Orientation.z, pose.Orientation.w);
        rotVel = Vector3f(sensors.Predicted.AngularVelocity.x, sensors.Predicted.AngularVelocity.y, sensors.Predicted.AngularVelocity.z);
        rotAccel = Vector3f(sensors.Predicted.AngularAcceleration.x, sensors.Predicted.AngularAcceleration.y, sensors.Predicted.AngularAcceleration.z);

        OVR::Quat<float>(pose.Orientation).GetEulerAngles<OVR::Axis::Axis_X, OVR::Axis::Axis_Y, OVR::Axis::Axis_Z>(&eulerAngles.x, &eulerAngles.y, &eulerAngles.z);
    }
    if (sensors.StatusFlags & ovrStatus_PositionTracked)
    {
        pos = Vector3f(pose.Position.x, pose.Position.y, pose.Position.z);
        vel = Vector3f(sensors.Predicted.LinearVelocity.x, sensors.Predicted.LinearVelocity.y, sensors.Predicted.LinearVelocity.z);
        accel = Vector3f(sensors.Predicted.LinearAcceleration.x, sensors.Predicted.LinearAcceleration.y, sensors.Predicted.LinearAcceleration.z);
    }
}


bool OculusDevice::AttemptStartSensor(void)
{
    return ovrHmd_StartSensor(ManbilOVR::GetDevice(index),
                              ovrHmdCap_Orientation | ovrHmdCap_YawCorrection | ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction,
                              ovrHmdCap_Orientation);
}