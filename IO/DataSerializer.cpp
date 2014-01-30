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
		   * const ClassTag = "class";



MaybeXMLElPtr DataSerializer::findChildElement(const char * tag, const char * name)
{
	XMLAttribute * childElName;

	for (const XMLNode * child = rootElement.FirstChild(); child != NULL; child = child->NextSibling())
	{
		XMLElement * childEl = child->ToElement();

		//Looking for elements.
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
	outV->insert()

	MaybeValue<std::string> tryParse = StringToType<float>(tryFind.GetValue()->GetText());
	if (!tryParse.HasValue())
	{
		errorMsg = tryParse.GetErrorMsg();
		return false;
	}

	*outV = tryParse.GetValue();
	return true;
}