#include "SerializationWrappers.h"


#define IMPL_WRITE(typeName) void typeName ## _Writable::WriteData(DataWriter* writer) const
#define IMPL_READ(typeName) void typeName ## _Readable::ReadData(DataReader* reader)


#pragma region Vector serializers


#define DO_FOR_ALL_VECTORS(toDo) \
    toDo(f, Float) \
    toDo(b, Byte) \
    toDo(i, Int) \
    toDo(u, UInt)

#define IMPL_WRITE_V2(vLetter, writeName) \
    IMPL_WRITE(Vector2 ## vLetter) \
    { \
        writer->Write ## writeName(Value.x, "x"); \
        writer->Write ## writeName(Value.y, "y"); \
    }
#define IMPL_READ_V2(vLetter, readName) \
    IMPL_READ(Vector2 ## vLetter) \
    { \
        reader->Read ## readName(Value.x); \
        reader->Read ## readName(Value.y); \
    }
DO_FOR_ALL_VECTORS(IMPL_WRITE_V2)
DO_FOR_ALL_VECTORS(IMPL_READ_V2)


#define IMPL_WRITE_V3(vLetter, writeName) \
    IMPL_WRITE(Vector3 ## vLetter) \
    { \
        writer->Write ## writeName(Value.x, "x"); \
        writer->Write ## writeName(Value.y, "y"); \
        writer->Write ## writeName(Value.z, "z"); \
    }
#define IMPL_READ_V3(vLetter, readName) \
    IMPL_READ(Vector3 ## vLetter) \
    { \
        reader->Read ## readName(Value.x); \
        reader->Read ## readName(Value.y); \
        reader->Read ## readName(Value.z); \
    }
DO_FOR_ALL_VECTORS(IMPL_WRITE_V3)
DO_FOR_ALL_VECTORS(IMPL_READ_V3)

#define IMPL_WRITE_V4(vLetter, writeName) \
    IMPL_WRITE(Vector4 ## vLetter) \
    { \
        writer->Write ## writeName(Value.x, "x"); \
        writer->Write ## writeName(Value.y, "y"); \
        writer->Write ## writeName(Value.z, "z"); \
        writer->Write ## writeName(Value.w, "w"); \
    }
#define IMPL_READ_V4(vLetter, readName) \
    IMPL_READ(Vector4 ## vLetter) \
    { \
        reader->Read ## readName(Value.x); \
        reader->Read ## readName(Value.y); \
        reader->Read ## readName(Value.z); \
        reader->Read ## readName(Value.w); \
    }
DO_FOR_ALL_VECTORS(IMPL_WRITE_V4)
DO_FOR_ALL_VECTORS(IMPL_READ_V4)


#pragma endregion


#pragma region Math serializers


IMPL_WRITE(Matrix4f)
{
    std::string name = "0x0";

    for (unsigned int y = 0; y < 4; ++y)
    {
        char rowNumber = '0' + (char)y;
        name[2] = rowNumber;

        for (unsigned int x = 0; x < 4; ++x)
        {
            char colNumber = '0' + (char)x;
            name[0] = colNumber;

            writer->WriteFloat(Value[Vector2u(x, y)], name);
        }
    }
}
IMPL_READ(Matrix4f)
{
    for (unsigned int y = 0; y < 4; ++y)
    {
        for (unsigned int x = 0; x < 4; ++x)
        {
            reader->ReadFloat(Value[Vector2u(x, y)]);
        }
    }
}

IMPL_WRITE(Quaternion)
{
    writer->WriteFloat(Value.x, "x");
    writer->WriteFloat(Value.y, "y");
    writer->WriteFloat(Value.z, "z");
    writer->WriteFloat(Value.w, "w");
}
IMPL_READ(Quaternion)
{
    reader->ReadFloat(Value.x);
    reader->ReadFloat(Value.y);
    reader->ReadFloat(Value.z);
    reader->ReadFloat(Value.w);
}

IMPL_WRITE(Interval)
{
    writer->WriteFloat(Value.GetCenter(), "Center");
    writer->WriteFloat(Value.GetRange(), "Range");
}
IMPL_READ(Interval)
{
    float center, range;
    reader->ReadFloat(center);
    reader->ReadFloat(range);

    Value = Interval(center, range);
}


IMPL_WRITE(VectorF)
{
    writer->WriteUInt(Value.GetSize(), "NComponents");
    for (unsigned int i = 0; i < Value.GetSize(); ++i)
    {
        writer->WriteFloat(Value.GetValue()[i], std::to_string(i));
    }
}
IMPL_READ(VectorF)
{
    unsigned int size;
    reader->ReadUInt(size);

    float values[4];
    for (unsigned int i = 0; i < size; ++i)
    {
        reader->ReadFloat(values[i]);
    }

    Value.SetValue(size, values);
}

IMPL_WRITE(VectorI)
{
    writer->WriteUInt(Value.GetSize(), "NComponents");
    for (unsigned int i = 0; i < Value.GetSize(); ++i)
    {
        writer->WriteInt(Value.GetValue()[0], std::to_string(i));
    }
}
IMPL_READ(VectorI)
{
    unsigned int size;
    reader->ReadUInt(size);

    int values[4];
    for (unsigned int i = 0; i < size; ++i)
    {
        reader->ReadInt(values[i]);
    }

    Value.SetValue(size, values);
}


#pragma endregion


IMPL_WRITE(RenderIOAttributes_Attribute)
{
    writer->WriteString(Value.Name, "Name");
    writer->WriteUInt(Value.Size, "Number of components");
    writer->WriteBool(Value.IsNormalized, "Needs to be normalized?");
}
IMPL_READ(RenderIOAttributes_Attribute)
{
    Value.Name = "";
    reader->ReadString(Value.Name);
    reader->ReadUInt(Value.Size);
    reader->ReadBool(Value.IsNormalized);
}

IMPL_WRITE(RenderIOAttributes)
{
    typedef RenderIOAttributes::Attribute Attr;
    typedef RenderIOAttributes_Attribute_Writable AttrWriter;
    

#pragma warning(disable: 4100)
    writer->WriteCollection([](DataWriter* writer, const void* elementToWrite,
                               unsigned int index, void* pData)
                               {
                                   AttrWriter write(*((Attr*)elementToWrite));
                                   writer->WriteDataStructure(write, std::to_string(index));
                               },
                               "Attributes", sizeof(Attr), Value.GetAttributes().data(),
                               Value.GetNumbAttributes());

#pragma warning(default: 4100)
}
IMPL_READ(RenderIOAttributes)
{
    typedef RenderIOAttributes::Attribute Attr;
    typedef RenderIOAttributes_Attribute_Readable AttrReader;
    
#pragma warning(disable: 4100)
    std::vector<Attr> newData;
    reader->ReadCollection([](DataReader* reader, void* pCollection, unsigned int index, void* pData)
                           {
                               std::vector<Attr>* attrs = (std::vector<Attr>*)pCollection;
                               reader->ReadDataStructure(AttrReader(attrs->operator[](index)));
                           },
                           [](void* pCollection, unsigned int newSize)
                           {
                               std::vector<Attr>* attrs = (std::vector<Attr>*)pCollection;
                               attrs->resize(newSize);
                           },
                           &newData);
#pragma warning(default: 4100)

    Value.SetAttributes(newData);
}


#pragma region Uniform serializers

//Note that the "Location" values (and texture handle values) are not serialized;
//    they are dependent on the GLSL shader and compiler.

IMPL_WRITE(UniformValueF)
{
    writer->WriteString(Value.Name, "Name");
    writer->WriteUInt(Value.NData, "Number of floats");
    for (unsigned int i = 0; i < Value.NData; ++i)
    {
        writer->WriteFloat(Value.Value[i],
                           (i == 0 ? "X" : (i == 1 ? "Y" : (i == 2 ? "Z" : "W"))));
    }
}
IMPL_READ(UniformValueF)
{
    reader->ReadString(Value.Name);
    reader->ReadUInt(Value.NData);
    for (unsigned int i = 0; i < Value.NData; ++i)
    {
        reader->ReadFloat(Value.Value[i]);
    }
}
IMPL_WRITE(UniformValueI)
{
    writer->WriteString(Value.Name, "Name");
    writer->WriteUInt(Value.NData, "Number of ints");
    for (unsigned int i = 0; i < Value.NData; ++i)
    {
        writer->WriteInt(Value.Value[i],
                         (i == 0 ? "X" : (i == 1 ? "Y" : (i == 2 ? "Z" : "W"))));
    }
}
IMPL_READ(UniformValueI)
{
    reader->ReadString(Value.Name);
    reader->ReadUInt(Value.NData);
    for (unsigned int i = 0; i < Value.NData; ++i)
    {
        reader->ReadInt(Value.Value[i]);
    }
}
IMPL_WRITE(UniformValueArrayF)
{
    writer->WriteString(Value.Name, "Name");
    writer->WriteUInt(Value.NComponentsPerValue, "Number of floats per vector");
    writer->WriteUInt(Value.NValues, "Number of vectors");
    for (unsigned int i = 0; i < Value.NValues; ++i)
    {
        const float* start = &Value.Values[i * Value.NComponentsPerValue];
        switch (Value.NComponentsPerValue)
        {
            case 1:
                writer->WriteFloat(*start, "Value");
                break;
            case 2:
                writer->WriteDataStructure(Vector2f_Writable(*(Vector2f*)start), "Value");
                break;
            case 3:
                writer->WriteDataStructure(Vector3f_Writable(*(Vector3f*)start), "Value");
                break;
            case 4:
                writer->WriteDataStructure(Vector4f_Writable(*(Vector4f*)start), "Value");
                break;

            default:
                assert(false);
        }
    }
}
IMPL_READ(UniformValueArrayF)
{
    if (Value.Values != 0)
    {
        delete Value.Values;
    }
    Value.Location = -1;

    reader->ReadString(Value.Name);
    reader->ReadUInt(Value.NComponentsPerValue);
    reader->ReadUInt(Value.NValues);

    Value.Values = new float[Value.NValues * Value.NComponentsPerValue];

    for (unsigned int i = 0; i < Value.NValues; ++i)
    {
        float* start = &Value.Values[i * Value.NComponentsPerValue];
        switch (Value.NComponentsPerValue)
        {
            case 1:
                reader->ReadFloat(*start);
                break;
            case 2:
                reader->ReadDataStructure(Vector2f_Readable(*(Vector2f*)start));
                break;
            case 3:
                reader->ReadDataStructure(Vector3f_Readable(*(Vector3f*)start));
                break;
            case 4:
                reader->ReadDataStructure(Vector4f_Readable(*(Vector4f*)start));
                break;

            default:
                assert(false);
        }
    }
}
IMPL_WRITE(UniformValueArrayI)
{
    writer->WriteString(Value.Name, "Name");
    writer->WriteUInt(Value.NComponentsPerValue, "Number of ints per vector");
    writer->WriteUInt(Value.NValues, "Number of vectors");
    for (unsigned int i = 0; i < Value.NValues; ++i)
    {
        const int* start = &Value.Values[i * Value.NComponentsPerValue];
        switch (Value.NComponentsPerValue)
        {
            case 1:
                writer->WriteInt(*start, "Value");
                break;
            case 2:
                writer->WriteDataStructure(Vector2i_Writable(*(Vector2i*)start), "Value");
                break;
            case 3:
                writer->WriteDataStructure(Vector3i_Writable(*(Vector3i*)start), "Value");
                break;
            case 4:
                writer->WriteDataStructure(Vector4i_Writable(*(Vector4i*)start), "Value");
                break;

            default:
                assert(false);
        }
    }
}
IMPL_READ(UniformValueArrayI)
{
    if (Value.Values != 0)
    {
        delete Value.Values;
    }
    Value.Location = -1;

    reader->ReadString(Value.Name);
    reader->ReadUInt(Value.NComponentsPerValue);
    reader->ReadUInt(Value.NValues);

    Value.Values = new int[Value.NValues * Value.NComponentsPerValue];

    for (unsigned int i = 0; i < Value.NValues; ++i)
    {
        int* start = &Value.Values[i * Value.NComponentsPerValue];
        switch (Value.NComponentsPerValue)
        {
            case 1:
                reader->ReadInt(*start);
                break;
            case 2:
                reader->ReadDataStructure(Vector2i_Readable(*(Vector2i*)start));
                break;
            case 3:
                reader->ReadDataStructure(Vector3i_Readable(*(Vector3i*)start));
                break;
            case 4:
                reader->ReadDataStructure(Vector4i_Readable(*(Vector4i*)start));
                break;

            default:
                assert(false);
        }
    }
}
IMPL_WRITE(UniformValueMatrix4f)
{
    writer->WriteString(Value.Name, "Name");
    writer->WriteDataStructure(Matrix4f_Writable(Value.Value), "Matrix");
}
IMPL_READ(UniformValueMatrix4f)
{
    Value.Location = -1;

    reader->ReadString(Value.Name);
    reader->ReadDataStructure(Matrix4f_Readable(Value.Value));
}
IMPL_WRITE(UniformValueSampler2D)
{
    writer->WriteString(Value.Name, "Name");
}
IMPL_READ(UniformValueSampler2D)
{
    Value.Location = -1;
    Value.Texture = 0;

    reader->ReadString(Value.Name);
}
IMPL_WRITE(UniformValueSampler3D)
{
    writer->WriteString(Value.Name, "Name");
}
IMPL_READ(UniformValueSampler3D)
{
    Value.Location = -1;
    Value.Texture = 0;

    reader->ReadString(Value.Name);
}
IMPL_WRITE(UniformValueSamplerCubemap)
{
    writer->WriteString(Value.Name, "Name");
}
IMPL_READ(UniformValueSamplerCubemap)
{
    Value.Location = -1;
    Value.Texture = 0;

    reader->ReadString(Value.Name);
}

IMPL_WRITE(SubroutineDefinition_Parameter)
{
    writer->WriteString(Value.Name, "Name");
    writer->WriteUInt(Value.Size, "Number of components");
    writer->WriteBool(Value.IsInt, "Is int vector instead of float?");
}
IMPL_READ(SubroutineDefinition_Parameter)
{
    reader->ReadString(Value.Name);
    reader->ReadUInt(Value.Size);
    reader->ReadBool(Value.IsInt);
}
IMPL_WRITE(SubroutineDefinition)
{
    writer->WriteString(Value.Name, "Name");
    writer->WriteUInt(Value.ReturnValueSize, "Return value size");
    writer->WriteString(ShaderTypeToString(Value.Shader), "Shader type");

    writer->WriteCollection([](DataWriter* writer, const void* elementToWrite,
                               unsigned int elIndex, void* pData)
                            {
                                auto& param = *(const SubroutineDefinition::Parameter*)elementToWrite;
                                writer->WriteDataStructure(SubroutineDefinition_Parameter_Writable(param),
                                                           "Param " + std::to_string(elIndex + 1));
                            },
                            "Parameters", sizeof(SubroutineDefinition::Parameter),
                            Value.Params.data(), Value.Params.size());
}
IMPL_READ(SubroutineDefinition)
{
    reader->ReadString(Value.Name);
    reader->ReadUInt(Value.ReturnValueSize);
    
    std::string shaderType;
    reader->ReadString(shaderType);
    Value.Shader = ShaderTypeFromString(shaderType);

    reader->ReadCollection([](DataReader* reader, void* pCollection, unsigned int elIndex, void* pData)
                           {
                               auto collection = (std::vector<SubroutineDefinition::Parameter>*)pCollection;
                               auto& element = (*collection)[elIndex];
                               reader->ReadDataStructure(SubroutineDefinition_Parameter_Readable(element));
                           },
                           [](void* pCollection, unsigned int newSize)
                           {
                               auto collection = (std::vector<SubroutineDefinition::Parameter>*)pCollection;
                               collection->resize(newSize);
                           },
                           &Value.Params);
}
IMPL_WRITE(UniformValueSubroutine)
{
    writer->WriteString(Value.Name, "Name");
    writer->WriteDataStructure(SubroutineDefinition_Writable(Value.Definition), "Signature definition");
    writer->WriteCollection([](DataWriter* writer, const void* elementToWrite,
                               unsigned int elIndex, void* pData)
                            {
                                const std::string& el = *(const std::string*)elementToWrite;
                                writer->WriteString(el, "Possible value " + std::to_string(elIndex + 1));
                            },
                            "Possible values", sizeof(std::string),
                            Value.PossibleValues.data(), Value.PossibleValues.size());
}
IMPL_READ(UniformValueSubroutine)
{
    reader->ReadString(Value.Name);
    reader->ReadDataStructure(SubroutineDefinition_Readable(Value.Definition));
    reader->ReadCollection([](DataReader* reader, void* pCollection, unsigned int elIndex, void* pData)
                           {
                               auto collection = (std::vector<std::string>*)pCollection;
                               auto& element = (*collection)[elIndex];
                               reader->ReadString(element);
                           },
                           [](void* pCollection, unsigned int newSize)
                           {
                               auto collection = (std::vector<std::string>*)pCollection;
                               collection->resize(newSize);
                           },
                           &Value.PossibleValues);

    Value.Location = -1;
    Value.PossibleValueIDs = std::vector<RenderObjHandle>();
    Value.PossibleValueIDs.resize(Value.PossibleValues.size());
    Value.ValueIndex = 0;
}

IMPL_WRITE(UniformDictionary)
{
#define WRITE_UNIF_DICT(Name, ValueType) \
    writer->WriteDictionary<std::string, ValueType>( \
                Value.Name, \
                [](DataWriter* write, const std::string* k, const ValueType* val, void* pDat) \
                { \
                    write->WriteString(*k, "Name"); \
                    write->WriteDataStructure(ValueType ## _Writable(*val), "Value"); \
                }, \
                #Name);

    WRITE_UNIF_DICT(Floats, UniformValueF);
    WRITE_UNIF_DICT(Ints, UniformValueI);
    WRITE_UNIF_DICT(FloatArrays, UniformValueArrayF);
    WRITE_UNIF_DICT(IntArrays, UniformValueArrayI);
    WRITE_UNIF_DICT(Matrices, UniformValueMatrix4f);
    WRITE_UNIF_DICT(Texture2Ds, UniformValueSampler2D);
    WRITE_UNIF_DICT(Texture3Ds, UniformValueSampler3D);
    WRITE_UNIF_DICT(TextureCubemaps, UniformValueSamplerCubemap);
    WRITE_UNIF_DICT(Subroutines, UniformValueSubroutine);

#undef WRITE_UNIF_DICT
}
IMPL_READ(UniformDictionary)
{
#define READ_UNIF_DICT(Name, ValueType) \
    reader->ReadDictionary<std::string, ValueType>( \
                Value.Name, \
                [](DataReader* reader, std::string* k, ValueType* val, void* pDat) \
                { \
                    reader->ReadString(*k); \
                    reader->ReadDataStructure(ValueType ## _Readable(*val)); \
                });

    READ_UNIF_DICT(Floats, UniformValueF);
    READ_UNIF_DICT(Ints, UniformValueI);
    READ_UNIF_DICT(FloatArrays, UniformValueArrayF);
    READ_UNIF_DICT(IntArrays, UniformValueArrayI);
    READ_UNIF_DICT(Matrices, UniformValueMatrix4f);
    READ_UNIF_DICT(Texture2Ds, UniformValueSampler2D);
    READ_UNIF_DICT(Texture3Ds, UniformValueSampler3D);
    READ_UNIF_DICT(TextureCubemaps, UniformValueSamplerCubemap);
    READ_UNIF_DICT(Subroutines, UniformValueSubroutine);

#undef READ_UNIF_DICT
}


#pragma endregion


#pragma region Texture serializers


IMPL_WRITE(TextureSampleSettings2D)
{
    writer->WriteBool(Value.MinFilter == FT_LINEAR, "Min filter high quality?");
    writer->WriteBool(Value.MagFilter == FT_LINEAR, "Mag filter high quality?");
    writer->WriteBool(Value.HorzWrap == WT_WRAP, "Wrap vorizontal UVs?");
    writer->WriteBool(Value.VertWrap == WT_WRAP, "Wrap vertical UVs?");
}
IMPL_READ(TextureSampleSettings2D)
{
    bool highQualityMin, highQualityMag, wrapHorz, wrapVert;
    reader->ReadBool(highQualityMin);
    reader->ReadBool(highQualityMag);
    reader->ReadBool(wrapHorz);
    reader->ReadBool(wrapVert);
    Value = TextureSampleSettings2D((highQualityMin ? FT_LINEAR : FT_NEAREST),
                                    (highQualityMag ? FT_LINEAR : FT_NEAREST),
                                    (wrapHorz ? WT_WRAP : WT_CLAMP),
                                    (wrapVert ? WT_WRAP : WT_CLAMP));
}

IMPL_WRITE(TextureSampleSettings3D)
{
    writer->WriteBool(Value.MinFilter == FT_LINEAR, "Min filter high quality?");
    writer->WriteBool(Value.MagFilter == FT_LINEAR, "Mag filter high quality?");
    writer->WriteBool(Value.XWrap == WT_WRAP, "Wrap tex coords along X?");
    writer->WriteBool(Value.YWrap == WT_WRAP, "Wrap tex coords along Y?");
    writer->WriteBool(Value.ZWrap == WT_WRAP, "Wrap tex coords along Z?");
}
IMPL_READ(TextureSampleSettings3D)
{
    bool highQualityMin, highQualityMag, wrapX, wrapY, wrapZ;
    reader->ReadBool(highQualityMin);
    reader->ReadBool(highQualityMag);
    reader->ReadBool(wrapX);
    reader->ReadBool(wrapY);
    reader->ReadBool(wrapZ);

    Value = TextureSampleSettings3D(highQualityMin ? FT_LINEAR : FT_NEAREST,
                                    highQualityMag ? FT_LINEAR : FT_NEAREST,
                                    wrapX ? WT_WRAP : WT_CLAMP,
                                    wrapY ? WT_WRAP : WT_CLAMP,
                                    wrapY ? WT_WRAP : WT_CLAMP);
}



//Have to use braces around each case because of compiler bullshit
#define GET_AND_WRITE_TEX2(arrayTemplateArg, pixelColorType, pixelType, pixelTypeStr) \
    case pixelType: { \
        writer->WriteString(pixelTypeStr, "Pixel Type"); \
        Array2D<arrayTemplateArg> dat(Value.GetWidth(), Value.GetHeight()); \
        Value.Get ## pixelColorType ## Data(dat); \
        writer->WriteBytes((unsigned char*)dat.GetArray(), \
                           sizeof(arrayTemplateArg) * dat.GetNumbElements(), "Data"); \
        break; }
#define READ_TEX2(pixelSizeStr, arrayTemplateArg, pixelColorType, pixelSize) \
    if (pixelType == std::string(pixelSizeStr)) \
    { \
        Array2D<arrayTemplateArg> dat(width, height); \
        std::vector<unsigned char> byteData; \
        byteData.reserve(width * height * sizeof(arrayTemplateArg)); \
        reader->ReadBytes(byteData); \
        memcpy(dat.GetArray(), byteData.data(), byteData.size() * sizeof(arrayTemplateArg)); \
        byteData.clear(); \
        Value.Create(settings, useMips, pixelSize); \
        Value.Set ## pixelColorType ## Data(dat, pixelSize); \
    }

IMPL_WRITE(MTexture2D)
{
    writer->WriteUInt(Value.GetWidth(), "Width");
    writer->WriteUInt(Value.GetHeight(), "Height");

    writer->WriteDataStructure(TextureSampleSettings2D_Writable(Value.GetSamplingSettings()),
                               "Sampling Settings");

    writer->WriteBool(Value.UsesMipmaps(), "Use Mipmapping?");

    switch (Value.GetPixelSize())
    {
        GET_AND_WRITE_TEX2(Vector4b, Color, PS_8U, "RGBA UInt 8");
        GET_AND_WRITE_TEX2(Vector4f, Color, PS_16U, "RGBA UInt 16");
        GET_AND_WRITE_TEX2(Vector4f, Color, PS_16F, "RGBA Float 16");
        GET_AND_WRITE_TEX2(Vector4f, Color, PS_32F, "RGBA Float 32");
        GET_AND_WRITE_TEX2(unsigned char, Greyscale, PS_8U_GREYSCALE, "Greyscale UInt 8");
        GET_AND_WRITE_TEX2(float, Greyscale, PS_16U_GREYSCALE, "Greyscale UInt 16");
        GET_AND_WRITE_TEX2(float, Greyscale, PS_32F_GREYSCALE, "Greyscale Float 32");
        GET_AND_WRITE_TEX2(float, Depth, PS_16U_DEPTH, "Depth UInt 16");
        GET_AND_WRITE_TEX2(float, Depth, PS_24U_DEPTH, "Depth UInt 24");
        GET_AND_WRITE_TEX2(float, Depth, PS_32F_DEPTH, "Depth Float 32");

        default:
            assert(false);
    }
}
IMPL_READ(MTexture2D)
{
    unsigned int width, height;
    reader->ReadUInt(width);
    reader->ReadUInt(height);

    TextureSampleSettings2D settings;
    reader->ReadDataStructure(TextureSampleSettings2D_Readable(settings));
    Value.SetSettings(settings);

    bool useMips;
    reader->ReadBool(useMips);

    std::string pixelType;
    reader->ReadString(pixelType);
    READ_TEX2("RGBA UInt 8", Vector4b, Color, PS_8U)
    READ_TEX2("RGBA UInt 16", Vector4f, Color, PS_16U)
    READ_TEX2("RGBA Float 16", Vector4f, Color, PS_16F)
    READ_TEX2("RGBA Float 32", Vector4f, Color, PS_32F)
    READ_TEX2("Greyscale UInt 8", unsigned char, Greyscale, PS_8U_GREYSCALE)
    READ_TEX2("Greyscale UInt 16", float, Greyscale, PS_16U_GREYSCALE)
    READ_TEX2("Greyscale Float 32", float, Greyscale, PS_32F_GREYSCALE)
    READ_TEX2("Depth UInt 16", float, Depth, PS_16U_DEPTH)
    READ_TEX2("Depth UInt 24", float, Depth, PS_24U_DEPTH)
    READ_TEX2("Depth Float 32", float, Depth, PS_32F_DEPTH)
}


#define GET_AND_WRITE_TEX3(arrayTemplateArg, pixelColorType, pixelType, pixelTypeStr) \
    case pixelType: { \
        writer->WriteString(pixelTypeStr, "Pixel Type"); \
        Array3D<arrayTemplateArg> dat(Value.GetWidth(), Value.GetHeight(), Value.GetDepth()); \
        Value.Get ## pixelColorType ## Data(dat); \
        writer->WriteBytes((unsigned char*)dat.GetArray(), \
                           sizeof(arrayTemplateArg) * dat.GetNumbElements(), "Data"); \
        break; }
#define READ_TEX3(pixelSizeStr, arrayTemplateArg, pixelColorType, pixelSize) \
    if (pixelType == std::string(pixelSizeStr)) \
    { \
        Array3D<arrayTemplateArg> dat(width, height, depth); \
        std::vector<unsigned char> byteData; \
        byteData.reserve(width * height * depth * sizeof(arrayTemplateArg)); \
        reader->ReadBytes(byteData); \
        memcpy(dat.GetArray(), byteData.data(), byteData.size() * sizeof(arrayTemplateArg)); \
        byteData.clear(); \
        Value.Create(settings, useMips, pixelSize); \
        Value.Set ## pixelColorType ## Data(dat, pixelSize); \
    }

IMPL_WRITE(MTexture3D)
{
    writer->WriteUInt(Value.GetWidth(), "Width");
    writer->WriteUInt(Value.GetHeight(), "Height");
    writer->WriteUInt(Value.GetDepth(), "Depth");

    writer->WriteDataStructure(TextureSampleSettings3D_Writable(Value.GetSamplingSettings()),
                               "Sampling Settings");

    writer->WriteBool(Value.UsesMipmaps(), "Use mipmapping?");

    switch (Value.GetPixelSize())
    {
        GET_AND_WRITE_TEX3(Vector4b, Color, PS_8U, "RGBA UInt 8");
        GET_AND_WRITE_TEX3(Vector4f, Color, PS_16U, "RGBA UInt 16");
        GET_AND_WRITE_TEX3(Vector4f, Color, PS_16F, "RGBA Float 16");
        GET_AND_WRITE_TEX3(Vector4f, Color, PS_32F, "RGBA Float 32");
        GET_AND_WRITE_TEX3(unsigned char, Greyscale, PS_8U_GREYSCALE, "Greyscale UInt 8");
        GET_AND_WRITE_TEX3(float, Greyscale, PS_16U_GREYSCALE, "Greyscale UInt 16");
        GET_AND_WRITE_TEX3(float, Greyscale, PS_32F_GREYSCALE, "Greyscale Float 32");
        GET_AND_WRITE_TEX3(float, Depth, PS_16U_DEPTH, "Depth UInt 16");
        GET_AND_WRITE_TEX3(float, Depth, PS_24U_DEPTH, "Depth UInt 24");
        GET_AND_WRITE_TEX3(float, Depth, PS_32F_DEPTH, "Depth Float 32");

    default:
        assert(false);
    }
}
IMPL_READ(MTexture3D)
{
    unsigned int width, height, depth;
    reader->ReadUInt(width);
    reader->ReadUInt(height);
    reader->ReadUInt(depth);

    TextureSampleSettings3D settings;
    reader->ReadDataStructure(TextureSampleSettings3D_Readable(settings));
    Value.SetSettings(settings);

    bool useMips;
    reader->ReadBool(useMips);

    std::string pixelType;
    reader->ReadString(pixelType);
    READ_TEX3("RGBA UInt 8", Vector4b, Color, PS_8U)
    READ_TEX3("RGBA UInt 16", Vector4f, Color, PS_16U)
    READ_TEX3("RGBA Float 16", Vector4f, Color, PS_16F)
    READ_TEX3("RGBA Float 32", Vector4f, Color, PS_32F)
    READ_TEX3("Greyscale UInt 8", unsigned char, Greyscale, PS_8U_GREYSCALE)
    READ_TEX3("Greyscale UInt 16", float, Greyscale, PS_16U_GREYSCALE)
    READ_TEX3("Greyscale Float 32", float, Greyscale, PS_32F_GREYSCALE)
    READ_TEX3("Depth UInt 16", float, Depth, PS_16U_DEPTH)
    READ_TEX3("Depth UInt 24", float, Depth, PS_24U_DEPTH)
    READ_TEX3("Depth Float 32", float, Depth, PS_32F_DEPTH)
}



#define GET_AND_WRITE_TEXCUBE_FACE(face, arrayTemplateArg, pixelColorType, faceDescriptionStr) \
    Value.GetFace ## pixelColorType(face, dat); \
    writer->WriteBytes((unsigned char*)dat.GetArray(), \
                       sizeof(arrayTemplateArg) * dat.GetNumbElements(), faceDescriptionStr);
#define GET_AND_WRITE_TEXCUBE(arrayTemplateArg, pixelColorType, pixelType, pixelTypeStr) \
    case pixelType: { \
        writer->WriteString(pixelTypeStr, "Pixel Type"); \
        Array2D<arrayTemplateArg> dat(Value.GetWidth(), Value.GetHeight()); \
        GET_AND_WRITE_TEXCUBE_FACE(CTT_X_NEG, arrayTemplateArg, pixelColorType, "Data Negative X") \
        GET_AND_WRITE_TEXCUBE_FACE(CTT_Y_NEG, arrayTemplateArg, pixelColorType, "Data Negative Y") \
        GET_AND_WRITE_TEXCUBE_FACE(CTT_Z_NEG, arrayTemplateArg, pixelColorType, "Data Negative Z") \
        GET_AND_WRITE_TEXCUBE_FACE(CTT_X_POS, arrayTemplateArg, pixelColorType, "Data Positive X") \
        GET_AND_WRITE_TEXCUBE_FACE(CTT_Y_POS, arrayTemplateArg, pixelColorType, "Data Positive Y") \
        GET_AND_WRITE_TEXCUBE_FACE(CTT_Z_POS, arrayTemplateArg, pixelColorType, "Data Positive Z") \
        break; }
#define READ_TEXCUBE_FACE(face, arrayTemplateArg, pixelColorType, updateMips) \
    reader->ReadBytes(byteData); \
    memcpy(dat.GetArray(), byteData.data(), byteData.size() * sizeof(arrayTemplateArg)); \
    byteData.clear(); \
    Value.SetFace ## pixelColorType(face, dat, updateMips);
#define READ_TEXCUBE(pixelSizeStr, arrayTemplateArg, pixelColorType, pixelSize) \
    if (pixelType == std::string(pixelSizeStr)) \
    { \
        Array2D<arrayTemplateArg> dat(width, height); \
        std::vector<unsigned char> byteData; \
        byteData.reserve(width * height * sizeof(arrayTemplateArg)); \
        Value.Create(settings, useMips, pixelSize); \
        READ_TEXCUBE_FACE(CTT_X_NEG, arrayTemplateArg, pixelColorType, false) \
        READ_TEXCUBE_FACE(CTT_Y_NEG, arrayTemplateArg, pixelColorType, false) \
        READ_TEXCUBE_FACE(CTT_Z_NEG, arrayTemplateArg, pixelColorType, false) \
        READ_TEXCUBE_FACE(CTT_X_POS, arrayTemplateArg, pixelColorType, false) \
        READ_TEXCUBE_FACE(CTT_Y_POS, arrayTemplateArg, pixelColorType, false) \
        READ_TEXCUBE_FACE(CTT_Z_POS, arrayTemplateArg, pixelColorType, true) \
    }

IMPL_WRITE(MTextureCubemap)
{
    writer->WriteUInt(Value.GetWidth(), "Width");
    writer->WriteUInt(Value.GetHeight(), "Height");

    writer->WriteDataStructure(TextureSampleSettings3D_Writable(Value.GetSamplingSettings()),
                               "Sampling Settings");

    writer->WriteBool(Value.UsesMipmaps(), "Use mipmapping?");

    switch (Value.GetPixelSize())
    {
        GET_AND_WRITE_TEXCUBE(Vector4b, Color, PS_8U, "RGBA UInt 8");
        GET_AND_WRITE_TEXCUBE(Vector4f, Color, PS_16U, "RGBA UInt 16");
        GET_AND_WRITE_TEXCUBE(Vector4f, Color, PS_16F, "RGBA Float 16");
        GET_AND_WRITE_TEXCUBE(Vector4f, Color, PS_32F, "RGBA Float 32");
        GET_AND_WRITE_TEXCUBE(unsigned char, Greyscale, PS_8U_GREYSCALE, "Greyscale UInt 8");
        GET_AND_WRITE_TEXCUBE(float, Greyscale, PS_16U_GREYSCALE, "Greyscale UInt 16");
        GET_AND_WRITE_TEXCUBE(float, Greyscale, PS_32F_GREYSCALE, "Greyscale Float 32");
        GET_AND_WRITE_TEXCUBE(float, Depth, PS_16U_DEPTH, "Depth UInt 16");
        GET_AND_WRITE_TEXCUBE(float, Depth, PS_24U_DEPTH, "Depth UInt 24");
        GET_AND_WRITE_TEXCUBE(float, Depth, PS_32F_DEPTH, "Depth Float 32");

    default:
        assert(false);
    }
}
IMPL_READ(MTextureCubemap)
{
    unsigned int width, height;
    reader->ReadUInt(width);
    reader->ReadUInt(height);

    TextureSampleSettings3D settings;
    reader->ReadDataStructure(TextureSampleSettings3D_Readable(settings));
    Value.SetSettings(settings);

    bool useMips;
    reader->ReadBool(useMips);

    std::string pixelType;
    reader->ReadString(pixelType);

    READ_TEXCUBE("RGBA UInt 8", Vector4b, Color, PS_8U)
    READ_TEXCUBE("RGBA UInt 16", Vector4f, Color, PS_16U)
    READ_TEXCUBE("RGBA Float 16", Vector4f, Color, PS_16F)
    READ_TEXCUBE("RGBA Float 32", Vector4f, Color, PS_32F)
    READ_TEXCUBE("Greyscale UInt 8", unsigned char, Greyscale, PS_8U_GREYSCALE)
    READ_TEXCUBE("Greyscale UInt 16", float, Greyscale, PS_16U_GREYSCALE)
    READ_TEXCUBE("Greyscale Float 32", float, Greyscale, PS_32F_GREYSCALE)
    READ_TEXCUBE("Depth UInt 16", float, Depth, PS_16U_DEPTH)
    READ_TEXCUBE("Depth UInt 24", float, Depth, PS_24U_DEPTH)
    READ_TEXCUBE("Depth Float 32", float, Depth, PS_32F_DEPTH)
}


#pragma endregion