#pragma once

#include <string>
#include <assert.h>
#include "tinyxml2.h"

template<class Type>
//Represents a value that may or may not exist.
class MaybeValue
{
public:

	std::string GetErrorMsg(void) const { return errorMsg; }

	bool HasValue(void) const { return errorMsg.empty(); }
	Type GetValue(void) const { assert(HasValue());  return val; }

	MaybeValue(std::string errorMessage) : errorMsg(errorMessage) { }
	MaybeValue(Type value) : errorMsg(""), val(value) { }

private:

	std::string errorMsg;

	Type val;
};



class ISerializable;

//Reads/writes XML data from/to an XML element.
class DataSerializer
{
public:

	DataSerializer(tinyxml2::XMLElement * element, tinyxml2::XMLDocument & doc)
		: rootElement(element), rootDocument(doc), errorMsg(""), isCollectionSerializer(false)
	{

	}


	std::string GetErrorMsg(void) const { return errorMsg; }
	bool HasError(void) const { return !errorMsg.empty(); }

	void ClearErrorMsg(void) { errorMsg.clear(); }


	//Reading/writing functions return whether or not they were successful.
	//If they were not successful, writes an error to this serializer's error message.

	bool ReadFloat(const char * name, float * outValue);
	bool ReadInt(const char * name, int * outValue);
	bool ReadBoolean(const char * name, bool * outValue);
	bool ReadString(const char * name, std::string * outValue);
	bool ReadClass(const char * name, ISerializable * outValue);

	bool WriteFloat(const char * name, float value);
	bool WriteInt(const char * name, int value);
	bool WriteBoolean(const char * name, bool value);
	bool WriteString(const char * name, const char * value);
	bool WriteClass(const char * name, const ISerializable & value);


	//Reading/writing collections.

	template<typename ValueType, typename CollectionType>
	bool ReadCollection(const char * name,
						bool (*valueReader)(DataSerializer & ser, CollectionType * collection, int index, const char * writeName),
						CollectionType * outCollection);
	template<typename ValueType, typename CollectionType>
	bool WriteCollection(const char * name,
						 bool (*valueWriter)(DataSerializer & ser, const CollectionType * collection, int index, const char * writeName),
						 const CollectionType * collection, int collectionSize);
					   


private:

	//Returns an error message, or "" if everything went fine.
	MaybeValue<tinyxml2::XMLElement*> findChildElement(const char * tag, const char * childName);

	tinyxml2::XMLElement * rootElement;
	tinyxml2::XMLDocument & rootDocument;
	std::string errorMsg;


	//A special constructor that is only used to construct DataSerializers for writing/reading a collection.
	DataSerializer(tinyxml2::XMLElement * element, tinyxml2::XMLDocument & doc, int dummy)
		: rootElement(element), rootDocument(doc), errorMsg(""), isCollectionSerializer(true)
	{

	}

	bool isCollectionSerializer;
};



//Represents a class that can be written to/read from using a DataSerializer.
//The write/read functions both return whether or not they were successful.
struct ISerializable
{
public:

	virtual bool ReadData(DataSerializer & data) = 0;
	virtual bool WriteData(DataSerializer & data) const = 0;
};