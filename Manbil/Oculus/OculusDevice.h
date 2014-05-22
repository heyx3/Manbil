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
    Vector3f GetEulerAngles(void) const { return eulerAngles; }

    Vector3f GetVelocity(void) const { return vel; }
    Vector3f GetAcceleration(void) const { return accel; }
    Vector3f GetRotationalVelocity(void) const { return rotVel; }
    Vector3f GetRotationalAccel(void) const { return rotAccel; }


    void Update(void);


    //Tries to start up sensor tracking.
    //Returns whether the device has a sensor capable of tracking (at the very least) orientation.
    bool AttemptStartSensor(void);


private:

    int index;
    Vector3f pos;
    Quaternion rot;
    Vector3f eulerAngles;

    Vector3f vel, accel;
    Vector3f rotVel, rotAccel;
    
    static bool initialized;
};