#pragma once

#include "../Rendering/Rendering.hpp"
#include "../Rendering/Data Nodes/ShaderGenerator.h"
#include "GeoSet.h"



extern const Vector3f LightDir;
extern const float AmbientLight, DiffuseLight;


//A part of the world geometry.
class WorldObject
{
public:

    Mesh MyMesh;

    Material* Mat;
    UniformDictionary Params;

    MTexture2D DiffTex, NormalTex;


    WorldObject(GeoSet geoInfo, std::string& outError);
    ~WorldObject(void);


    void Render(RenderInfo& info);


private:

    //Returns an error message if anything went wrong.
    static std::string LoadMesh(const std::string& meshFile, bool getUVs,
                                bool hasNormalMaps, MeshData& outDat);
    ShaderGenerator::GeneratedMaterial LoadMaterial(const GeoSet& geoInfo);
};