#pragma once

#include "DataSerialization.h"
#include "tinyxml2.h"


//Converts the given error code into a string.
std::string TinyXMLErrorToString(tinyxml2::XMLError err);


//Writes data to XML.
class XmlWriter : public DataWriter
{
public:

    XmlWriter(std::string rootNodeName = "root");

    
    //Saves the written data out to a file at the given path.
    //Returns an error message, or the empty string if the data was saved successfully.
    std::string SaveData(const std::string& path);

    virtual void WriteBool(bool value, const std::string& name) override;
    virtual void WriteByte(unsigned char value, const std::string& name) override;
    virtual void WriteInt(int value, const std::string& name) override;
    virtual void WriteUInt(unsigned int value, const std::string& name) override;
    virtual void WriteFloat(float value, const std::string& name) override;
    virtual void WriteDouble(double value, const std::string& name) override;
    virtual void WriteString(const std::string& value, const std::string& name) override;
    virtual void WriteBytes(const unsigned char* bytes, unsigned int nBytes, const std::string& name) override;

    virtual void WriteCollection(ElementWriter writerFunc, const std::string& name,
                                 unsigned int bytesPerElement,
                                 const void* collection, unsigned int collectionSize,
                                 void* optionalData = 0) override;

    virtual void WriteDataStructure(const IWritable& toSerialize, const std::string& name) override;


private:

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* currentRoot;
};


//Reads data from XML.
class XmlReader : public DataReader
{
public:

    //If there was an error reading the given file,
    //    an error message is written to this instance's "ErrorMessage".
    //This constructor will NOT throw an exception.
    XmlReader(const std::string& filePath);

    //Loads in new XML data, resetting this reader.
    void Reload(const std::string& filePath, std::string& outErrorMsg);


    virtual void ReadBool(bool& outB) override;
    virtual void ReadByte(unsigned char& outB) override;
    virtual void ReadInt(int& outI) override;
    virtual void ReadUInt(unsigned int& outU) override;
    virtual void ReadFloat(float& outF) override;
    virtual void ReadDouble(double& outD) override;
    virtual void ReadString(std::string& outStr) override;
    virtual void ReadBytes(std::vector<unsigned char>& outBytes) override;

    virtual void ReadCollection(ElementReader readerFunc, CollectionResizer resizer,
                                void* pCollection, void* optionalData = 0) override;

    virtual void ReadDataStructure(IReadable& toSerialize) override;


private:

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement *currentRoot,
                         *currentChild;
};