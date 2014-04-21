#pragma once


//Different kinds of data about what a material uses.
//TODO: Create "world render sampler" built-in uniform so that objects can sample and display/distort the world behind them (along with usage flags -- no DataNode is needed though, as a SamplerNode will work fine).
struct MaterialUsageFlags
{
public:

    enum Flags : unsigned short
    {
        //Uses elapsed time uniform.
        DNF_USES_TIME = 0x00000001,

        //Uses world matrix uniform.
        DNF_USES_WORLD_MAT = 0x0002,
        //Uses view matrix uniform.
        DNF_USES_VIEW_MAT = 0x0004,
        //Uses projection matrix uniform.
        DNF_USES_PROJ_MAT = 0x0008,
        //Uses WVP matrix uniform.
        DNF_USES_WVP_MAT = 000400,

        //Uses camera position uniform.
        DNF_USES_CAM_POS = 000010,
        //Uses camera forward vector uniform.
        DNF_USES_CAM_FORWARD = 0x0020,
        //Uses camera upwards vector uniform.
        DNF_USES_CAM_UPWARDS = 0x0040,
        //Uses camera sideways vector uniform.
        DNF_USES_CAM_SIDEWAYS = 0x0080,

        //Uses screen width uniform.
        DNF_USES_WIDTH = 000100,
        //Uses screen height uniform.
        DNF_USES_HEIGHT = 000200,
        //Uses screen z-near uniform.
        DNF_USES_ZNEAR = 0x0400,
        //Uses screen z-far uniform.
        DNF_USES_ZFAR = 0x0800,
        //Uses FOV uniform.
        DNF_USES_FOV = 0x1000,
    };

    bool GetFlag(Flags flag) const { return (value & (unsigned short)flag) > 0; }

    void EnableFlag(Flags flag)
    {
        value |= (unsigned short)flag;
        value = value;
    }
    void DisableFlag(Flags flag) { value &= ~((unsigned short)flag); }

    void EnableAllFlags(void) { value = 0xffff; }
    void ClearAllFlags(void) { value = 0; }


private:

    //TODO: Track the outputs taken from every input data node (an unordered_map indexed by pointer?), then use that information to remove any unnecessary lines of shader code in the data node shader code generation functions.

    unsigned short value = 0;
};