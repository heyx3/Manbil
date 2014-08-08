#include "SFML/Graphics.hpp"

#include "Math/Array3D.h"

#include "IO/XmlSerialization.h"
#include "IO/BinarySerialization.h"

#include "NoiseTest.h"
#include "OpenGLTestWorld.h"
#include "TwoTrianglesWorld.h"
#include "Toys/Voxel/VoxelWorld.h"
#include "Tests/TwoDOpenGLTest.h"
#include "Tests/GUITestWorld.h"
#include "Toys/PlanetSim/PlanetSimWorld.h"


#include <iostream>
using namespace std;
void PauseConsole(void)
{
    char dummy;
    cout << "Enter any character to continue.\n";
    cin >> dummy;
}
void WriteToConsole(const std::string & outStr)
{
    cout << outStr << "\n";
}


#pragma region IO Test

struct TestDataInner : public ISerializable
{
public:

    float Float = 8.9601f;
    int Int = -5;
    unsigned int UInt = 18;
    bool Bool = true;
    unsigned char Byte = 97;
    double Double = 235.13543732;
    std::string String = "OneTwoThree";

    virtual bool ReadData(DataReader * data, std::string & outError) override
    {
        MaybeValue<float> fl = data->ReadFloat(outError);
        if (!fl.HasValue()) return false;

        MaybeValue<int> in = data->ReadInt(outError);
        if (!in.HasValue()) return false;

        MaybeValue<unsigned int> uint = data->ReadUInt(outError);
        if (!uint.HasValue()) return false;

        MaybeValue<bool> bl = data->ReadBool(outError);
        if (!bl.HasValue()) return false;

        MaybeValue<unsigned char> byte = data->ReadByte(outError);
        if (!byte.HasValue()) return false;

        MaybeValue<double> dbl = data->ReadDouble(outError);
        if (!dbl.HasValue()) return false;

        MaybeValue<std::string> str = data->ReadString(outError);
        if (!str.HasValue()) return false;

        Float = fl.GetValue();
        Int = in.GetValue();
        UInt = uint.GetValue();
        Bool = bl.GetValue();
        Byte = byte.GetValue();
        Double = dbl.GetValue();
        String = str.GetValue();
        return true;
    }
    virtual bool WriteData(DataWriter * data, std::string & outError) const override
    {
        if (!data->WriteFloat(Float, "MyFloat", outError))
            return false;
        if (!data->WriteInt(Int, "MyInt", outError))
            return false;
        if (!data->WriteUInt(UInt, "MyUInt", outError))
            return false;
        if (!data->WriteBool(Bool, "MyBool", outError))
            return false;
        if (!data->WriteByte(Byte, "MyByte", outError))
            return false;
        if (!data->WriteDouble(Double, "MyDouble", outError))
            return false;
        if (!data->WriteString(String, "MyString", outError))
            return false;

        return true;
    }
};
struct TestDataOuter : public ISerializable
{
public:
    
    float Floats[4];
    std::string Strings[3];
    std::vector<int> Ints = std::vector<int>();
    TestDataInner InnerStruct = TestDataInner();

    TestDataOuter(void)
    {
        Floats[0] = 1.0f;
        Floats[1] = 2.0f;
        Floats[2] = 3.0f;
        Floats[3] = 4.0f;
        Strings[0] = "Haha";
        Strings[1] = "Hahaha2";
        Strings[2] = "Hahaha3";
    }

