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


const char * DataSerializer::FloatTag = "float",
		   * DataSerializer::IntTag = "int",
		   * DataSerializer::BoolTag = "bool",
		   * DataSerializer::StringTag = "string",
		   * DataSerializer::ClassTag = "class",
		   * DataSerializer::CollectionTag = "collection",
		   * DataSerializer::CollectionElementTag = "element";

//Evaluates to the special collection element tag if this DataSerializer is a collection serializer.
//Otherwise, evaluates to the given name.
#define CTAG(regularName) ((isCollectionSerializer ? CollectionElementTag : regularName))


DataSerializer::DataSerializer(std::string filePath, tinyxml2::XMLDocument & doc)
: rootElement(0), rootDocument(doc), errorMsg(""), isCollectionSerializer(false)
{
    //Try opening the document.
    tinyxml2::XMLError error = rootDocument.LoadFile(filePath.c_str());
    if (error != 0)
    {
        errorMsg = std::string("Error loading document '") + filePath + "': " + std::to_string(error);
        return;
    }


    //Try getting the root element.

    XMLNode * childNode = rootDocument.FirstChild();
    while (childNode != NULL && childNode->ToElement() == NULL)
        childNode = childNode->NextSibling();

    rootElement = childNode->ToElement();
    if (rootElement == NULL) errorMsg = "Root element is NULL.";
}

MaybeValue<XMLElement*> DataSerializer::findChildElement(const char * tag, const char * name, XMLElement* rootEl)
{
	if (rootEl == 0) rootEl = rootElement;

	for (XMLNode * child = rootEl->FirstChild(); child != NULL; child = child->NextSibling())
	{
		//Only looking at elements.
		XMLElement * childEl = child->ToElement();
		if (childEl != NULL)
		{
			//Look for the right tag.
			if (strcmp(childEl->Value(), tag) == 0)
			{
				//Look for the right name.
				if (strcmp(childEl->Attribute("name"), name) == 0)
				{
					return MaybeXMLElPtr(childEl);
				}
			}
			//Error-catching.
			else if (strcmp(childEl->Attribute("name"), name) == 0)
			{
				return MaybeXMLElPtr(std::string("An element with the name '") + name +
												 "' was found, but it was of type '" + childEl->Value() + "'");
			}
		}
	}

	return MaybeXMLElPtr(std::string("Couldn't find the element '") + name + "' of type '" + tag + "'");
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
bool DataSerializer::WriteClass(const char * name, ISerializable & value)
{
	const char * tag = CTAG(ClassTag);

	//If the field doesn't exist already, create it.
	MaybeXMLElPtr tryFind = findChildElement(tag, name);
	if (!tryFind.HasValue())
	{
		XMLElement * newChild = rootDocument.NewElement(tag);
		newChild->SetAttribute("name", name);
        rootElement->InsertEndChild(newChild);
		return value.WriteData(DataSerializer(newChild, rootDocument));
	}
	else
	{
		tryFind.GetValue()->DeleteChildren();
		return value.WriteData(DataSerializer(tryFind.GetValue(), rootDocument));
	}
}