#pragma once

#include "DataSerialization.h"
#include <vector>


//The different types of data that can be written into a binary file.
//These enum values can be written into the binary file to add error-checking to the data.
enum BinaryDataTypes : unsigned char
{
    BDT_BOOL = 0,
    BDT_BYTE = 1,
    BDT_INT = 2,
    BDT_UINT = 3,
    BDT_FLOAT = 4,
    BDT_DOUBLE = 5,
    BDT_STRING = 6,
    BDT_BYTES = 7,

    BDT_COLLECTION = 8,
    BDT_COLLECTION_END = 9,

    BDT_DATA_STRUCTURE = 19,
    BDT_DATA_STRUCTURE_END = 11,
};


//Writes data to a binary file.
class BinaryWriter : public DataWriter
{
public:

    //Whether to ensure type safety when reading this data back later.
    //Adds an extra byte of data for every call to a Write function.
    bool EnsureTypeSafety;


    //"moreErrorChecking" indicates whether to ensure type safety.
    //Optionally takes in the amount of space to initially reserve in the byte data container.
    BinaryWriter(bool ensureTypeSafety, unsigned int byteDataSizeReservation = 64)
        : EnsureTypeSafety(ensureTypeSafety)
    {
        byteData.reserve(byteDataSizeReservation);
    }

    
    //Saves the written data out to a file at the given path.
    //Returns an error message, or the empty string if the data was saved successfully.
    std::string SaveData(const std::string& filePath);

    virtual void WriteBool(bool value, const std::string& name) override;
    virtual void WriteByte(unsigned char value, const std::string& name) override;
    virtual void WriteInt(int value, const std::string& name) override;
    virtual void WriteUInt(unsigned int value, const std::string& name) override;
    virtual void WriteFloat(float value, const std::string& name) override;
    virtual void WriteDouble(double value, const std::string& name) override;
    virtual void WriteString(const std::string& value, const std::string& name) override;
    virtual void WriteBytes(const unsigned char* bytes, unsigned int nBytes,
                            const std::string& name) override;

    virtual void WriteCollection(ElementWriter writerFunc, const std::string& name,
                                 unsigned int bytesPerElement,
                                 const void* collection, unsigned int collectionSize,
                                 void* optionalData = 0) override;

    virtual void WriteDataStructure(const IWritable& toSerialize, const std::string& name) override;
    
private:

    //Used for writing primitive data types.
    void WriteSimpleData(unsigned int sizeofType, BinaryDataTypes dataType, const void* pData);

    std::vector<unsigned char> byteData;
};

//Reads data from a binary file.
class BinaryReader : public DataReader
{
public:

    //Indicates whether the BinaryWriter that made the file being read used extra error checking.
    bool EnsureTypeSafety;

    //If this reader throws an exception, this string will be set to an error message.
    std::string ErrorMessage;


    //"ensureTypeSafety" indicates whether the BinaryWriter that made the file used extra error checking.
    //This constructor does not throw exceptions, but it may set the "ErrorMessage" field
    //    if something went wrong.
    BinaryReader(bool ensureTypeSafety, const std::string& fileName);


    virtual void ReadBool(bool& outB) override;
    virtual void ReadByte(unsigned char& outB) override;
    virtual void ReadInt(int& outI) override;
    virtual void ReadUInt(unsigned int& outU) override;
    virtual void ReadFloat(float& outF) override;
    virtual void ReadDouble(double& outD) override;
    virtual void ReadString(std::string& outStr) override;
    virtual void ReadBytes(std::vector<unsigned char>& outBytes) override;

    virtual void ReadCollection(ElementCreator creatorFunc, ElementReader readerFunc,
                                CollectionResizer resizer, void* pCollection,
                                void* optionalData = 0) override;

    virtual void ReadDataStructure(IReadable& toSerialize) override;

private:

    void ReadSimpleData(unsigned int sizeofType, BinaryDataTypes expectedType,
                        void* outData);

    //Reads in the next few bytes from the "byteData" vector.
    //Returns true if everything went fine, or false if there weren't enough bytes left to read.
    bool NextData(unsigned int size, void *pData);

    std::vector<unsigned char> byteData;
    unsigned int currentDataPtr;
};