#pragma once

#include <string>
#include <assert.h>
#include "tinyxml2.h"


//TODO: Abstract the serialization system so that the XML part can be easily swapped out for a different child class that uses something else, e.x. networking, plain text, or JSON.



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



struct ISerializable;

//Reads/writes XML data from/to an XML element.
class DataSerializer
{
public:

	DataSerializer(tinyxml2::XMLElement * element, tinyxml2::XMLDocument & doc)
		: rootElement(element), rootDocument(doc), errorMsg(""), isCollectionSerializer(false)
	{
		if (element == NULL) errorMsg = "Root Element is NULL.";
	}
    //Gets a serializer for the root element in the given XML file.
    //Also takes in an uninitialized XMLDocument instance.
    DataSerializer(std::string filePath, tinyxml2::XMLDocument & doc);


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
	bool WriteClass(const char * name, ISerializable & value);


	//Reading/writing collections.

	template<typename ValueType, typename CollectionType>
	inline bool ReadCollection(const char * name,
							   bool(*valueReader)(DataSerializer & ser, CollectionType * collection, int index, const char * writeName),
							   CollectionType * outCollection)
	{
		const char * tag = (isCollectionSerializer ? CollectionElementTag : CollectionTag);

		//Find the collection to read.
		MaybeValue<XMLElement*> tryFind = findChildElement(tag, name);
		if (!tryFind.HasValue())
		{
			errorMsg = tryFind.GetErrorMsg();
			return false;
		}

		//Read each value.
		int index = 0;
		DataSerializer ser(tryFind.GetValue(), rootDocument, 0);
		for (MaybeValue<XMLElement*> child = findChildElement(CollectionElementTag, std::to_string(index).c_str(), tryFind.GetValue());
			 child.HasValue();
			 child = findChildElement(CollectionElementTag, std::to_string(index).c_str(), tryFind.GetValue()))
		{
			if (!valueReader(ser, outCollection, index, std::to_string(index).c_str()))
			{
				errorMsg = std::string("Error reading collection element ") + std::to_string(index) + ": " + ser.errorMsg;
				return false;
			}

			index += 1;
		}

		return true;
	}
	template<typename ValueType, typename CollectionType>
	inline bool WriteCollection(const char * name,
								bool(*valueWriter)(DataSerializer & ser, CollectionType * collection, int index, const char * writeName),
								CollectionType * collectionP, int collectionSize)
	{
		const char * tag = (isCollectionSerializer ? CollectionElementTag : CollectionTag);

		XMLElement * el = 0;

		//If the element doesn't exist already, create it.
		MaybeValue<XMLElement*> tryFind = findChildElement(tag, name);
		if (!tryFind.HasValue())
		{
			el = rootDocument.NewElement(tag);
			rootElement->InsertEndChild(el);
			el->SetAttribute("name", name);
		}
		//Otherwise, clear out the values that were already there.
		else
		{
			el = tryFind.GetValue();
			el->DeleteChildren();
		}

		//Write each value to the collection.
		DataSerializer elementSerializer(el, rootDocument, 0);
		for (int i = 0; i < collectionSize; ++i)
		{
			//Write the child element.
			DataSerializer elementSerializer(el, rootDocument);
			if (!valueWriter(elementSerializer, collectionP, i, std::to_string(i).c_str()))
			{
				errorMsg = std::string("Error writing collection: ") + elementSerializer.errorMsg;
				return false;
			}
		}

		return true;
	}



private:

	static const char * FloatTag,
				      * IntTag,
				      * BoolTag,
				      * StringTag,
				      * ClassTag,
				      * CollectionTag,
				      * CollectionElementTag;

	//Returns an error message, or "" if everything went fine.
	MaybeValue<tinyxml2::XMLElement*> findChildElement(const char * tag, const char * childName, tinyxml2::XMLElement* rootEl = 0);

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
	virtual bool WriteData(DataSerializer & data) = 0;
};