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


    virtual std::string SaveData(std::string path) override;

    virtual bool WriteBool(bool value, std::string name, std::string & outError) override;
    virtual bool WriteByte(unsigned char value, std::string name, std::string & outError) override;
    virtual bool WriteInt(int value, std::string name, std::string & outError) override;
    virtual bool WriteUInt(unsigned int value, std::string name, std::string & outError) override;
    virtual bool WriteFloat(float value, std::string name, std::string & outError) override;
    virtual bool WriteDouble(double value, std::string name, std::string & outError) override;
    virtual bool WriteString(const std::string & value, std::string name, std::string & outError) override;

    virtual bool WriteCollection(std::string name, ElementWriter writerFunc, const void* collection,
                                 unsigned int collectionSize, std::string & outError, void * optionalData = 0) override;

    virtual bool WriteDataStructure(const ISerializable & toSerialize, std::string name, std::string & outError) override;

private:

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement * currentRoot;
};


//Reads data from XML.
class XmlReader : public DataReader
{
public:

    XmlReader(std::string filePath, std::string & outErrorMsg);

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

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement * currentRoot,
                         * currentChild;
};