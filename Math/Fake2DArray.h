#pragma once

#include "Vectors.h"

template<class ArrayType>
//Wraps a one-dimensional array so it can be treated like a two-dimensional array.
class Fake2DArray
{
public:

    //TODO: Change to use unsigned ints for the dimensions.

	//Creates a new Fake2DArray without initializing any of the values.
	Fake2DArray(int aWidth, int aHeight)
	{
		width = aWidth;
		height = aHeight;

		arrayVals = new ArrayType[width * height];
	}
	Fake2DArray(int aWidth, int aHeight, const ArrayType & defaultValue)
	{
		width = aWidth;
		height = aHeight;

		arrayVals = new ArrayType[width * height];

		for (int i = 0; i < width * height; ++i)
		{
			arrayVals[i] = defaultValue;
		}
	}
	Fake2DArray(const Fake2DArray<ArrayType> & cpy); //Don't implement this function -- prevent accidental copying.

	~Fake2DArray(void)
	{
		delete[] arrayVals;
	}

	void Reset(int _width, int _height)
	{
		width = _width;
		height = _height;

		delete[] arrayVals;
		arrayVals = new ArrayType[width * height];
	}
	void Reset(int _width, int _height, const ArrayType & defaultValue)
	{
		Reset(_width, _height);

		for (int i = 0; i < width * height; ++i)
		{
			arrayVals[i] = defaultValue;
		}
	}

	ArrayType& operator[](Vector2i l) { return arrayVals[GetIndex(l.x, l.y)]; }
	const ArrayType& operator[](Vector2i l) const { return arrayVals[GetIndex(l.x, l.y)]; }

	int GetWidth(void) const { return width; }
	int GetHeight(void) const { return height; }

	//Fills every element with the given value.
	void Fill(const ArrayType & value)
	{
		for (int i = (width * height) - 1; i >= 0; --i)
		{
			arrayVals[i] = value;
		}
	}
	//Copies the given array into this one. Optionally specifies an offset for the top-left position of "toCopy".
	void Fill(const Fake2DArray<ArrayType> & toCopy, const ArrayType & defaultValue, Vector2i copyOffset = Vector2i(0, 0))
	{
		int x, y;
		Vector2i loc, offsetLoc;
		for (x = 0; x < width; ++x)
		{
			loc.x = x;

			for (y = 0; y < height; ++y)
			{
				loc.y = y;

                offsetLoc = loc + copyOffset;
                if (offsetLoc.x < 0 || offsetLoc.y < 0 || offsetLoc.x > width || offsetLoc.y > height)
                    operator[](loc) = defaultValue;
                else operator[](loc) = toCopy[loc + copyOffset];
			}
		}
	}
	template<typename Func>
	//Fills every element using the given function.
	//The function must have signature "void getValue(Vector2i loc, ArrayType * outValue)".
	void Fill(Func getValue)
	{
		int x, y;
		for (x = 0; x < width; ++x)
		{
			for (y = 0; y < height; ++y)
			{
				getValue(Vector2i(x, y), &arrayVals[GetIndex(x, y)]);
			}
		}
	}

	ArrayType * GetArray(void) const { return arrayVals; }
	void GetArrayCopy(ArrayType * outValues) const
	{
		for (int i = 0; i < width * height; ++i)
		{
			outValues[i] = arrayVals[i];
		}
	}

private:

	int width, height;
	ArrayType * arrayVals;

	int GetIndex(int x, int y) const { return x + (y * width); }
};