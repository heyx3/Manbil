#pragma once


#include "../IO/SerializationWrappers.h"

#include "../Rendering/Textures/MTexture2D.h"


//A mesh of world geometry that uses a material, diffuse texture, and possibly a normal map texture.
//Either uses the mesh's UVs, or uses the horizontal world-space position as UVs.
struct GeoSet : public ISerializable
{
public:

    std::string DiffuseTexFile, NormalTexFile;
    std::string MeshFile;
    
    Vector2f DiffuseTexScale, NormalTexScale;

    float Specular, SpecularIntensity;

    bool IsTransparent, UseNormalMap, UseWorldPosUV;


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