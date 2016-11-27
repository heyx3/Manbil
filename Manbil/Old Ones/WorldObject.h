#pragma once

#include "../Rendering/Rendering.hpp"
#include "../Rendering/Data Nodes/ShaderGenerator.h"
#include "GeoSet.h"


class OldOneShadowMap;


//A part of the world geometry.
class WorldObject
{
public:

    static const Vector3f LightDir;
    static const float AmbientLight, DiffuseLight;

    
    static RenderIOAttributes GetVertexInputs(bool getUVs, bool hasNormalMaps);


    Mesh MyMesh;
    Transform MyTransform;

    std::unique_ptr<Material> Mat;
    UniformDictionary Params;

    MTexture2D DiffTex, NormalTex;


    WorldObject(GeoSet geoInfo, std::string& outError);


    bool GetUsesVertexUVs(void) const { return getUVs; }
    bool GetUsesNormalMaps(void) const { return useNormalMaps; }

    void Render(RenderInfo& info, OldOneShadowMap& shadowMap);


private:

    //Returns an error message if anything went wrong.
    std::string LoadMesh(const std::string& meshFile, bool getUVs,
                         bool hasNormalMaps, Mesh& outDat);
    ShaderGenerator::GeneratedMaterial LoadMaterial(const GeoSet& geoInfo);


    bool getUVs, useNormalMaps;
};