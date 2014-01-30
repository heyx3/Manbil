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
		   * const CollectionTag = "collection";



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
	MaybeXMLElPtr tryFind = findChildElement(FloatTag, name);
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
	MaybeXMLElPtr tryFind = findChildElement(IntTag, name);
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
	MaybeXMLElPtr tryFind = findChildElement(BoolTag, name);
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
	MaybeXMLElPtr tryFind = findChildElement(StringTag, name);
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
	MaybeXMLElPtr tryFind = findChildElement(ClassTag, name);
	if (!tryFind.HasValue())
	{
		errorMsg = tryFind.GetErrorMsg();
		return false;
	}

	return outV->ReadData(DataSerializer(tryFind.GetValue(), rootDocument));
}


bool DataSerializer::WriteFloat(const char * name, float value)
{
	//If the field doesn't exist already, create it.
	MaybeXMLElPtr tryFind = findChildElement(FloatTag, name);
	if (!tryFind.HasValue())
	{
		XMLElement * newChild = rootDocument.NewElement(FloatTag);
		newChild->SetText(std::to_string(value).c_str());
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
	//If the field doesn't exist already, create it.
	MaybeXMLElPtr tryFind = findChildElement(IntTag, name);
	if (!tryFind.HasValue())
	{
		XMLElement * newChild = rootDocument.NewElement(IntTag);
		newChild->SetText(std::to_string(value).c_str());
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
	//If the field doesn't exist already, create it.
	MaybeXMLElPtr tryFind = findChildElement(BoolTag, name);
	if (!tryFind.HasValue())
	{
		XMLElement * newChild = rootDocument.NewElement(BoolTag);
		newChild->SetText(std::to_string(value).c_str());
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
	//If the field doesn't exist already, create it.
	MaybeXMLElPtr tryFind = findChildElement(StringTag, name);
	if (!tryFind.HasValue())
	{
		XMLElement * newChild = rootDocument.NewElement(StringTag);
		newChild->SetText(value);
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
	//If the field doesn't exist already, create it.
	MaybeXMLElPtr tryFind = findChildElement(ClassTag, name);
	if (!tryFind.HasValue())
	{
		XMLElement * newChild = rootDocument.NewElement(ClassTag);
		return value.WriteData(DataSerializer(newChild, rootDocument));
	}
	else
	{
		tryFind.GetValue()->DeleteChildren();
		return value.WriteData(DataSerializer(tryFind.GetValue(), rootDocument));
	}
}


template<typename ValueType, typename CollectionType>
bool DataSerializer::WriteCollection(const char * name, WriteCollectionElement<ValueType, CollectionType> writer,
								     CollectionType<ValueType> & collection, int size)
{
	XMLElement * el = 0;

	//If the element doesn't exist already, create it.
	MaybeValue<XMLElement*> tryFind = findChildElement(CollectionTag, name);
	if (!tryFind.HasValue())
	{
		el = rootDocument.NewElement(name);
		rootElement->InsertEndChild(el);
	}
	//Otherwise, clear out the values that were already there.
	else
	{
		el = tryFind.GetValue();
		el->DeleteChildren();
	}

	//Write each value to the collection.
	for (int i = 0; i < size; ++i)
	{
		//Create the child element.
		XMLElement * innerEl = rootDocument.NewElement(ClassTag);
		el->InsertEndChild(innerEl);
		innerEl->SetAttribute("name", std::to_string(i));
		
		//Write to the child element.
		DataSerializer elementSerializer(el, rootDocument);
		if (!writer(elementSerializer, collection, i))
		{
			errorMsg = std::string("Error writing collection: ") + elementSerializer.errorMsg;
			return false;
		}
	}

	return true;
}

template<typename ValueType, typename CollectionType>
bool DataSerializer::ReadCollection(const char * name, ReadCollectionElement<ValueType, CollectionType> reader, CollectionType<ValueType> & collection)
{
	//Find the collection to read.
	Maybevalue<XMLElement*> tryFind = findChildElement(CollectionTag, name);
	if (!tryFind.HasValue())
	{
		errorMsg = tryFind.GetErrorMsg();
		return false;
	}

	//Read each value.
	int index = 0;
	for (MaybeXMLElPtr child = findChildElement(ClassTag, std::to_string(index).c_str());
		child.HasValue(); child = findChildElement(ClassTag, std::to_string(index).c_str()))
	{
		DataSerializer ser(child->ToElement(), rootDocument);
		if (!reader(ser, collection, index))
		{
			errorMsg = std::string("Error reading collecion element ") + std::to_string(index) + ": " + ser.errorMsg;
			return false;
		}

		index += 1;
	}

	return true;
}