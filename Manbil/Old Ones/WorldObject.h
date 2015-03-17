#pragma once

#include "../Rendering/Rendering.hpp"
#include "../Rendering/Data Nodes/ShaderGenerator.h"
#include "GeoSet.h"





//A part of the world geometry.
class WorldObject
{
public:

    static const Vector3f LightDir;
    static const float AmbientLight, DiffuseLight;


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