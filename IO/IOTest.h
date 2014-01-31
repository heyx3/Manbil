#pragma once

#include "DataSerializer.h"

struct MiniIOTester : ISerializable
{
public:
	float Floats[10];
	virtual bool WriteData(DataSerializer & data) const override
	{
		return data.WriteCollection<float, float*>("MyFloats",
												   [](DataSerializer & data, const float ** collection, int index, const char * writeName) { return data.WriteFloat(writeName, (*collection)[index]); },
												   &Floats, 10);
	}
	virtual bool ReadData(DataSerializer & data) override
	{
		return data.ReadCollection<float, float*>("MyFloats",
												  [](DataSerializer & ser, float ** collection, int index, const char * readName) { return ser.ReadFloat(readName, &(*collection)[index]); },
												  &Floats);
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

	virtual bool WriteData(DataSerializer & data) const override
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