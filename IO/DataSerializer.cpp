#include "DataSerializer.h"

using namespace tinyxml2;

std::string DataSerializer::findChildElement(const char * tag, const char * name, XMLElement * outEl)
{
	XMLElement * childEl;
	XMLAttribute * childElName;
	for (XMLNode * child = rootElement.FirstChild(); child != NULL; child = child->NextSibling())
	{
		childEl = child->ToElement();
		if (childEl != NULL)
		{
			const char * name = childEl->Attribute("name");
			if (childElName != NULL)
			{

			}
		}
	}

}

bool DataSerializer::ReadFloat(const char * name, float * outV)
{
	
}
bool DataSerializer::ReadInt(const char * name, int * outV)
{

}
bool DataSerializer::ReadBoolean(const char * name, bool * outV)
{

}
bool DataSerializer::ReadString(const char * name, std::string * outV)
{

}