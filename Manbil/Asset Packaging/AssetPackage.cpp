#include "AssetPackage.h"

#include "../DebugAssist.h"



struct KVPTex2 : public ISerializable
{
public:

    std::string Key = "";
    AssetPackage::Texture2DPtr Value = AssetPackage::Texture2DPtr(0);

    virtual bool WriteData(DataWriter * writer, std::string & outError) const override
    {
        if (!writer->WriteString(Key, "Tex Name", outError))
        {
            outError = "Error writing out the key, '" + Key + "': " + outError;
            return false;
        }

        if (!writer->WriteDataStructure(*Value.get(), "Texture Data", outError))
        {
            outError = "Error writing out the texture data: " + outError;
            return false;
        }

        return true;
    }
    virtual bool ReadData(DataReader * reader, std::string & outError) override
    {
        MaybeValue<std::string> tryKey = reader->ReadString(outError);
        if (!tryKey.HasValue())
        {
            outError = "Error reading in the key: " + outError;
            return false;
        }
        Key = tryKey.GetValue();

        Value = AssetPackage::Texture2DPtr(new MTexture2D(TextureSampleSettings2D(), PixelSizes::PS_8U, false));
        if (!reader->ReadDataStructure(*Value.get(), outError))
        {
            outError = "Error reading in the texture: " + outError;
            return false;
        }

        return true;
    }
};
struct KVPTex3 : public ISerializable
{
public:

    std::string Key = "";
    AssetPackage::Texture3DPtr Value = AssetPackage::Texture3DPtr(0);

    virtual bool WriteData(DataWriter * writer, std::string & outError) const override
    {
        if (!writer->WriteString(Key, "Key", outError))
        {
            outError = "Error writing out the key, '" + Key + "': " + outError;
            return false;
        }

        if (!writer->WriteDataStructure(*Value.get(), "Texture Data", outError))
        {
            outError = "Error writing out the texture data: " + outError;
            return false;
        }

        return true;
    }
    virtual bool ReadData(DataReader * reader, std::string & outError) override
    {
        MaybeValue<std::string> tryKey = reader->ReadString(outError);
        if (!tryKey.HasValue())
        {
            outError = "Error reading in the key: " + outError;
            return false;
        }
        Key = tryKey.GetValue();

        Value = AssetPackage::Texture3DPtr(new MTexture3D(TextureSampleSettings3D(), PixelSizes::PS_8U, false));
        if (!reader->ReadDataStructure(*Value.get(), outError))
        {
            outError = "Error reading in the texture: " + outError;
            return false;
        }

        return true;
    }
};
struct KVPTexCube : public ISerializable
{
public:

    std::string Key = "";
    AssetPackage::TextureCubePtr Value = AssetPackage::TextureCubePtr(0);

    virtual bool WriteData(DataWriter * writer, std::string & outError) const override
    {
        if (!writer->WriteString(Key, "Key", outError))
        {
            outError = "Error writing out the key, '" + Key + "': " + outError;
            return false;
        }

        if (!writer->WriteDataStructure(*Value.get(), "Texture Data", outError))
        {
            outError = "Error writing out the texture data: " + outError;
            return false;
        }

        return true;
    }
    virtual bool ReadData(DataReader * reader, std::string & outError) override
    {
        MaybeValue<std::string> tryKey = reader->ReadString(outError);
        if (!tryKey.HasValue())
        {
            outError = "Error reading in the key: " + outError;
            return false;
        }
        Key = tryKey.GetValue();

        Value = AssetPackage::TextureCubePtr(new MTextureCubemap(TextureSampleSettings3D(), PixelSizes::PS_8U, false));
        if (!reader->ReadDataStructure(*Value.get(), outError))
        {
            outError = "Error reading in the texture: " + outError;
            return false;
        }

        return true;
    }
};

struct KVPMaterial : public ISerializable
{
public:

    std::string Key = "";
    AssetPackage::SerializedMatPtr Value = AssetPackage::SerializedMatPtr(0);

