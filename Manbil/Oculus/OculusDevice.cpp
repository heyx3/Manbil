#include "OculusDevice.h"

#include <assert.h>
#include <unordered_map>
#include <OVR_CAPI.h>


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

    ovrPosef pse = ovrHmd_GetSensorState(ManbilOVR::GetDevice(index), 0.0f).Recorded.Pose;
    pos = Vector3f(pse.Position.x, pse.Position.y, pse.Position.z);
    rot = Quaternion(pse.Orientation.x, pse.Orientation.y, pse.Orientation.z, pse.Orientation.w);
}












/*
using namespace OVR;

typedef std::string STR;

Ptr<DeviceManager> OculusSystem::pManager = Ptr<DeviceManager>();

void OculusDevice::UpdateDevice(void)
{
    OVR::Posef psf = sensorFusion.GetPoseAtTime(sensorFusion.GetTime());
	OVR::Quatf hmdOrient;
	quat = Quaternion(hmdOrient.x, hmdOrient.y, hmdOrient.z, hmdOrient.w);

	previousRot = currentRot;
	hmdOrient.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&currentRot.yaw, &currentRot.pitch, &currentRot.roll);


	//if (calibrator.IsAutoCalibrating())
	//{
	//	calibrator.UpdateAutoCalibration(sensorFusion);
	//	if (calibrator.IsCalibrated())
	//	{
	//		doneAutoCalibration = true;
	//	}
	//}
}

void OculusSystem::GetDeviceInfo(Ptr<HMDDevice> pHMD, RiftDeviceInfo & outInfo)
{
	HMDInfo hmd;
	if (pHMD->GetDeviceInfo(&hmd))
	{
		outInfo.DisplayInfo = RiftDisplayInfo(hmd);
	}

	outInfo.SensorInfo = *pHMD->GetSensor();
}

std::shared_ptr<OculusDevice> OculusSystem::GetDevice(int deviceNumb)
{
	OVR::Ptr<OVR::HMDDevice> pHMD;
	pManager = *OVR::DeviceManager::Create();
	for (int i = 0; i < deviceNumb; ++i)
	{
		pManager->EnumerateDevices<OVR::HMDDevice>().Next();
	}

	pHMD = *pManager->EnumerateDevices<OVR::HMDDevice>().CreateDevice();
	if (pHMD == 0) return std::shared_ptr<OculusDevice>();

	RiftDeviceInfo rdi;
	GetDeviceInfo(pHMD, rdi);
	return std::shared_ptr<OculusDevice>(new OculusDevice(pHMD, rdi));
}


*/