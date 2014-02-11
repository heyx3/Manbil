/*#include "GeneralOculus.h"

using namespace OVR;

typedef std::string STR;

Ptr<DeviceManager> GeneralOculus::pManager = Ptr<DeviceManager>();

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



STR GeneralOculus::GetDistortionPixelShader(STR vec2_lensCenter, STR vec2_screenCenter,
											STR vec2_scale, STR vec2_scaleIn, STR float4_hmdWarpParam,
											STR texCoordVarIn, STR texCoordVarOut,
											STR tempSuffix)
{
	const std::string theta = STR("theta") + tempSuffix,
		rSq = STR("rSq") + tempSuffix,
		rVector = STR("rVector") + tempSuffix,
		hmdwpx = float4_hmdWarpParam + ".x",
		hmdwpy = float4_hmdWarpParam + ".y",
		hmdwpz = float4_hmdWarpParam + ".z",
		hmdwpw = float4_hmdWarpParam + ".w";

	return STR("\n\tvec2 ") + theta + " = (" + texCoordVarIn + " - " + vec2_lensCenter + ") * " + vec2_scaleIn + ";\n\t" +
		"float " + rSq + " = " + theta + ".x * " + theta + ".x + " + theta + ".y * " + theta + ".y;\n\t" +
		"float " + rVector + " = " + theta + " * (" + hmdwpx + " + " + hmdwpy + " * " + rSq + " +\n\t\t\t\t" +
		hmdwpz + " * " + rSq + " * " + rSq + " +\n\t\t\t\t" +
		hmdwpw + " * " + rSq + " * " + rSq + " * " + rSq + ");\n\t" +
		"vec2 " + texCoordVarOut + " = " + vec2_lensCenter + " + " + vec2_scale + " * " + rVector + ";\n\n\t";
}

void GeneralOculus::GetDeviceInfo(Ptr<HMDDevice> pHMD, RiftDeviceInfo & outInfo)
{
	HMDInfo hmd;
	if (pHMD->GetDeviceInfo(&hmd))
	{
		outInfo.DisplayInfo = RiftDisplayInfo(hmd);
	}

	outInfo.SensorInfo = *pHMD->GetSensor();
}

std::shared_ptr<OculusDevice> GeneralOculus::GetDevice(int deviceNumb)
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
}*/