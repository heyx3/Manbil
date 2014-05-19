#pragma once

#include <string>
#include <memory>
#include "../Math/Quaternion.h"



//Handles a Rift device.
class OculusDevice
{
public:

    //Initializes the whole Oculus Rift system.
    static void InitializeSystem(void);
    //Destroys the whole Oculus Rift system.
    static void DestroySystem(void);


    OculusDevice(int _index = 0);
    OculusDevice(const OculusDevice & cpy); //Intentionally not implemented -- no copying!
    ~OculusDevice(void);

    //Returns whether this instance points to a valid HMD.
    bool IsValid(void) const;

    Vector3f GetPosition(void) const { return pos; }
    Quaternion GetRotation(void) const { return rot; }

    void Update(void);

private:

    int index;
    Vector3f pos;
    Quaternion rot;
    
    static bool initialized;
};











//#include <OVR.h>
//#include <OVRVersion.h>

/*
typedef OVR::HMDInfo RiftDisplayInfo;
typedef OVR::SensorDevice RiftSensor;
struct RiftDeviceInfo { RiftDisplayInfo DisplayInfo; OVR::Ptr<RiftSensor> SensorInfo; };

struct YawPitchRoll { public: float yaw, pitch, roll; YawPitchRoll(float _yaw = 0, float _pitch = 0, float _roll = 0) : yaw(_yaw), pitch(_pitch), roll(_roll) { } };


//Wraps an Oculus device.
class OculusDevice
{
public:

	OculusDevice(OVR::Ptr<OVR::HMDDevice> hmd, const RiftDeviceInfo & _info)
		: pHMD(hmd), info(_info), sensorFusion(_info.SensorInfo)//, doneAutoCalibration(false)
	{
        if (info.SensorInfo)
            sensorFusion.AttachToSensor(info.SensorInfo);
    }
    ~OculusDevice(void) { info.SensorInfo.Clear(); }



	Quaternion GetCurrentRotation(void) const { return quat; }
    YawPitchRoll GetYawPitchRoll(void) const { return currentRot; }
	YawPitchRoll GetPreviousYawPitchRoll(void) const { return previousRot; }

	const RiftDeviceInfo & GetDeviceInfo(void) const { return info; }


	void UpdateDevice(void);
    void ReleaseDevice(void) { info.SensorInfo.Clear(); }

	//"Auto calibration" means that the Rift collects magnetometer data for yaw drift correction on its own. Once it has enough data points from a big enough sample range, it will be done.
	//void StartAutoCalibration(void) { doneAutoCalibration = false; if (!calibrator.IsAutoCalibrating()) calibrator.BeginAutoCalibration(sensorFusion); }
	//bool IsDoneAutoCalibration(void) { return doneAutoCalibration; }


private:

	OVR::Ptr<OVR::HMDDevice> pHMD;
	RiftDeviceInfo info;
	OVR::SensorFusion sensorFusion;

	Quaternion quat;
	YawPitchRoll currentRot, previousRot;

	//bool doneAutoCalibration;
	//OVR::Util::Render: calibrator;
};



//Handles general oculus stuff.
struct OculusSystem
{
public:

	//Must be called before using any rift hardware.
	static void InitializeRiftSystem(void) { OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All)); }
	//Must be called after all rift hardware is done with at the end of the program.
	static void DestroyRiftSystem(void) { pManager.Clear(); OVR::System::Destroy(); }

	static OVR::Ptr<OVR::DeviceManager> pManager;

	static void GetDeviceInfo(OVR::Ptr<OVR::HMDDevice> pHMD, RiftDeviceInfo & outInfo);

	//0 gets the first rift, 1 gets the second, and so on.
	//Returns "std::shared_ptr<OculusDevice>()" if the device cannot be found.
	static std::shared_ptr<OculusDevice> GetDevice(int deviceNumb);
};

*/