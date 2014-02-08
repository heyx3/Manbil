#pragma once

//Channels that can be read in from a texture.
enum ChannelsIn
{
    Red, Green, Blue, Alpha,
    GrayscaleAverage,
    GrayscaleMax,
    GrayscaleMin,
};

//Channels that can be written into a texture.
enum ChannelsOut
{
    Red, Green, Blue, Alpha,
    AllColorChannels,
    AllChannels,
};