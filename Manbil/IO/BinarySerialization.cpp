#include "BinarySerialization.h"

#include "../DebugAssist.h"
#include <fstream>



std::string BinaryWriter::SaveData(std::string filePath)
{
    //Try to open the file for writing.
    std::ofstream writer(filePath.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
    if (writer.fail())
        return "Could not open the file for writing";
    
    //First write the number of bytes in the data array.
    if (EnsureTypeSafety)
    {
        BinaryDataTypes type = BDT_UINT;
        writer.write((const char*)&type, 1);
    }
    unsigned int size = byteData.size();
    writer.write((const char*)&size, sizeof(unsigned int));

    //Write all the data out.
    writer.write((const char*)byteData.data(), byteData.size());
    if (writer.fail())
    {
        writer.close();
        return "Could not write to the file";
    }

    writer.close();
    return "";
}


bool BinaryWriter::WriteSimpleData(unsigned int sizeofType, BinaryDataTypes dataType, const void* pData)
{
    //Insert the header describing the data type.
    if (EnsureTypeSafety) byteData.insert(byteData.end(), dataType);

    //Reserve space for the data.
    unsigned int startIndex = byteData.size();
    for (unsigned int i = 0; i < sizeofType; ++i)
        byteData.insert(byteData.end(), 0);

    //Set the data.
    memcpy(&byteData.data()[startIndex], pData, sizeofType);

    return true;
}

#pragma warning(disable: 4100)
bool BinaryWriter::WriteBool(bool value, std::string name, std::string & outError)
{
    return WriteSimpleData(sizeof(bool), BDT_BOOL, &value);
}
bool BinaryWriter::WriteByte(unsigned char value, std::string name, std::string & outError)
{
    return WriteSimpleData(sizeof(unsigned char), BDT_BYTE, &value);
}
bool BinaryWriter::WriteInt(int value, std::string name, std::string & outError)
{
    return WriteSimpleData(sizeof(int), BDT_INT, &value);
}
bool BinaryWriter::WriteUInt(unsigned int value, std::string name, std::string & outError)
{
    return WriteSimpleData(sizeof(unsigned int), BDT_UINT, &value);
}
bool BinaryWriter::WriteFloat(float value, std::string name, std::string & outError)
{
    return WriteSimpleData(sizeof(float), BDT_FLOAT, &value);
}
bool BinaryWriter::WriteDouble(double value, std::string name, std::string & outError)
{
    return WriteSimpleData(sizeof(double), BDT_DOUBLE, &value);
}
bool BinaryWriter::WriteString(const std::string & value, std::string name, std::string & outError)
{
    if (!WriteUInt(value.size(), "", outError))
        return false;

    return WriteSimpleData(sizeof(char) * value.size(), BDT_STRING, value.c_str());
}
#pragma warning(default: 4100)

bool BinaryWriter::WriteCollection(std::string name, ElementWriter writerFunc, const void* collection, unsigned int size,
                                   std::string & outError, void *optionalData)
{
    outError.clear();

    //Insert the header describing the data type.
    if (EnsureTypeSafety) byteData.insert(byteData.end(), BDT_COLLECTION);

    //Write the size of the collection.
    if (!WriteUInt(size, "size", outError))
        return false;

    //Write each collection element.
    unsigned int i = 0;
    while (i < size && writerFunc(collection, i, this, outError, optionalData))
        i += 1;

    //Insert the footer indicating the end of the data type.
    if (EnsureTypeSafety) byteData.insert(byteData.end(), BDT_COLLECTION_END);

    return outError.empty();
}

bool BinaryWriter::WriteDataStructure(const ISerializable & toSerialize, std::string name, std::string & outError)
{
    //Insert the header describing the data type.
    if (EnsureTypeSafety) byteData.insert(byteData.end(), BDT_DATA_STRUCTURE);

    //Write the class.
    if (!toSerialize.WriteData(this, outError)) return false;

    //Insert the footer describing the end of the structure.
    if (EnsureTypeSafety) byteData.insert(byteData.end(), BDT_DATA_STRUCTURE_END);

    return true;
}



BinaryReader::BinaryReader(bool ensureTypeSafety, std::string filePath, std::string & outError)
    : EnsureTypeSafety(ensureTypeSafety), currentDataPtr(0)
{
    //Try to open the file.
    std::ifstream fileDat(filePath, std::ios_base::in | std::ios_base::binary);
    if (fileDat.fail())
    {
        outError = "Couldn't open file";
        return;
    }

    //Try to read in the size of the data in bytes.
    if (EnsureTypeSafety)
    {
        unsigned char firstType;
        fileDat >> firstType;
        if (firstType != BDT_UINT)
        {
            outError = "First data element isn't an unsigned int";
            return;
        }
    }
    unsigned int dataSize;
    fileDat.read((char*)&dataSize, sizeof(unsigned int));
    if (fileDat.fail())
    {
        outError = "Couldn't read out size of data in file.";
        return;
    }
    
    //Copy in the data.
    byteData.resize(dataSize);
    fileDat.read((char*)byteData.data(), byteData.size());
    if (fileDat.fail())
    {
        outError = "Expected " + std::to_string(dataSize) + " bytes of data, but it contained " + std::to_string(fileDat.gcount());
        fileDat.close();
        return;
    }

    fileDat.close();
}

MaybeValue<bool> BinaryReader::ReadBool(std::string & outError)
{
    bool val;
    if (!ReadSimpleData(sizeof(bool), BDT_BOOL, &val, outError))
        return MaybeValue<bool>();
    else return MaybeValue<bool>(val);
}
MaybeValue<unsigned char> BinaryReader::ReadByte(std::string & outError)
{
    bool val;
    if (!ReadSimpleData(sizeof(unsigned char), BDT_BYTE, &val, outError))
        return MaybeValue<unsigned char>();
    else return MaybeValue<unsigned char>(val);
}
MaybeValue<int> BinaryReader::ReadInt(std::string & outError)
{
    int val;
    if (!ReadSimpleData(sizeof(int), BDT_INT, &val, outError))
        return MaybeValue<int>();
    else return MaybeValue<int>(val);
}
MaybeValue<unsigned int> BinaryReader::ReadUInt(std::string & outError)
{
    unsigned int val;
    if (!ReadSimpleData(sizeof(unsigned int), BDT_UINT, &val, outError))
        return MaybeValue<unsigned int>();
    else return MaybeValue<unsigned int>(val);
}
MaybeValue<float> BinaryReader::ReadFloat(std::string & outError)
{
    float val;
    if (!ReadSimpleData(sizeof(float), BDT_FLOAT, &val, outError))
        return MaybeValue<float>();
    else return MaybeValue<float>(val);
}
MaybeValue<double> BinaryReader::ReadDouble(std::string & outError)
{
    double val;
    if (!ReadSimpleData(sizeof(double), BDT_DOUBLE, &val, outError))
        return MaybeValue<double>();
    else return MaybeValue<double>(val);
}
MaybeValue<std::string> BinaryReader::ReadString(std::string & outError)
{
    MaybeValue<unsigned int> size = ReadUInt(outError);
    if (!size.HasValue())
        return MaybeValue<std::string>();

    std::vector<char> str;
    str.resize(size.GetValue());
    if (!ReadSimpleData(sizeof(char) * size.GetValue(), BDT_STRING, str.data(), outError))
        return MaybeValue<std::string>();
    else return MaybeValue<std::string>(std::string(str.begin(), str.end()));
}

bool BinaryReader::NextData(unsigned int size, void *pData)
{
    int dataLeft = (int)byteData.size() - (int)currentDataPtr;
    if (dataLeft < size) return false;

    memcpy(pData, &byteData.data()[currentDataPtr], size);
    currentDataPtr += size;
    return true;
}
bool BinaryReader::ReadSimpleData(unsigned int sizeofType, BinaryDataTypes expectedType, void* outData, std::string & errorMsg)
{
    //Get the type of the data.
    if (EnsureTypeSafety)
    {
        BinaryDataTypes type;
        if (!NextData(1, &type))
        {
            errorMsg = "Unexptected end of file when reading data type";
            return false;
        }

        //Make sure it is the expected type.
        if (type != expectedType)
        {
            errorMsg = "Wrong type: expected " + DebugAssist::ToString(expectedType) +
                " but found " + DebugAssist::ToString(type);
            return false;
        }
    }

    //Read the actual data.
    if (!NextData(sizeofType, outData))
    {
        errorMsg = "Unexpected end of file when reading data";
        return false;
    }

    return true;
}

bool BinaryReader::ReadCollection(ElementReader readerFunc, unsigned int bytesPerElement, std::string & outError,
                                  std::vector<unsigned char> & outData, void * optionalData)
{
    //Check the header describing the data type.
    if (EnsureTypeSafety)
    {
        BinaryDataTypes collHeader;
        if (!NextData(1, &collHeader))
        {
            outError = "Unexpected end of file when reading data type";
            return false;
        }

        //Make sure it's the expected type.
        if (collHeader != BDT_COLLECTION)
        {
            outError = "Wrong type: expected 'collection' but found '" + DebugAssist::ToString(collHeader);
            return false;
        }
    }

    //Read the size of the collection.
    MaybeValue<unsigned int> readSize = ReadUInt(outError);
    if (!readSize.HasValue())
    {
        outError = "Error reading collection size: " + outError;
        return false;
    }
    outData.resize(readSize.GetValue() * bytesPerElement);

    //Read each collection element.
    for (unsigned int i = 0; i < readSize.GetValue(); ++i)
    {
        if (!readerFunc(outData.data(), i, this, outError, optionalData))
        {
            outError = "Error reading collection element " + std::to_string(i) + ": " + outError;
            return false;
        }
    }

    //Check that the data structure is at its end.
    if (EnsureTypeSafety)
    {
        BinaryDataTypes collFooter;
        if (!NextData(1, &collFooter))
        {
            outError = "Unexpected end of file when reading footer data";
            return false;
        }

        //Make sure it's the expected type.
        if (collFooter != BDT_COLLECTION_END)
        {
            outError = "Invalid footer: expected 'collectionEND' but found '" + DebugAssist::ToString(collFooter, false) + "' instead";
            return false;
        }
    }

    return true;
}

bool BinaryReader::ReadDataStructure(ISerializable & toSerialize, std::string & outError)
{
    //Check the header describing the data type.
    if (EnsureTypeSafety)
    {
        BinaryDataTypes bdt;
        if (!NextData(1, &bdt))
        {
            outError = "Unexpected end of file.";
            return false;
        }
        if (bdt != BDT_DATA_STRUCTURE)
        {
            outError = "Expected 'DataStructure' type but got " + DebugAssist::ToString(bdt, false) + " instead";
            return false;
        }
    }

    if (!toSerialize.ReadData(this, outError))
    {
        outError = "Error reading data structure: " + outError;
        return false;
    }

    //Check the footer describing the end of the data structure.
    if (EnsureTypeSafety)
    {
        BinaryDataTypes bdt;
        if (!NextData(1, &bdt))
        {
            outError = "Unexpected end of file.";
            return false;
        }
        if (bdt != BDT_DATA_STRUCTURE_END)
        {
            outError = "Expected 'DataStructureEND' type but got '" + DebugAssist::ToString(bdt, false) + " instead";
        }
    }

    return true;
}