    virtual bool WriteData(DataWriter * writer, std::string & outError) const override
    {
        if (!writer->WriteString(Key, "Key", outError))
        {
            outError = "Error writing out the material name, '" + Key + "': " + outError;
            return false;
        }

        if (!writer->WriteDataStructure(*Value.get(), "Material Data", outError))
        {
            outError = "Error writing out the material data: " + outError;
            return false;
        }

        return true;
    }
    virtual bool ReadData(DataReader * reader, std::string & outError) override
    {
        MaybeValue<std::string> tryKey = reader->ReadString(outError);
        if (!tryKey.HasValue())
        {
            outError = "Error reading in the material name: " + outError;
            return false;
        }
        Key = tryKey.GetValue();

        Value = AssetPackage::SerializedMatPtr(new SerializedMaterial());
        if (!reader->ReadDataStructure(*Value.get(), outError))
        {
            outError = "Error reading in the data for the material '" + Key + "': " + outError;
            return false;
        }

        return true;
    }
};

struct KVPString : public ISerializable
{
public:

    std::string Key = "";
    std::string Value = "";

    virtual bool WriteData(DataWriter * writer, std::string & outError) const override
    {
        if (!writer->WriteString(Key, "Sampler Name", outError))
        {
            outError = "Error writing out the key, '" + Key + "': " + outError;
            return false;
        }

        if (!writer->WriteString(Value, "Default Texture Name", outError))
        {
            outError = "Error writing out the value, '" + Value + "': " + outError;
            return false;
        }

        return true;
    }
    virtual bool ReadData(DataReader * reader, std::string & outError) override
    {
        MaybeValue<std::string> tryKey = reader->ReadString(outError);
        if (!tryKey.HasValue())
        {
            outError = "Error reading in the key: " + outError;
            return false;
        }
        Key = tryKey.GetValue();

        MaybeValue<std::string> tryVal = reader->ReadString(outError);
        if (!tryVal.HasValue())
        {
            outError = "Error reading in the value: " + outError;
            return false;
        }
        Value = tryVal.GetValue();

        return true;
    }
};

struct KVPVectorF : public ISerializable
{
public:

    std::string Key = "";
    VectorF Value = VectorF();

    virtual bool WriteData(DataWriter * writer, std::string & outError) const override
    {
        if (!writer->WriteString(Key, "Uniform Name", outError))
        {
            outError = "Error writing out the uniform name, '" + Key + "': " + outError;
            return false;
        }

        if (!writer->WriteDataStructure(Value, "Default Value", outError))
        {
            outError = "Error writing out the default value, '" + DebugAssist::ToString(Value) + "': " + outError;
            return false;
        }

        return true;
    }
    virtual bool ReadData(DataReader * reader, std::string & outError) override
    {
        MaybeValue<std::string> tryKey = reader->ReadString(outError);
        if (!tryKey.HasValue())
        {
            outError = "Error reading in the uniform name: " + outError;
            return false;
        }
        Key = tryKey.GetValue();

        if (!reader->ReadDataStructure(Value, outError))
        {
            outError = "Error reading in the default value for uniform '" + Key + "': " + outError;
            return false;
        }

        return true;
    }
};


template<typename TexPtrType, typename KVPType>
struct SerializedAssetPackageTexMap : public ISerializable
{
public:

    std::unordered_map<std::string, TexPtrType> Values;

    virtual bool WriteData(DataWriter * writer, std::string & outError) const override
    {
        if (!writer->WriteUInt(Values.size(), "Number of Textures", outError))
        {
            outError = "Error writing out the number of textures (" +
                           std::to_string(Values.size()) + ": " + outError;
            return false;
        }
        for (auto el = Values.begin(); el != Values.end(); ++el)
        {
            KVPType kvp;
            kvp.Key = el->first;
            kvp.Value = el->second;
            if (!writer->WriteDataStructure(kvp, "Texture File", outError))
            {
                outError = "Error writing out the info for texture '" + kvp.Key + "': " + outError;
                return false;
            }
        }

        return true;
    }
    virtual bool ReadData(DataReader * reader, std::string & outError) override
    {
        Values.clear();
        MaybeValue<unsigned int> tryNumbValues = reader->ReadUInt(outError);
        if (!tryNumbValues.HasValue())
        {
            outError = "Error reading in the number of textures: " + outError;
            return false;
        }
        Values.reserve(tryNumbValues.GetValue());

        for (unsigned int i = 0; i < tryNumbValues.GetValue(); ++i)
        {
            KVPType kvp;
            if (!reader->ReadDataStructure(kvp, outError))
            {
                outError = "Error reading in the " + std::to_string(i + 1) + "-th texture: " + outError;
                return false;
            }

            Values[kvp.Key] = kvp.Value;
        }

        return true;
    }
};
struct SerializedAssetPackageMaterialMap : public ISerializable
{
public:

