#pragma once

#include <string>
#include "tinyxml2.h"

//Reads/writes XML data from/to an XML element.
class DataSerializer
{
public:

	DataSerializer(tinyxml2::XMLElement element)
		: rootElement(element), errorMsg("")
	{

	}


	std::string GetErrorMsg(void) const { return errorMsg; }
	bool HasError(void) const { return !errorMsg.empty(); }

	void ClearErrorMsg(void) { errorMsg.clear(); }


	//Getter/setter functions return whether or not they were successful.
	//If they were not successful, writes an error to this serializer's error message.

	bool ReadFloat(const char * name, float * outValue);
	bool ReadInt(const char * name, int * outValue);
	bool ReadBoolean(const char * name, bool * outValue);
	bool ReadString(const char * name, std::string * outValue);

	bool WriteFloat(const char * name, float value);
	bool WriteInt(const char * name, int value);
	bool WriteBoolean(const char * name, bool value);
	bool WriteString(const char * name, const char * value);

	//TODO: Write/read collections.


private:

	//Returns an error message, or "" if everything went fine.
	std::string findChildElement(const char * tag, const char * childName, tinyxml2::XMLElement * outValue) const;

	tinyxml2::XMLElement rootElement;
	std::string errorMsg;
};