    virtual bool ReadData(DataReader * data, std::string & outError) override
    {
        //Floats array.
        std::vector<unsigned char> tryData;
        if (!data->ReadCollection([](void* outColl, unsigned int index, DataReader * read, std::string & outError, void* pData)
                                    {
                                        MaybeValue<float> tryFloat = read->ReadFloat(outError);
                                        if (!tryFloat.HasValue())
                                            return false;

                                        ((float*)outColl)[index] = tryFloat.GetValue();
                                        return true;
                                    }, sizeof(float), outError, tryData))
        {
            outError = "Error reading 'Floats': " + outError;
            return false;
        }
        if (tryData.size() != sizeof(Floats))
        {
            outError = "Size of Floats array is " + std::to_string(sizeof(Floats)) +
                            " bytes but the data read in was " + std::to_string(tryData.size());
            return false;
        }
        memcpy(Floats, tryData.data(), tryData.size());

        //Strings array.
        tryData.clear();
        if (!data->ReadCollection(
            [](void* outColl, unsigned int index, DataReader * read, std::string & outError, void* pData)
            {
                MaybeValue<std::string> tryString = read->ReadString(outError);
                if (!tryString.HasValue())
                    return false;

                ((std::string*)outColl)[index] = tryString.GetValue();
                return true;
            }, sizeof(std::string), outError, tryData))
        {
            outError = "Error reading 'Strings': " + outError;
            return false;
        }
        if (tryData.size() != sizeof(Strings))
        {
            outError = "Size of Strings array is " + std::to_string(sizeof(Strings)) +
                            " bytes but the data read in was " + std::to_string(tryData.size());
        }
        for (int i = 0; i < tryData.size() / sizeof(std::string); ++i)
        {
            Strings[i] = ((std::string*)tryData.data())[i];
        }

        //Ints array.
        tryData.clear();
        if (!data->ReadCollection(
            [](void* outColl, unsigned int index, DataReader * read, std::string & outError, void* pData)
            {
                MaybeValue<int> tryInt = read->ReadInt(outError);
                if (!tryInt.HasValue())
                    return false;

                ((int*)outColl)[index] = tryInt.GetValue();
                return true;
            }, sizeof(int), outError, tryData))
        {
            outError = "Error reading 'Ints': " + outError;
            return false;
        }
        Ints.resize(tryData.size() / sizeof(int));
        memcpy(Ints.data(), tryData.data(), tryData.size());

        //Inner struct.
        if (!data->ReadDataStructure(InnerStruct, outError))
        {
            outError = "Error reading 'InnerStruct': " + outError;
            return false;
        }

        return true;
    }
    virtual bool WriteData(DataWriter * data, std::string & outError) const override
    {
        if (!data->WriteCollection("Floats", [](const void* pCollection, unsigned int index, DataWriter * writer, std::string & outError, void * optionalData)
                                  {
                                      return writer->WriteFloat(((float*)pCollection)[index], std::to_string(index), outError);
                                  }, Floats, sizeof(Floats) / sizeof(float), outError))
        {
            outError = "Error writing 'Floats': " + outError;
            return false;
        }
        if (!data->WriteCollection("Strings", [](const void* pCollection, unsigned int index, DataWriter * writer, std::string & outError, void* optionalData)
                                  {
                                      return writer->WriteString(((std::string*)pCollection)[index], std::to_string(index), outError);
                                  }, Strings, sizeof(Strings) / sizeof(std::string), outError))
        {
            outError = "Error writing 'Strings': " + outError;
            return false;
        }
        if (!data->WriteCollection("Ints", [](const void* pCollection, unsigned int index, DataWriter * writer, std::string & outError, void* optionalData)
                                  {
                                      return writer->WriteInt(((int*)pCollection)[index], std::to_string(index), outError);
                                  }, Ints.data(), Ints.size(), outError))
        {
            outError = "Error writing 'Ints': " + outError;
            return false;
        }
        if (!data->WriteDataStructure(InnerStruct, "InnerStruct", outError))
        {
            outError = "Error writing 'InnerStruct': " + outError;
            return false;
        }

        return true;
    }
};


