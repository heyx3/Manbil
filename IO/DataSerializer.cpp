#include "DataSerializer.h"
#include <sstream>

using namespace tinyxml2;

typedef MaybeValue<XMLElement*> MaybeXMLElPtr;

template<typename Type>
MaybeValue<Type> StringToType(std::string str)
{
	Type t;
	std::stringstream ss(str);

	ss >> t;

	if (ss.fail())
		return MaybeValue<Type>("Invalid string");
	return MaybeValue<Type>(t);
}


const char * const FloatTag = "float",
		   * const IntTag = "int",
		   * const BoolTag = "bool",
		   * const StringTag = "string",
		   * const ClassTag = "class",
		   * const CollectionTag = "collection",
		   * const CollectionElementTag = "element";

//Evaluates to the special collection element tag if this DataSerializer is a collection serializer.
//Otherwise, evaluates to the given name.
#define CTAG(regularName) ((isCollectionSerializer ? CollectionElementTag : regularName))


MaybeXMLElPtr DataSerializer::findChildElement(const char * tag, const char * name)
{
	XMLAttribute * childElName;

	for (XMLNode * child = rootElement->FirstChild(); child != NULL; child = child->NextSibling())
	{
		//Only looking at elements.
		XMLElement * childEl = child->ToElement();
		if (childEl != NULL)
		{
			//Look for the right tag.
			if (childEl->Value() == tag)
			{
				//Look for the right name.
				const char * attName = childEl->Attribute("name");
				if (childEl->Attribute("name") == name)
				{
					return MaybeXMLElPtr(childEl);
				}
			}
			//Error-catching.
			else if (childEl->Attribute("name") == name)
			{
				return MaybeXMLElPtr(std::string("An element with the name '") + name +
												 "' was found, but it was of type '" + childEl->Value() + "'");
			}
		}
	}

	return MaybeXMLElPtr("Couldn't find the element!");
}

bool DataSerializer::ReadFloat(const char * name, float * outV)
{
	MaybeXMLElPtr tryFind = findChildElement(CTAG(FloatTag), name);
	if (!tryFind.HasValue())
	{
		errorMsg = tryFind.GetErrorMsg();
		return false;
	}

	MaybeValue<float> tryParse = StringToType<float>(tryFind.GetValue()->GetText());
	if (!tryParse.HasValue())
	{
		errorMsg = tryParse.GetErrorMsg();
		return false;
	}

	*outV = tryParse.GetValue();
	return true;
}
bool DataSerializer::ReadInt(const char * name, int * outV)
{
	MaybeXMLElPtr tryFind = findChildElement(CTAG(IntTag), name);
	if (!tryFind.HasValue())
	{
		errorMsg = tryFind.GetErrorMsg();
		return false;
	}

	MaybeValue<int> tryParse = StringToType<int>(tryFind.GetValue()->GetText());
	if (!tryParse.HasValue())
	{
		errorMsg = tryParse.GetErrorMsg();
		return false;
	}

	*outV = tryParse.GetValue();
	return true;
}
bool DataSerializer::ReadBoolean(const char * name, bool * outV)
{
	MaybeXMLElPtr tryFind = findChildElement(CTAG(BoolTag), name);
	if (!tryFind.HasValue())
	{
		errorMsg = tryFind.GetErrorMsg();
		return false;
	}

	MaybeValue<bool> tryParse = StringToType<bool>(tryFind.GetValue()->GetText());
	if (!tryParse.HasValue())
	{
		errorMsg = tryParse.GetErrorMsg();
		return false;
	}

	*outV = tryParse.GetValue();
	return true;
}
bool DataSerializer::ReadString(const char * name, std::string * outV)
{
	MaybeXMLElPtr tryFind = findChildElement(CTAG(StringTag), name);
	if (!tryFind.HasValue())
	{
		errorMsg = tryFind.GetErrorMsg();
		return false;
	}

	outV->clear();
	outV->append(tryFind.GetValue()->GetText());
	return true;
}
bool DataSerializer::ReadClass(const char * name, ISerializable * outV)
{
	MaybeXMLElPtr tryFind = findChildElement(CTAG(ClassTag), name);
	if (!tryFind.HasValue())
	{
		errorMsg = tryFind.GetErrorMsg();
		return false;
	}

	return outV->ReadData(DataSerializer(tryFind.GetValue(), rootDocument));
}


