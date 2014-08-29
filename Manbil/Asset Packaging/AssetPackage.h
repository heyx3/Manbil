#pragma once

#include "../IO/DataSerialization.h"
#include "../Rendering/Texture Management/MTexture2D.h"
#include "../Rendering/Texture Management/MTexture3D.h"
#include "../Rendering/Texture Management/MTextureCubemap.h"
#include "../Rendering/Materials/Data Nodes/DataNode.h"
#include "../Material.h"



//Represents a standalone collection of textures and material DataNode graphs.
struct AssetPackage : public ISerializable
{
public:

    typedef std::shared_ptr<SerializedMaterial> SerializedMatPtr;
    typedef std::shared_ptr<MTexture2D> Texture2DPtr;
    typedef std::shared_ptr<MTexture3D> Texture3DPtr;
    typedef std::shared_ptr<MTextureCubemap> TextureCubePtr;


    //Each 2D texture, indexed by its name.
    std::unordered_map<std::string, Texture2DPtr> Tex2s;
    //Each 3D texture, indexed by its name.
    std::unordered_map<std::string, Texture3DPtr> Tex3s;
    //Each Cubemap texture, indexed by its name.
    std::unordered_map<std::string, TextureCubePtr> TexCubes;
    //Each material, indexed by its name.
    std::unordered_map<std::string, SerializedMatPtr> Materials;

    //Associates each texture sampler with its default texture value.
    std::unordered_map<std::string, std::string> DefaultTextureSamplers;
    //Associates each float uniform with its default value.
    std::unordered_map<std::string, VectorF> DefaultUniformValues;
    //TODO: Make a "float array uniform" DataNode. Then add an unordered map here and update the data serialization stuff.


    virtual bool WriteData(DataWriter * writer, std::string & outError) const override;
    virtual bool ReadData(DataReader * reader, std::string & outError) override;
};