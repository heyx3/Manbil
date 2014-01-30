#pragma once

#include "Vectors.h"

template<class ArrayType>
//Wraps a one-dimensional array so it can be treated like a three-dimensional array.
class Fake3DArray
{
public:

	//Creates a new Fake3DArray without initializing any of the values.
	Fake3DArray(int aWidth, int aHeight, int aDepth)
	{
		width = aWidth;
		height = aHeight;
		depth = aDepth;

		arrayVals = new ArrayType[width * height * depth];
	}
	Fake3DArray(int aWidth, int aHeight, int aDepth, const ArrayType defaultValue)
	{
		width = aWidth;
		height = aHeight;
		depth = aDepth;

		arrayVals = new ArrayType[width * height * depth];

		for (int i = 0; i < width * height * depth; ++i)
		{
			arrayVals[i] = defaultValue;
		}
	}
	Fake3DArray(const Fake3DArray<ArrayType> & cpy)
	{
		width = cpy.width;
		height = cpy.height;
		depth = cpy.depth;

		arrayVals = new ArrayType[width * height * depth];
		Fill(cpy);
	}

	~Fake3DArray(void)
	{
		delete[] arrayVals;
	}

	void Reset(int _width, int _height, int _depth)
	{
		width = _width;
		height = _height;
		depth = _depth;

		delete[] arrayVals;
		arrayVals = new ArrayType[width * height * depth];
	}
	void Reset(int _width, int _height, int aDepth, const ArrayType defaultValue)
	{
		Reset(_width, _height, aDepth);

		for (int i = 0; i < width * height * depth; ++i)
		{
			arrayVals[i] = defaultValue;
		}
	}

	ArrayType& operator[](Vector3i l) { return arrayVals[GetIndex(l.x, l.y, l.z)]; }
	const ArrayType& operator[](Vector3i l) const { return arrayVals[GetIndex(l.x, l.y, l.z)]; }

	int GetWidth(void) const { return width; }
	int GetHeight(void) const { return height; }
	int GetDepth(void) const { return depth; }

	//Fills every element with the given value.
	void Fill(const ArrayType & value)
	{
		for (int i = (width * height * depth) - 1; i >= 0; --i)
		{
			arrayVals[i] = value;
		}
	}
	//Copies the given array into this one.
	void Fill(const Fake3DArray<ArrayType> & toCopy, Vector3i copyOffset = Vector3i(0, 0))
	{
		int x, y, z;
		Vector3i loc;
		for (x = 0; x < width; ++x)
		{
			loc.x = x;

			for (y = 0; y < height; ++y)
			{
				loc.y = y;

				for (z = 0; z < depth; ++z)
				{
					(*this)[loc] = toCopy[loc + copyOffset];
				}
			}
		}
	}
	template<typename Func>
	//Fills every element using the given function.
	//The function must have signature "void getValue(Vector3i loc, ArrayType * outValue)".
	void Fill(Func getValue)
	{
		int x, y;
		for (x = 0; x < width; ++x)
		{
			for (y = 0; y < height; ++y)
			{
				for (z = 0; z < depth; ++z)
				{
					getValue(Vector3i(x, y, z), &arrayVals[GetIndex(x, y, z)]);
				}
			}
		}
	}

	ArrayType * GetArray(void) const { return arrayVals; }
	void GetArrayCopy(ArrayType * outValues) const
	{
		for (int i = 0; i < width * height * depth; ++i)
		{
			outValues[i] = arrayVals[i];
		}
	}

private:

	int width, height, depth;
	ArrayType * arrayVals;

	int GetIndex(int x, int y, int z) const { return x + (y * width) + (z * width * height); }
};