#pragma once

#include "DataSerializer.h"

struct MiniIOTester : ISerializable
{
public:
	float Floats[4];
	virtual bool WriteData(DataSerializer & data) override
	{
		float * floats = Floats;

		bool(*writerFunc)(DataSerializer & ser, float ** collection, int index, const char * writeName) =
			[](DataSerializer & ser, float ** flp, int i, const char * name)
				{ return ser.WriteFloat(name, (*flp)[i]); };

		return data.WriteCollection<float, float*>("MyFloats", writerFunc, &floats, 4);
	}
	virtual bool ReadData(DataSerializer & data) override
	{
		float * floats = Floats;
		return data.ReadCollection<float, float*>("MyFloats",
												  [](DataSerializer & ser, float ** collection, int index, const char * readName) -> bool
														{ return ser.ReadFloat(readName, &(*collection)[index]); },
												  &floats);
	}
};

struct IOTester : ISerializable
{
public:

	float Float;
	int Int;
	bool Bool;
	std::string String;
	MiniIOTester MiniMe;

	virtual bool WriteData(DataSerializer & data) override
	{
		return (data.WriteFloat("MyFloat", Float) &&
				data.WriteInt("MyInt", Int) &&
				data.WriteBoolean("MyBool", Bool) &&
				data.WriteString("MyString", String.c_str()) &&
				data.WriteClass("MiniMe", MiniMe));
	}
	virtual bool ReadData(DataSerializer & data) override
	{
		return (data.ReadFloat("MyFloat", &Float) &&
			    data.ReadInt("MyInt", &Int) &&
				data.ReadBoolean("MyBool", &Bool) &&
				data.ReadString("MyString", &String) &&
				data.ReadClass("MiniMe", &MiniMe));
	}
};