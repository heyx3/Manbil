#pragma once

#include "DataSerialization.h"
#include <vector>


//The different types of data that can be written into a binary file.
enum BinaryDataTypes : unsigned char
{
    BDT_BOOL = 0,
    BDT_BYTE = 1,
    BDT_INT = 2,
    BDT_UINT = 3,
    BDT_FLOAT = 4,
    BDT_DOUBLE = 5,
    BDT_STRING = 6,
    BDT_COLLECTION = 7,
    BDT_DATA_STRUCTURE = 8,
};


//Writes data to a binary file.
class BinaryWriter : public DataWriter
{
public:

    //Whether to ensure type safety when reading this data back later at the cost of an extra byte for every piece of data.
    bool EnsureTypeSafety;

    //Optionally takes in the amount of space to initially reserve in the byte data container.
    //"moreErrorChecking" indicates whether to ensure type safety at the cost of an extra byte for every pice of data.
    BinaryWriter(bool ensureTypeSafety, unsigned int byteDataSizeReservation = 64)
        : EnsureTypeSafety(ensureTypeSafety)
    {
        byteData.reserve(byteDataSizeReservation);
    }

    virtual std::string SaveData(std::string filePath) override;

    virtual bool WriteBool(bool value, std::string name, std::string & outError) override;
    virtual bool WriteByte(unsigned char value, std::string name, std::string & outError) override;
    virtual bool WriteInt(int value, std::string name, std::string & outError) override;
    virtual bool WriteUInt(unsigned int value, std::string name, std::string & outError) override;
    virtual bool WriteFloat(float value, std::string name, std::string & outError) override;
    virtual bool WriteDouble(double value, std::string name, std::string & outError) override;
    virtual bool WriteString(const std::string & value, std::string name, std::string & outError) override;

    virtual bool WriteCollection(std::string name, ElementWriter writerFunc, const void* collection, unsigned int collectionSize,
                                 std::string & outError, void *optionalData = 0) override;

    virtual bool WriteDataStructure(const ISerializable & toSerialize, std::string name, std::string & outError) override;
    
private:

    //Used for writing primitive data types.
    bool WriteSimpleData(unsigned int sizeofType, BinaryDataTypes dataType, const void* pData);

    std::vector<unsigned char> byteData;
};

//Reads data from a binary file.
class BinaryReader : public DataReader
{
public:

    //Whether the BinaryWriter that made this file used extra error checking at the cost of 1 extra byte per piece of data.
    bool EnsureTypeSafety;

    //"ensureTypeSafety" indicates whether the BinaryWriter that made this file used extra error checking
    //    at the cost of 1 extra byte per piece of data.
    BinaryReader(bool ensureTypeSafety, std::string fileName, std::string & outErrorMsg);

    virtual MaybeValue<bool> ReadBool(std::string & outError) override;
    virtual MaybeValue<unsigned char> ReadByte(std::string & outError) override;
    virtual MaybeValue<int> ReadInt(std::string & outError) override;
    virtual MaybeValue<unsigned int> ReadUInt(std::string & outError) override;
    virtual MaybeValue<float> ReadFloat(std::string & outError) override;
    virtual MaybeValue<double> ReadDouble(std::string & outError) override;
    virtual MaybeValue<std::string> ReadString(std::string & outError) override;

    virtual bool ReadCollection(ElementReader readerFunc, unsigned int bytesPerElement, std::string & outError,
                                std::vector<unsigned char> & outData, void * optionalData = 0) override;

    virtual bool ReadDataStructure(ISerializable & toSerialize, std::string & outError) override;

private:

    bool ReadSimpleData(unsigned int sizeofType, BinaryDataTypes expectedType, void* outData, std::string & errorMsg);

    bool NextData(unsigned int size, void *pData);

    std::vector<unsigned char> byteData;
    unsigned int currentDataPtr;
};