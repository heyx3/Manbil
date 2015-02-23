#include "BinarySerialization.h"

#include "../DebugAssist.h"
#include <fstream>



std::string BinaryWriter::SaveData(const std::string& filePath)
{
    //Try to open the file for writing.
    std::ios_base::openmode openMode = std::ios_base::out | std::ios_base::binary | std::ios_base::trunc;
    std::ofstream writer(filePath.c_str(), openMode);
    if (writer.fail())
    {
        return "Could not open the file for writing";
    }
    
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


void BinaryWriter::WriteSimpleData(unsigned int sizeofType, BinaryDataTypes dataType, const void* pData)
{
    //Insert the header describing the data type.
    if (EnsureTypeSafety)
    {
        byteData.insert(byteData.end(), dataType);
    }

    //Reserve space for the data.
    unsigned int startIndex = byteData.size();
    for (unsigned int i = 0; i < sizeofType; ++i)
    {
        byteData.insert(byteData.end(), 0);
    }

    //Set the data.
    memcpy(&byteData.data()[startIndex], pData, sizeofType);
}

#pragma warning(disable: 4100)

#define BINARY_WRITE_DATA(dataType, dataTypeEnum, dataTypeFuncName) \
    void BinaryWriter::Write ## dataTypeFuncName(dataType value, const std::string& name) \
    { \
        WriteSimpleData(sizeof(dataType), dataTypeEnum, &value); \
    }
BINARY_WRITE_DATA(bool, BDT_BOOL, Bool)
BINARY_WRITE_DATA(unsigned char, BDT_BYTE, Byte)
BINARY_WRITE_DATA(int, BDT_INT, Int)
BINARY_WRITE_DATA(unsigned int, BDT_UINT, UInt)
BINARY_WRITE_DATA(float, BDT_FLOAT, Float)
BINARY_WRITE_DATA(double, BDT_DOUBLE, Double)

void BinaryWriter::WriteString(const std::string& value, const std::string& name)
{
    WriteUInt(value.size(), "");
    return WriteSimpleData(sizeof(char) * value.size(), BDT_STRING, value.c_str());
}
void BinaryWriter::WriteBytes(const unsigned char* bytes, unsigned int nBytes, const std::string& name)
{
    WriteUInt(nBytes, "");
    return WriteSimpleData(nBytes, BDT_BYTES, bytes);
}

void BinaryWriter::WriteCollection(ElementWriter writerFunc, const std::string& name,
                                   unsigned int bytesPerElement,
                                   const void* collection, unsigned int collectionSize,
                                   void* optionalData)
{
    //Insert the header describing the data type.
    if (EnsureTypeSafety)
    {
        byteData.insert(byteData.end(), BDT_COLLECTION);
    }

    //Write the size of the collection.
    WriteUInt(collectionSize, "size");

    //Write each collection element.
    for (unsigned int i = 0; i < collectionSize; ++i)
    {
        const void* toWrite = ((const char*)collection) + (i * bytesPerElement);
        writerFunc(this, toWrite, i, optionalData);
    }

    //Insert the footer indicating the end of the data type.
    if (EnsureTypeSafety)
    {
        byteData.insert(byteData.end(), BDT_COLLECTION_END);
    }
}

void BinaryWriter::WriteDataStructure(const IWritable& toSerialize, const std::string& name)
{
    //Insert the header describing the data type.
    if (EnsureTypeSafety)
    {
        byteData.insert(byteData.end(), BDT_DATA_STRUCTURE);
    }

    //Write the class.
    toSerialize.WriteData(this);

    //Insert the footer describing the end of the structure.
    if (EnsureTypeSafety)
    {
        byteData.insert(byteData.end(), BDT_DATA_STRUCTURE_END);
    }
}

#pragma warning(default: 4100)



BinaryReader::BinaryReader(bool ensureTypeSafety, const std::string& filePath)
    : EnsureTypeSafety(ensureTypeSafety), currentDataPtr(0)
{
    //Try to open the file.
    std::ifstream fileDat(filePath, std::ios_base::in | std::ios_base::binary);
    if (fileDat.fail())
    {
        ErrorMessage = "Couldn't open file";
        return;
    }

    //Try to read in the size of the data in bytes.
    if (EnsureTypeSafety)
    {
        unsigned char firstType;
        fileDat >> firstType;
        if (firstType != BDT_UINT)
        {
            ErrorMessage = "First data element isn't an unsigned int";
            fileDat.close();
            return;
        }
    }
    unsigned int dataSize;
    fileDat.read((char*)&dataSize, sizeof(unsigned int));
    if (fileDat.fail())
    {
        ErrorMessage = "Couldn't read out size of data in file.";
        fileDat.close();
        return;
    }
    
    //Copy in the data.
    byteData.resize(dataSize);
    fileDat.read((char*)byteData.data(), byteData.size());
    if (fileDat.fail())
    {
        ErrorMessage = "Expected " + std::to_string(dataSize) +
                            " bytes of data, but it contained " + std::to_string(fileDat.gcount());
        fileDat.close();
        return;
    }

    fileDat.close();
}


#define BINARY_READ_DATA(dataType, dataTypeEnum, dataTypeName) \
    void BinaryReader::Read ## dataTypeName(dataType& outDat) \
    { \
        ReadSimpleData(sizeof(dataType), dataTypeEnum, &outDat); \
    }
BINARY_READ_DATA(bool, BDT_BOOL, Bool)
BINARY_READ_DATA(unsigned char, BDT_BYTE, Byte)
BINARY_READ_DATA(int, BDT_INT, Int)
BINARY_READ_DATA(unsigned int, BDT_UINT, UInt)
BINARY_READ_DATA(float, BDT_FLOAT, Float)
BINARY_READ_DATA(double, BDT_DOUBLE, Double)

void BinaryReader::ReadString(std::string& outStr)
{
    //Get the length of the string.
    unsigned int size;
    ReadUInt(size);

    //Read in the string data.
    std::vector<char> str;
    str.resize(size);
    ReadSimpleData(sizeof(char) * size, BDT_STRING, str.data());

    outStr.clear();
    outStr.reserve(size);
    for (unsigned int i = 0; i < str.size(); ++i)
    {
        outStr += str[i];
    }
}
void BinaryReader::ReadBytes(std::vector<unsigned char>& outBytes)
{
    //Get the number of bytes.
    unsigned int size;
    ReadUInt(size);

    //Read in the bytes.
    outBytes.resize(outBytes.size() + size);
    ReadSimpleData(sizeof(unsigned char) * size, BDT_BYTES, outBytes.data());
}

bool BinaryReader::NextData(unsigned int size, void* pData)
{
    unsigned int dataLeft = byteData.size() - currentDataPtr;
    if (dataLeft < size)
    {
        return false;
    }

    memcpy(pData, &byteData.data()[currentDataPtr], size);
    currentDataPtr += size;
    return true;
}
void BinaryReader::ReadSimpleData(unsigned int sizeofType, BinaryDataTypes expectedType, void* outData)
{
    //Get the type of the data.
    if (EnsureTypeSafety)
    {
        BinaryDataTypes type;
        if (!NextData(1, &type))
        {
            ErrorMessage = "Unexptected end of file when reading data type";
            throw EXCEPTION_FAILURE;
        }

        //Make sure it is the expected type.
        if (type != expectedType)
        {
            ErrorMessage = "Wrong type: expected " + DebugAssist::ToString(expectedType) +
                                " but found " + DebugAssist::ToString(type);
            throw EXCEPTION_FAILURE;
        }
    }

    //Read the actual data.
    if (!NextData(sizeofType, outData))
    {
        ErrorMessage = "Unexpected end of file when reading data";
        throw EXCEPTION_FAILURE;
    }
}

void BinaryReader::ReadCollection(ElementCreator creatorFunc, ElementReader readerFunc,
                                  CollectionResizer resizer, void* pCollection,
                                  void* optionalData)
{
    //Check the header describing the data type.
    if (EnsureTypeSafety)
    {
        BinaryDataTypes collHeader;
        if (!NextData(1, &collHeader))
        {
            ErrorMessage = "Unexpected end of file when reading data type";
            throw EXCEPTION_FAILURE;
        }

        //Make sure it's the expected type.
        if (collHeader != BDT_COLLECTION)
        {
            ErrorMessage = "Wrong type: expected 'collection' but found '" +
                                DebugAssist::ToString(collHeader);
            throw EXCEPTION_FAILURE;
        }
    }

    //Read the size of the collection.
    unsigned int readSize;
    ReadUInt(readSize);
    resizer(pCollection, readSize);

    //Read each collection element.
    for (unsigned int i = 0; i < readSize; ++i)
    {
        creatorFunc(pCollection, i, optionalData);
        readerFunc(this, pCollection, i, optionalData);
    }

    //Check that the data structure is at its end.
    if (EnsureTypeSafety)
    {
        BinaryDataTypes collFooter;
        if (!NextData(1, &collFooter))
        {
            ErrorMessage = "Unexpected end of file when reading footer data";
            throw EXCEPTION_FAILURE;
        }

        //Make sure it's the expected type.
        if (collFooter != BDT_COLLECTION_END)
        {
            ErrorMessage = "Invalid footer: expected 'collectionEND' but found '" +
                                DebugAssist::ToString(collFooter, false) + "' instead";
            throw EXCEPTION_FAILURE;
        }
    }
}

void BinaryReader::ReadDataStructure(IReadable& toSerialize)
{
    //Check the header describing the data type.
    if (EnsureTypeSafety)
    {
        BinaryDataTypes bdt;
        if (!NextData(1, &bdt))
        {
            ErrorMessage = "Unexpected end of file.";
            throw EXCEPTION_FAILURE;
        }
        if (bdt != BDT_DATA_STRUCTURE)
        {
            ErrorMessage = "Expected 'DataStructure' type but got " +
                                DebugAssist::ToString(bdt, false) + " instead";
            throw EXCEPTION_FAILURE;
        }
    }

    toSerialize.ReadData(this);

    //Check the footer describing the end of the data structure.
    if (EnsureTypeSafety)
    {
        BinaryDataTypes bdt;
        if (!NextData(1, &bdt))
        {
            ErrorMessage = "Unexpected end of file.";
            throw EXCEPTION_FAILURE;
        }
        if (bdt != BDT_DATA_STRUCTURE_END)
        {
            ErrorMessage = "Expected 'DataStructureEND' type but got '" +
                                DebugAssist::ToString(bdt, false) + " instead";
        }
    }
}