bool DataSerializer::WriteFloat(const char * name, float value)
{
	const char * tag = CTAG(FloatTag);

	//If the field doesn't exist already, create it.
	MaybeXMLElPtr tryFind = findChildElement(tag, name);
	if (!tryFind.HasValue())
	{
		XMLElement * newChild = rootDocument.NewElement(tag);
		newChild->SetText(std::to_string(value).c_str());
		newChild->SetAttribute("name", name);
		rootElement->InsertEndChild(newChild);
	}
	else
	{
		tryFind.GetValue()->SetText(std::to_string(value).c_str());
	}

	return true;
}
bool DataSerializer::WriteInt(const char * name, int value)
{
	const char * tag = CTAG(IntTag);

	//If the field doesn't exist already, create it.
	MaybeXMLElPtr tryFind = findChildElement(tag, name);
	if (!tryFind.HasValue())
	{
		XMLElement * newChild = rootDocument.NewElement(tag);
		newChild->SetText(std::to_string(value).c_str());
		newChild->SetAttribute("name", name);
		rootElement->InsertEndChild(newChild);
	}
	else
	{
		tryFind.GetValue()->SetText(std::to_string(value).c_str());
	}

	return true;
}
bool DataSerializer::WriteBoolean(const char * name, bool value)
{
	const char * tag = CTAG(BoolTag);

	//If the field doesn't exist already, create it.
	MaybeXMLElPtr tryFind = findChildElement(tag, name);
	if (!tryFind.HasValue())
	{
		XMLElement * newChild = rootDocument.NewElement(tag);
		newChild->SetText(std::to_string(value).c_str());
		newChild->SetAttribute("name", name);
		rootElement->InsertEndChild(newChild);
	}
	else
	{
		tryFind.GetValue()->SetText(std::to_string(value).c_str());
	}

	return true;
}
bool DataSerializer::WriteString(const char * name, const char * value)
{
	const char * tag = CTAG(StringTag);

	//If the field doesn't exist already, create it.
	MaybeXMLElPtr tryFind = findChildElement(tag, name);
	if (!tryFind.HasValue())
	{
		XMLElement * newChild = rootDocument.NewElement(tag);
		newChild->SetText(value);
		newChild->SetAttribute("name", name);
		rootElement->InsertEndChild(newChild);
	}
	else
	{
		tryFind.GetValue()->SetText(value);
	}

	return true;
}
bool DataSerializer::WriteClass(const char * name, const ISerializable & value)
{
	const char * tag = CTAG(ClassTag);

	//If the field doesn't exist already, create it.
	MaybeXMLElPtr tryFind = findChildElement(tag, name);
	if (!tryFind.HasValue())
	{
		XMLElement * newChild = rootDocument.NewElement(tag);
		newChild->SetAttribute("name", name);
		return value.WriteData(DataSerializer(newChild, rootDocument));
	}
	else
	{
		tryFind.GetValue()->DeleteChildren();
		return value.WriteData(DataSerializer(tryFind.GetValue(), rootDocument));
	}
}


template<typename ValueType, typename CollectionType>
bool DataSerializer::WriteCollection(const char * name,
									 bool(*writer)(DataSerializer & ser, const CollectionType * collection, int index, const char * writeName),
								     const CollectionType * collectionP, int size)
{
	const CollectionType & collection = *collectionP;

	const char * tag = CTAG(CollectionTag);

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
	for (int i = 0; i < size; ++i)
	{
		//Write the child element.
		DataSerializer elementSerializer(el, rootDocument);
		if (!writer(elementSerializer, collection, i, std::to_string(i)))
		{
			errorMsg = std::string("Error writing collection: ") + elementSerializer.errorMsg;
			return false;
		}
	}

	return true;
}

template<typename ValueType, typename CollectionType>
bool DataSerializer::ReadCollection(const char * name,
									bool(*reader)(DataSerializer & ser, CollectionType * collection, int index, const char * readName),
									CollectionType * collectionP)
{
	CollectionType & collection = *collectionP;

	const char * tag = CTAG(CollectionTag);

	//Find the collection to read.
	Maybevalue<XMLElement*> tryFind = findChildElement(tag, name);
	if (!tryFind.HasValue())
	{
		errorMsg = tryFind.GetErrorMsg();
		return false;
	}

	//Read each value.
	int index = 0;
	for (MaybeXMLElPtr child = findChildElement(CollectionElementTag, std::to_string(index).c_str());
		 child.HasValue(); child = findChildElement(CollectionElementTag, std::to_string(index).c_str()))
	{
		DataSerializer ser(child->ToElement(), rootDocument, 0);
		if (!reader(ser, collection, index, std::to_string(index)))
		{
			errorMsg = std::string("Error reading collecion element ") + std::to_string(index) + ": " + ser.errorMsg;
			return false;
		}

		index += 1;
	}

	return true;
}