void PrintStruct(const TestDataOuter & dat)
{
    WriteToConsole("Structure:");

    std::string buildStr;
    for (unsigned int i = 0; i < sizeof(dat.Floats) / sizeof(float); ++i)
    {
        if (i > 0) buildStr += ", ";
        buildStr += std::to_string(dat.Floats[i]);
    }
    WriteToConsole("\tFloats: " + buildStr);

    buildStr.clear();
    for (unsigned int i = 0; i < sizeof(dat.Strings) / sizeof(std::string); ++i)
    {
        if (i > 0) buildStr += ", ";
        buildStr += "\"" + dat.Strings[i] + "\"";
    }
    WriteToConsole("\tStrings: " + buildStr);

    buildStr.clear();
    for (unsigned int i = 0; i < dat.Ints.size(); ++i)
    {
        if (i > 0) buildStr += ", ";
        buildStr += std::to_string(dat.Ints[i]);
    }
    WriteToConsole("\tInts: " + buildStr);

    WriteToConsole("\tInner Struct:");
    WriteToConsole("\t\tFloat: " + std::to_string(dat.InnerStruct.Float));
    WriteToConsole("\t\tInt: " + std::to_string(dat.InnerStruct.Int));
    WriteToConsole("\t\tUInt: " + std::to_string(dat.InnerStruct.UInt));
    WriteToConsole("\t\tBool: " + std::to_string(dat.InnerStruct.Bool));
    WriteToConsole("\t\tByte: " + std::to_string(dat.InnerStruct.Byte));
    WriteToConsole("\t\tDouble: " + std::to_string(dat.InnerStruct.Double));
    WriteToConsole("\t\tString: " + dat.InnerStruct.String);
}
void TestIOStuff()
{
    TestDataOuter dat;
    std::string errorMsg;

    WriteToConsole("Original struct data:");
    PrintStruct(dat);

    XmlWriter xmlWriter;
    if (!xmlWriter.WriteDataStructure(dat, "OuterStruct", errorMsg))
    {
        WriteToConsole("Error writing structure to XML: " + errorMsg);
        PauseConsole();
        return;
    }
    errorMsg = xmlWriter.SaveData("Test_XML_IO.xml");
    if (!errorMsg.empty())
    {
        WriteToConsole("Error saving structure to XML: " + errorMsg);
        PauseConsole();
        return;
    }

    XmlReader xmlReader("Test_XML_IO.xml", errorMsg);
    if (!errorMsg.empty())
    {
        WriteToConsole("Error reading XML data: " + errorMsg);
        PauseConsole();
        return;
    }
    if (!xmlReader.ReadDataStructure(dat, errorMsg))
    {
        WriteToConsole("Error parsing XML data to memory: " + errorMsg);
        PauseConsole();
        return;
    }

    WriteToConsole("New struct data after reading in XML:");
    PrintStruct(dat);
    PauseConsole();


    BinaryWriter binWriter(false, sizeof(TestDataOuter));
    if (!binWriter.WriteDataStructure(dat, "OuterStruct", errorMsg))
    {
        WriteToConsole("Error writing structure in binary: " + errorMsg);
        PauseConsole();
        return;
    }
    errorMsg = binWriter.SaveData("Test_Bin_IO.binFile");
    if (!errorMsg.empty())
    {
        WriteToConsole("Error saving structure to binary: " + errorMsg);
        PauseConsole();
        return;
    }

    BinaryReader binReader(binWriter.EnsureTypeSafety, "Test_Bin_IO.binFile", errorMsg);
    if (!errorMsg.empty())
    {
        WriteToConsole("Error reading binary data: " + errorMsg);
        PauseConsole();
        return;
    }
    if (!binReader.ReadDataStructure(dat, errorMsg))
    {
        WriteToConsole("Error parsing binary data to memory: " + errorMsg);
        PauseConsole();
        return;
    }

    WriteToConsole("New struct data after reading in binary:");
    PrintStruct(dat);
    PauseConsole();
}


#pragma endregion


