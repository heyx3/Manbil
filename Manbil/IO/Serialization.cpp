#include "Serialization.h"


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
        writer->WriteFloat(Value.GetValue()[0], std::to_string(i));
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
    reader->ReadCollection([](void* pCollection, unsigned int index, void* pData)
                           {
                               std::vector<Attr>* attrs = (std::vector<Attr>*)pCollection;
                               attrs->insert(attrs->begin() + index, Attr());
                           },
                           [](DataReader* reader, void* pCollection, unsigned int index, void* pData)
                           {
                               std::vector<Attr>* attrs = (std::vector<Attr>*)pCollection;
                               reader->ReadDataStructure(AttrReader(attrs->operator[](index)));
                           },
                           [](void* pCollection, unsigned int reserveSize)
                           {
                               std::vector<Attr>* attrs = (std::vector<Attr>*)pCollection;
                               attrs->reserve(reserveSize);
                           },
                           &newData);
#pragma warning(default: 4100)

    Value.SetAttributes(newData);
}


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