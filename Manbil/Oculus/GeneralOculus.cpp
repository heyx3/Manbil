#include "GeneralOculus.h"

using namespace OVR;

typedef std::string STR;

Ptr<DeviceManager> OculusSystem::pManager = Ptr<DeviceManager>();

void OculusDevice::UpdateDevice(void)
{
	OVR::Quatf hmdOrient = sensorFusion.GetOrientation();
	quat = Quaternion(hmdOrient.x, hmdOrient.y, hmdOrient.z, hmdOrient.w);

	previousRot = currentRot;
	hmdOrient.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&currentRot.yaw, &currentRot.pitch, &currentRot.roll);


	if (calibrator.IsAutoCalibrating())
	{
		calibrator.UpdateAutoCalibration(sensorFusion);
		if (calibrator.IsCalibrated())
		{
			doneAutoCalibration = true;
		}
	}
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