void TestMaterialIOStuff()
{
    char dummy;

    //Generate the material data.
    SerializedMaterial ser;
    ser.VertexInputs = DrawingQuad::GetAttributeData();
    DataNode::Ptr vertPosOut(new CombineVectorNode(VertexInputNode::GetInstanceName(), 1.0f, "vertPosOutput"));
    ser.MaterialOuts.VertexPosOutput = vertPosOut->GetName();
    ser.MaterialOuts.VertexOutputs.insert(ser.MaterialOuts.VertexOutputs.end(),
                                          ShaderOutput("vOut_Pos", DataLine(VertexInputNode::GetInstanceName(), 0)));
    DataNode::Ptr remapToColor(new RemapNode(FragmentInputNode::GetInstance(), -1.0f, 1.0f, 0.0f, 1.0f, "posToColor"));
    DataNode::Ptr fragOut(new CombineVectorNode(remapToColor, 1.0f, "finalCol"));
    ser.MaterialOuts.FragmentOutputs.insert(ser.MaterialOuts.FragmentOutputs.end(),
                                            ShaderOutput("fOut_Color", fragOut));

    DataNode::VertexIns = ser.VertexInputs;
    DataNode::MaterialOuts = ser.MaterialOuts;
    std::string fragShader, vertShader;
    UniformDictionary params;
    std::string error = ShaderGenerator::GenerateVertFragShaders(vertShader, fragShader, params);
    if (!error.empty())
    {
        std::cout << "Error generating shaders: " << error << "\n";
        std::cin >> dummy;
        return;
    }

    std::cout << "Vertex Shader:\n" << vertShader << "\n\n\n\nFragment Shader:\n" << fragShader << "\n\n\n\n";
    std::cin >> dummy;

    XmlWriter writer;
    if (!writer.WriteDataStructure(ser, "Test Material", error))
    {
        std::cout << "Error writing material: " << error;
        std::cin >> dummy;
        return;
    }
    error = writer.SaveData("Content/Materials/SerializationTest.xml");
    if (!error.empty())
    {
        std::cout << "Error writing material file to 'Content/Materials/SerializationTest.xml': " << error;
        std::cin >> dummy;
        return;
    }

    std::cout << "Successfully wrote material to 'Content/Materials/SerializationTest.xml'.\n\n\n";
    std::cin >> dummy;

    vertPosOut.reset();
    remapToColor.reset();
    fragOut.reset();
    XmlReader reader("Content/Materials/SerializationTest.xml", error);
    if (!error.empty())
    {
        std::cout << "Error reading material file 'Content/Materials/SerializationTest.xml': " << error;
        std::cin >> dummy;
        return;
    }
    if (!reader.ReadDataStructure(ser, error))
    {
        std::cout << "Error reading material data: " << error;
        std::cin >> dummy;
        return;
    }

    fragShader.clear();
    vertShader.clear();
    params.ClearUniforms();
    DataNode::VertexIns = ser.VertexInputs;
    DataNode::MaterialOuts = ser.MaterialOuts;
    error = ShaderGenerator::GenerateVertFragShaders(vertShader, fragShader, params);
    if (!error.empty())
    {
        std::cout << "Error generating shaders for material from file: " << error;
        std::cin >> dummy;
        return;
    }

    std::cout << "\n\n\n\n\nVertex Shader:\n" << vertShader << "\n\n\n\nFragment Shader:\n" << fragShader << "\n\n\n\n";
    std::cin >> dummy;
}



//PRIORITY: Replace "intentionally not implemented" functions with the ' = delete' syntax.

int main()
{
    //TestIOStuff();
    //TestMaterialIOStuff();


    //OpenGLTestWorld().RunWorld();
    
    //TwoDOpenGLTest().RunWorld();
    //GUITestWorld().RunWorld();
    
    //NoiseTest().RunWorld();

    //TwoTrianglesWorld().RunWorld();

    //VoxelWorld().RunWorld();
    PlanetSimWorld().RunWorld();
}