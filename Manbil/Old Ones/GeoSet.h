#pragma once


#include "../IO/Serialization.h"
#include "../Rendering/Textures/MTexture2D.h"


//A set of world geometry that uses the same material/textures.
//Either uses the mesh's UVs or the horizontal world-space vertex positions as UVs.
struct GeoSet : public ISerializable
{
public:

    std::string DiffuseTexFile, NormalTexFile;
    std::string MeshFile;
    
    Vector2f DiffuseTexScale, NormalTexScale;

    float Specular, SpecularIntensity;

    bool UseNormalMap, UseWorldPosUV;


    GeoSet(void) { }


    virtual void WriteData(DataWriter* writer) const override;
    virtual void ReadData(DataReader* reader) override;
};

//A collection of world geometry data.
struct GeoSets : public ISerializable
{
public:
    std::vector<GeoSet> Sets;
    
    virtual void WriteData(DataWriter* writer) const override;
    virtual void ReadData(DataReader* reader) override;
};