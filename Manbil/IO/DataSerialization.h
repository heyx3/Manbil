#pragma once

#include <string>
#include <vector>
#include <assert.h>



template<class Type>
//Represents a value that may or may not exist.
class MaybeValue
{
public:

	bool HasValue(void) const { return hasVal; }

    const Type & GetValue(void) const { assert(HasValue()); return val; }
    Type & GetValue(void) { assert(HasValue()); return val; }

	MaybeValue(void) : hasVal(false) { }
	MaybeValue(Type value) : hasVal(true), val(value) { }

private:

    bool hasVal;
	Type val;
};



struct ISerializable;


//Writes data to some kind of stream. Classes inherit from this class to provide specific behavior,
//     e.x. XML files or binary files.
//Names can be provided when writing data, but those names are only cosmetic and may be ignored.
//The order that data is written in must match the order it will be read out from!
class DataWriter
{
public:

    //Returns an error message, or the empty string if the data was saved successfully.
    virtual std::string SaveData(std::string filePath) = 0;

    virtual bool WriteBool(bool value, std::string name, std::string & outError) = 0;
    virtual bool WriteByte(unsigned char value, std::string name, std::string & outError) = 0;
    virtual bool WriteInt(int value, std::string name, std::string & outError) = 0;
    virtual bool WriteUInt(unsigned int value, std::string name, std::string & outError) = 0;
    virtual bool WriteFloat(float value, std::string name, std::string & outError) = 0;
    virtual bool WriteDouble(double value, std::string name, std::string & outError) = 0;
    virtual bool WriteString(const std::string & value, std::string name, std::string & outError) = 0;
    //TODO: Read/write array of bytes.

    //A function that writes the given element of the given collection using the given DataSerializer,
    //   with optional data passed in.
    typedef bool(*ElementWriter)(const void* elementCollection, unsigned int elementIndex, DataWriter * writer,
                                 std::string & outError, void* optionalData);
    //"writerFunc" has the signature "(bool F(void*, unsigned int, DataWriter*, std::string&, void*))".
    virtual bool WriteCollection(std::string name, ElementWriter writerFunc, const void* collection, unsigned int collectionSize,
                                 std::string & outError, void *optionalData = 0) = 0;

    virtual bool WriteDataStructure(const ISerializable & toSerialize, std::string name, std::string & outError) = 0;
};
//Reads data from some kind of stream. Classes inherit from this class to provide specific behavior,
//    e.x. XML files or binary files.
//The order that data is read out must match the order it was written in!
class DataReader
{
public:

    virtual MaybeValue<bool> ReadBool(std::string & outError) = 0;
    virtual MaybeValue<unsigned char> ReadByte(std::string & outError) = 0;
    virtual MaybeValue<int> ReadInt(std::string & outError) = 0;
    virtual MaybeValue<unsigned int> ReadUInt(std::string & outError) = 0;
    virtual MaybeValue<float> ReadFloat(std::string & outError) = 0;
    virtual MaybeValue<double> ReadDouble(std::string & outError) = 0;
    virtual MaybeValue<std::string> ReadString(std::string & outError) = 0;

    //A function that reads the given element of the given collection using the given DataSerializer,
    //   with optional data passed in.
    typedef bool(*ElementReader)(void* elementCollection, unsigned int elementIndex, DataReader * reader,
                                 std::string & outError, void* optionalData);
    //Reads a collection of byte data into "outData".
    //"readerFunc" has the signature "(bool F(void*, unsigned int, DataReader*, std::string&, void*))".
    virtual bool ReadCollection(ElementReader readerFunc, unsigned int bytesPerElement, std::string & outError,
                                std::vector<unsigned char> & outData, void * optionalData = 0) = 0;
    virtual bool ReadDataStructure(ISerializable & toSerialize, std::string & outError) = 0;
};



//A data structure that can use DataWriter and DataReader instances for serialization.
struct ISerializable
{
public:

    virtual bool ReadData(DataReader * data, std::string & outError) = 0;
    virtual bool WriteData(DataWriter * data, std::string & outError) const = 0;
};