#pragma once

#include "Vectors.h"

template<class ArrayType>
//Wraps a contiguous heap-allocated one-dimensional array so it can be treated like a two-dimensional array.
class Fake2DArray
{
public:

	//Creates a new Fake2DArray without initializing any of the values.
	Fake2DArray(unsigned int aWidth, unsigned int aHeight)
	{
		width = aWidth;
		height = aHeight;

		arrayVals = new ArrayType[width * height];
	}
    Fake2DArray(unsigned int aWidth, unsigned int aHeight, const ArrayType & defaultValue)
	{
		width = aWidth;
		height = aHeight;

		arrayVals = new ArrayType[width * height];

        for (unsigned int i = 0; i < width * height; ++i)
		{
			arrayVals[i] = defaultValue;
		}
	}
    Fake2DArray(const Fake2DArray<ArrayType> & cpy); //Don't implement this function -- prevent accidental copying.
    Fake2DArray & operator=(const Fake2DArray<ArrayType> & other); //Don't implement this one either.

	~Fake2DArray(void)
	{
		delete[] arrayVals;
	}

    void Reset(unsigned int _width, unsigned int _height)
	{
		width = _width;
		height = _height;

		delete[] arrayVals;
		arrayVals = new ArrayType[width * height];
	}
    void Reset(unsigned int _width, unsigned int _height, const ArrayType & defaultValue)
	{
		Reset(_width, _height);

        for (unsigned int i = 0; i < width * height; ++i)
		{
			arrayVals[i] = defaultValue;
		}
	}

    Vector2i Clamp(Vector2i in) const { return Vector2i(BasicMath::Max(0, BasicMath::Min(GetWidth() - 1, in.x)),
                                                        BasicMath::Max(0, BasicMath::Min(GetHeight() - 1, in.y))); }
    Vector2f Clamp(Vector2f in) const { return Vector2i(BasicMath::Max(0, BasicMath::Min(GetWidth() - 1, in.x)),
                                                        BasicMath::Max(0, BasicMath::Min(GetHeight() - 1, in.y))); }
    Vector2i Wrap(Vector2i in) const
    {
        while (in.x < 0) in.x += GetWidth();
        while (in.x >= GetWidth()) in.x -= GetWidth();
        while (in.y < 0) in.y += GetHeight();
        while (in.y >= GetHeight()) in.y -= GetHeight();

        return in;
    }
    Vector2f Wrap(Vector2f in) const
    {
        while (in.x < 0.0f) in.x += GetWidth();
        while (in.x >= GetWidth()) in.x -= GetWidth();
        while (in.y < 0.0f) in.y += GetHeight();
        while (in.y >= GetHeight()) in.y -= GetHeight();

        return in;
    }

	ArrayType& operator[](Vector2i l) { return arrayVals[GetIndex(l.x, l.y)]; }
	const ArrayType& operator[](Vector2i l) const { return arrayVals[GetIndex(l.x, l.y)]; }

    unsigned int GetWidth(void) const { return width; }
    unsigned int GetHeight(void) const { return height; }

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
        unsigned int x, y;
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
        unsigned int x, y;
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
        for (unsigned int i = 0; i < width * height; ++i)
		{
			outValues[i] = arrayVals[i];
		}
	}

private:

    unsigned int width, height;
	ArrayType * arrayVals;

    unsigned int GetIndex(unsigned int x, unsigned int y) const { return x + (y * width); }
};