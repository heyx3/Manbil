#pragma once

#include <string>
#include <vector>
#include <assert.h>



//An interface for data that provides behavior for serialization.
struct IReadable;
//An interface for data that provides behavior for serialization.
struct IWritable;


//Writes data to some kind of stream. Classes inherit from this class to provide specific behavior,
//     e.x. XML files or binary files.
//Names can be provided when writing data, but those names are only cosmetic and may be ignored.
//The order that data is written in must match the order it will be read out from!
class DataWriter
{
public:

    //The exception value that's thrown if some operation fails.
    static const int EXCEPTION_FAILURE = 666;


    //An error message that may be set when EXCEPTION_FAILURE is thrown.
    std::string ErrorMessage;


    virtual void WriteBool(bool value, const std::string& name) = 0;
    virtual void WriteByte(unsigned char value, const std::string& name) = 0;
    virtual void WriteInt(int value, const std::string& name) = 0;
    virtual void WriteUInt(unsigned int value, const std::string& name) = 0;
    virtual void WriteFloat(float value, const std::string& name) = 0;
    virtual void WriteDouble(double value, const std::string& name) = 0;
    virtual void WriteString(const std::string& value, const std::string& name) = 0;
    virtual void WriteBytes(const unsigned char* bytes, unsigned int nBytes, const std::string& name) = 0;

    //A function that writes the given element of a collection using the given DataWriter,
    //   with optional data passed in.
    typedef void(*ElementWriter)(DataWriter* writer, const void* elementToWrite,
                                 unsigned int elementIndex, void* optionalData);
    //Writes a collection of some kind of data.
    virtual void WriteCollection(ElementWriter writerFunc, const std::string& name,
                                 unsigned int bytesPerElement,
                                 const void* collection, unsigned int collectionSize,
                                 void* optionalData = 0) = 0;

    //Writes a complex object that implements the IWritable interface.
    virtual void WriteDataStructure(const IWritable& toSerialize, const std::string& name) = 0;
};

//Reads data from some kind of stream. Classes inherit from this class to provide specific behavior,
//    e.x. XML files or binary files.
//The order that data is read out must match the order it was written in!
class DataReader
{
public:

    //The exception value that's thrown if some operation fails.
    static const int EXCEPTION_FAILURE = 667;


    //An error message that may be set when EXCEPTION_FAILURE is thrown.
    std::string ErrorMessage;


    virtual void ReadBool(bool& outB) = 0;
    virtual void ReadByte(unsigned char& outB) = 0;
    virtual void ReadInt(int& outI) = 0;
    virtual void ReadUInt(unsigned int& outU) = 0;
    virtual void ReadFloat(float& outF) = 0;
    virtual void ReadDouble(double& outD) = 0;
    virtual void ReadString(std::string& outStr) = 0;
    virtual void ReadBytes(std::vector<unsigned char>& outBytes) = 0;

    //A function that prepares a collection to store the given number of elements.
    typedef void(*CollectionResizer)(void* pCollection, unsigned int newElement);
    //A function that adds a default value to the end of the given collection.
    typedef void(*ElementCreator)(void* pCollection, unsigned int newElementIndex, void* optionalData);
    //A function that reads the given element of a collection using the given DataReader,
    //   with optional data passed in.
    typedef void(*ElementReader)(DataReader* reader, void* pCollection,
                                 unsigned int elementIndex, void* optionalData);
    //Reads a collection of some kind of data into "outData".
    virtual void ReadCollection(ElementCreator creatorFunc, ElementReader readerFunc,
                                CollectionResizer resizer, void* pCollection,
                                void* optionalData = 0) = 0;

    //Reads a complex object that implements the IReadable interface.
    virtual void ReadDataStructure(IReadable& toSerialize) = 0;
};



//A data structure that can read its data from DataReader instances.
struct IReadable
{
public:
    virtual void ReadData(DataReader* data) = 0;
};

//A data structure that can write its data to DataWriter instances.
struct IWritable
{
public:
    virtual void WriteData(DataWriter* data) const = 0;
};

//A data structure that can both read and write its data from/to DataReader/DataWriter instances.
struct ISerializable : public IReadable, IWritable { };