    std::unordered_map<std::string, AssetPackage::SerializedMatPtr> Values;

    virtual bool WriteData(DataWriter * writer, std::string & outError) const override
    {
        if (!writer->WriteUInt(Values.size(), "Total Number of Materials", outError))
        {
            outError = "Error writing out the number of materials (" +
                           std::to_string(Values.size()) + ": " + outError;
            return false;
        }
        for (auto el = Values.begin(); el != Values.end(); ++el)
        {
            KVPMaterial kvp;
            kvp.Key = el->first;
            kvp.Value = el->second;
            if (!writer->WriteDataStructure(kvp, "Material", outError))
            {
                outError = "Error writing out the material '" + kvp.Key + "': " + outError;
                return false;
            }
        }

        return true;
    }
    virtual bool ReadData(DataReader * reader, std::string & outError) override
    {
        Values.clear();
        MaybeValue<unsigned int> tryNumbValues = reader->ReadUInt(outError);
        if (!tryNumbValues.HasValue())
        {
            outError = "Error reading in the number of materials: " + outError;
            return false;
        }
        Values.reserve(tryNumbValues.GetValue());

        for (unsigned int i = 0; i < tryNumbValues.GetValue(); ++i)
        {
            KVPMaterial kvp;
            if (!reader->ReadDataStructure(kvp, outError))
            {
                outError = "Error reading in the " + std::to_string(i + 1) + "-th material: " + outError;
                return false;
            }

            Values[kvp.Key] = kvp.Value;
        }


        return true;
    }
};
struct SerializedAssetPackageStringMap : public ISerializable
{
public:

    std::unordered_map<std::string, std::string> Values;

    virtual bool WriteData(DataWriter * writer, std::string & outError) const override
    {
        if (!writer->WriteUInt(Values.size(), "Total Number of Samplers", outError))
        {
            outError = "Error writing out the number of samplers (" +
                           std::to_string(Values.size()) + ": " + outError;
            return false;
        }
        for (auto el = Values.begin(); el != Values.end(); ++el)
        {
            KVPString kvp;
            kvp.Key = el->first;
            kvp.Value = el->second;
            if (!writer->WriteDataStructure(kvp, "Sampler to Texture", outError))
            {
                outError = "Error writing out the association '" +
                               kvp.Key + "'->'" + kvp.Value + "': " + outError;
                return false;
            }
        }

        return true;
    }
    virtual bool ReadData(DataReader * reader, std::string & outError) override
    {
        Values.clear();
        MaybeValue<unsigned int> tryNumbValues = reader->ReadUInt(outError);
        if (!tryNumbValues.HasValue())
        {
            outError = "Error reading in the number of textures: " + outError;
            return false;
        }
        Values.reserve(tryNumbValues.GetValue());

        for (unsigned int i = 0; i < tryNumbValues.GetValue(); ++i)
        {
            KVPString kvp;
            if (!reader->ReadDataStructure(kvp, outError))
            {
                outError = "Error reading in the " + std::to_string(i + 1) + "-th association: " + outError;
                return false;
            }

            Values[kvp.Key] = kvp.Value;
        }

        return true;
    }
};
struct SerializedAssetPackageVectorFMap : public ISerializable
{
public:

    std::unordered_map<std::string, VectorF> Values;

    virtual bool WriteData(DataWriter * writer, std::string & outError) const override
    {
        if (!writer->WriteUInt(Values.size(), "Total Number of Uniforms", outError))
        {
            outError = "Error writing out the number of uniforms (" +
                           std::to_string(Values.size()) + ": " + outError;
            return false;
        }
        for (auto el = Values.begin(); el != Values.end(); ++el)
        {
            KVPVectorF kvp;
            kvp.Key = el->first;
            kvp.Value = el->second;
            if (!writer->WriteDataStructure(kvp, "Default Uniform Value", outError))
            {
                outError = "Error writing out the default value for uniform '" +
                               kvp.Key + "', " + DebugAssist::ToString(kvp.Value) + ": " + outError;
                return false;
            }
        }

        return true;
    }
    virtual bool ReadData(DataReader * reader, std::string & outError) override
    {
        Values.clear();
        MaybeValue<unsigned int> tryNumbValues = reader->ReadUInt(outError);
        if (!tryNumbValues.HasValue())
        {
            outError = "Error reading in the number of uniforms: " + outError;
            return false;
        }
        Values.reserve(tryNumbValues.GetValue());

        for (unsigned int i = 0; i < tryNumbValues.GetValue(); ++i)
        {
            KVPVectorF kvp;
            if (!reader->ReadDataStructure(kvp, outError))
            {
                outError = "Error reading in the " + std::to_string(i + 1) + "-th uniform: " + outError;
                return false;
            }

            Values[kvp.Key] = kvp.Value;
        }

        return true;
    }
};


