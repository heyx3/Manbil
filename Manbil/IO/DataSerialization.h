#pragma once

#include <assert.h>
#include <string>
#include <vector>
#include <unordered_map>



class DataReader;
class DataWriter;


//A data structure that can read its data from a DataReader.
struct IReadable
{
public:
    virtual ~IReadable(void) { }
    virtual void ReadData(DataReader* data) = 0;
};

//A data structure that can write its data to a DataWriter.
struct IWritable
{
public:
    virtual ~IWritable(void) { }
    virtual void WriteData(DataWriter* data) const = 0;
};

//A data structure that can both read and write its data.
struct ISerializable : public IReadable, IWritable { };



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

    //Writes a data structure that implements the IWritable interface.
    virtual void WriteDataStructure(const IWritable& toSerialize, const std::string& name) = 0;


    //A function that writes the given element of a collection using the given DataWriter,
    //   with optional data passed in.
    typedef void(*ElementWriter)(DataWriter* writer, const void* elementToWrite,
                                 unsigned int elementIndex, void* userData);
    //Writes a collection of some kind of data.
    //TODO: Implement this using templates in the same way that "WriteDictionary" is implemented.
    virtual void WriteCollection(ElementWriter writerFunc, const std::string& name,
                                 unsigned int bytesPerElement,
                                 const void* collection, unsigned int collectionSize,
                                 void* userData = 0) = 0;


    template<typename Key, typename Value>
    //Writes a key and value pair.
    using DictElementWriter = void(*)(DataWriter* writer, const Key* k, const Value* v, void* userData);

    template<typename Key, typename Value>
    //Writes a set of key-value pairs.
    void WriteDictionary(const std::unordered_map<Key, Value>& toWrite,
                         DictElementWriter<Key, Value> pairWriter,
                         const std::string& name, void* userData = 0)
    {
        KVPDict_Write<Key, Value> helper(toWrite, pairWriter, userData);
        WriteDataStructure(helper, name);
    }

private:

    #pragma region Helper data structure for "WriteDictionary()"

    template<typename Key, typename Value>
    //Used for "WriteDictionary()", because C++ doesn't like data structures inside a templated function.
    struct KVPDict_Write : public IWritable
    {
        const std::unordered_map<Key, Value>& Dict;
        DictElementWriter<Key, Value> WriterFunc;
        void* pData;

        KVPDict_Write(const std::unordered_map<Key, Value>& dict,
                      DictElementWriter<Key, Value> writerFunc,
                      void* _pData)
                : Dict(dict), WriterFunc(writerFunc), pData(_pData)
        { }

        virtual void WriteData(DataWriter* writer) const override
        {
            writer->WriteUInt(Dict.size(), "Number of elements");
            for (auto i = Dict.begin(); i != Dict.end(); ++i)
            {
                WriterFunc(writer, &i->first, &i->second, pData);
            }
        }
    };

    #pragma endregion
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

    //Reads a data structurethat implements the IReadable interface.
    virtual void ReadDataStructure(IReadable& toSerialize) = 0;


    //A function that resizes a collection to store at least the given number of elements.
    typedef void(*CollectionResizer)(void* pCollection, unsigned int nElements);
    //A function that reads the given element of a collection using the given DataReader,
    //   with optional data passed in.
    typedef void(*ElementReader)(DataReader* reader, void* pCollection,
                                 unsigned int elementIndex, void* userData);

    //Reads a collection of some kind of data into "outData".
    //TODO: Implement this using templates in the same way that "ReadDictionary" is implemented.
    virtual void ReadCollection(ElementReader readerFunc, CollectionResizer resizer,
                                void* pCollection, void* userData = 0) = 0;


    template<typename Key, typename Value>
    //Reads a key and value pair.
    using DictElementReader = void(*)(DataReader* reader, Key* k, Value* v, void* userData);

    template<typename Key, typename Value>
    //Reads a set of key-value pairs.
    void ReadDictionary(std::unordered_map<Key, Value>& toRead,
                        DictElementReader<Key, Value> pairReader,
                        void* userData = 0)
    {
        KVPDict_Read<Key, Value> helper(toRead, pairReader, userData);
        ReadDataStructure(helper);
    }

private:

    #pragma region Helper data structure for "ReadDictionary()"

    template<typename Key, typename Value>
    //Used for "ReadDictionary()", because C++ doesn't like data structures inside a templated function.
    struct KVPDict_Read : public IReadable
    {
        std::unordered_map<Key, Value>& Dict;
        DictElementReader<Key, Value> ReaderFunc;
        void* pData;

        KVPDict_Read(std::unordered_map<Key, Value>& dict,
                     DictElementReader<Key, Value> readerFunc,
                     void* _pData)
            : Dict(dict), ReaderFunc(readerFunc), pData(_pData) { }

        virtual void ReadData(DataReader* reader) override
        {
            unsigned int nElements;
            reader->ReadUInt(nElements);

            Dict.reserve(nElements);

            Key k;
            Value v;
            for (unsigned int i = 0; i < nElements; ++i)
            {
                ReaderFunc(reader, &k, &v, pData);
                Dict[k] = v;
            }
        }
    };

    #pragma endregion
};