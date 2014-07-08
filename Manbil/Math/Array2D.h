#pragma once

#include "Vectors.h"


#pragma warning(disable: 4018)

template<class ArrayType>
//Wraps a contiguous heap-allocated one-dimensional array so it can be treated like a two-dimensional array.
class Array2D
{
public:

	//Creates a new Array2D without initializing any of the values.
	Array2D(unsigned int aWidth, unsigned int aHeight)
	{
		width = aWidth;
		height = aHeight;

		arrayVals = new ArrayType[width * height];
	}
    Array2D(unsigned int aWidth, unsigned int aHeight, const ArrayType & defaultValue)
	{
		width = aWidth;
		height = aHeight;

		arrayVals = new ArrayType[width * height];

        for (unsigned int i = 0; i < width * height; ++i)
		{
			arrayVals[i] = defaultValue;
		}
	}
    Array2D(const Array2D<ArrayType> & cpy); //Don't implement this function -- prevent accidental copying.
    Array2D & operator=(const Array2D<ArrayType> & other); //Don't implement this one either.

	~Array2D(void)
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

    Vector2i Clamp(Vector2i in) const { return Vector2i(BasicMath::Clamp<int>(in.x, 0, GetWidth() - 1), BasicMath::Clamp<int>(in.y, 0, GetHeight() - 1)); }
    Vector2f Clamp(Vector2f in) const { return Vector2f(BasicMath::Clamp<float>(in.x, 0.0f, GetWidth() - 1), BasicMath::Clamp<float>(in.y, 0.0f, GetHeight() - 1)); }
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
	void Fill(const Array2D<ArrayType> & toCopy, const ArrayType & defaultValue, Vector2i copyOffset = Vector2i(0, 0))
	{
		Vector2i offsetLoc;

        for (Vector2i loc; loc.y < height; ++loc.y)
        {
            offsetLoc.y = loc.y + copyOffset.y;

            for (loc.x = 0; loc.x < width; ++loc.x)
            {
                offsetLoc.x = loc.x + copyOffset.x;

                if (offsetLoc.x < 0 || offsetLoc.y < 0 ||
                    offsetLoc.x > width || offsetLoc.y > height ||
                    offsetLoc.x > toCopy.width || offsetLoc.y > toCopy.height)
                    operator[](loc) = defaultValue;
                else operator[](loc) = toCopy[offsetLoc];
            }
        }
	}
	template<typename Func>
	//Fills every element using the given function.
	//The function must have signature "void getValue(Vector2i loc, ArrayType * outValue)".
	void Fill(Func getValue)
	{
        for (Vector2i loc; loc.y < height; ++loc.y)
            for (loc.x = 0; loc.x < width; ++loc.x)
                getValue(loc, &arrayVals[GetIndex(loc.x, loc.y)]);
	}
    //Copies the given elements to this array. Assumes that the size of this array matches with the given one.
    void Fill(const ArrayType * values)
    {
        for (unsigned int i = 0; i < (width * height); ++i)
            arrayVals[i] = values[i];
    }

    //Resizes this array to the given size, preserving all data
    //    (although some data will of course be lost if the array gets shortened).
    void Resize(unsigned int newWidth, unsigned int newHeight, const ArrayType & defaultVal)
    {
        Array2D<ArrayType> newArr(newWidth, newHeight);
        newArr.Fill(*this, defaultVal);
        Reset(newWidth, newHeight);
        Fill(newArr, defaultVal);
    }

	const ArrayType * GetArray(void) const { return arrayVals; }
    ArrayType * GetArray(void) { return arrayVals; }

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

#pragma warning(default: 4018)