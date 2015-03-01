#pragma once

//Channels that can be read in from a texture.
enum ChannelsIn
{
    CI_Red, CI_Green, CI_Blue, CI_Alpha,
    CI_GrayscaleAverage,
    CI_GrayscaleMax,
    CI_GrayscaleMin,
};

//Channels that can be written into a texture.
enum ChannelsOut
{
    CO_Red, CO_Green, CO_Blue, CO_Alpha,
    CO_AllColorChannels,
    CO_AllChannels,
};