bool AssetPackage::WriteData(DataWriter * writer, std::string & outError) const
{
    //First write all the textures.
    SerializedAssetPackageTexMap<Texture2DPtr, KVPTex2> texes2;
    SerializedAssetPackageTexMap<Texture3DPtr, KVPTex3> texes3;
    SerializedAssetPackageTexMap<TextureCubePtr, KVPTexCube> texesCube;
    texes2.Values = Tex2s;
    texes3.Values = Tex3s;
    texesCube.Values = TexCubes;
    if (!writer->WriteDataStructure(texes2, "2D Textures", outError))
    {
        outError = "Error writing out the 2D textures: " + outError;
        return false;
    }
    if (!writer->WriteDataStructure(texes3, "3D Textures", outError))
    {
        outError = "Error writing out the 3D textures: " + outError;
        return false;
    }
    if (!writer->WriteDataStructure(texesCube, "Cubemap Textures", outError))
    {
        outError = "Error writing out the Cubemap textures: " + outError;
        return false;
    }

    //Now write out the materials.
    SerializedAssetPackageMaterialMap materials;
    materials.Values = Materials;
    if (!writer->WriteDataStructure(materials, "Materials", outError))
    {
        outError = "Error writing out the materials: " + outError;
        return false;
    }

    //Finally, write out the uniform default values.
    SerializedAssetPackageStringMap texSamplers;
    texSamplers.Values = DefaultTextureSamplers;
    if (!writer->WriteDataStructure(texSamplers, "Default Texture Samplers", outError))
    {
        outError = "Error writing out the default texture samplers: " + outError;
        return false;
    }
    SerializedAssetPackageVectorFMap uniformVals;
    uniformVals.Values = DefaultUniformValues;
    if (!writer->WriteDataStructure(uniformVals, "Default Uniform Values", outError))
    {
        outError = "Error writing out the default uniform values: " + outError;
        return false;
    }

    return true;
}
bool AssetPackage::ReadData(DataReader * reader, std::string & outError)
{
    SerializedAssetPackageTexMap<Texture2DPtr, KVPTex2> texes2;
    if (!reader->ReadDataStructure(texes2, outError))
    {
        outError = "Error reading in the 2D Textures: " + outError;
        return false;
    }
    Tex2s = texes2.Values;

    SerializedAssetPackageTexMap<Texture3DPtr, KVPTex3> texes3;
    if (!reader->ReadDataStructure(texes3, outError))
    {
        outError = "Error reading in the 3D Textures: " + outError;
        return false;
    }
    Tex3s = texes3.Values;

    SerializedAssetPackageTexMap<TextureCubePtr, KVPTexCube> texesCube;
    if (!reader->ReadDataStructure(texesCube, outError))
    {
        outError = "Error reading in the Cubemap Textures: " + outError;
        return false;
    }
    TexCubes = texesCube.Values;

    SerializedAssetPackageMaterialMap materials;
    if (!reader->ReadDataStructure(materials, outError))
    {
        outError = "Error reading in the materials: " + outError;
        return false;
    }
    Materials = materials.Values;


    SerializedAssetPackageStringMap texSamplers;
    if (!reader->ReadDataStructure(texSamplers, outError))
    {
        outError = "Error reading in the default texture samplers: " + outError;
        return false;
    }
    DefaultTextureSamplers = texSamplers.Values;

    SerializedAssetPackageVectorFMap uniformVals;
    if (!reader->ReadDataStructure(uniformVals, outError))
    {
        outError = "Error reading in the default uniform values: " + outError;
        return false;
    }
    DefaultUniformValues = uniformVals.Values;


    return true;
}