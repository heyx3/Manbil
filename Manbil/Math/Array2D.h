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
        //Only resize if the current array does not have the same number of elements.
        if ((width * height) != (_width * _height))
        {
            delete[] arrayVals;
            arrayVals = new ArrayType[_width * _height];
        }

        width = _width;
        height = _height;
	}
    void Reset(unsigned int _width, unsigned int _height, const ArrayType & defaultValue)
	{
		Reset(_width, _height);
        for (unsigned int i = 0; i < width * height; ++i)
			arrayVals[i] = defaultValue;
	}

    Vector2i Clamp(Vector2i in) const
    {
        return Vector2i(BasicMath::Clamp<int>(in.x, 0, GetWidth() - 1),
                        BasicMath::Clamp<int>(in.y, 0, GetHeight() - 1));
    }
    Vector2u Clamp(Vector2u in) const
    {
        return Vector2u(BasicMath::Min<unsigned int>(in.x, GetWidth() - 1),
                        BasicMath::Min<unsigned int>(in.y, GetHeight() - 1));
    }
    Vector2f Clamp(Vector2f in) const
    {
        return Vector2f(BasicMath::Clamp<float>(in.x, 0.0f, GetWidth() - 1),
                        BasicMath::Clamp<float>(in.y, 0.0f, GetHeight() - 1));
    }
    Vector2i Wrap(Vector2i in) const
    {
        while (in.x < 0) in.x += GetWidth();
        while (in.x >= GetWidth()) in.x -= GetWidth();
        while (in.y < 0) in.y += GetHeight();
        while (in.y >= GetHeight()) in.y -= GetHeight();

        return in;
    }
    Vector2u Wrap(Vector2u in) const
    {
        while (in.x >= GetWidth()) in.x -= GetWidth();
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

	ArrayType& operator[](Vector2u l) { return arrayVals[GetIndex(l.x, l.y)]; }
	const ArrayType& operator[](Vector2u l) const { return arrayVals[GetIndex(l.x, l.y)]; }

    //Gets whether this array's width and height are the same.
    bool IsSquare(void) const { return width == height; }

    unsigned int GetWidth(void) const { return width; }
    unsigned int GetHeight(void) const { return height; }
    Vector2u GetDimensions(void) const { return Vector2u(width, height); }

    bool HasSameDimensions(const Array2D<ArrayType> & other) const
    {
        return width == other.width && height == other.height;
    }

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

        for (Vector2u loc; loc.y < height; ++loc.y)
        {
            offsetLoc.y = loc.y - copyOffset.y;

            for (loc.x = 0; loc.x < width; ++loc.x)
            {
                offsetLoc.x = loc.x - copyOffset.x;

                bool inToCopy = (offsetLoc.x >= 0) && (offsetLoc.y >= 0);
                operator[](loc) = (inToCopy ? toCopy[Vector2u(offsetLoc.x, offsetLoc.y)] : defaultValue);
            }
        }
	}
    //Copies the given elements to this array. Assumes that the size of this array matches with the given one.
    //If "useMemcpy" is true, this array will have its exact binary data copied quickly using memcpy.
    //Otherwise, each element will be set using its assignment operator.
    void Fill(const ArrayType * values, bool useMemcpy)
    {
        if (useMemcpy) memcpy(arrayVals, values, width * height * sizeof(ArrayType));
        else for (unsigned int i = 0; i < (width * height); ++i)
            arrayVals[i] = values[i];
    }

    template<typename Func>
    //Fills every element using the given function.
    //The function must have signature "void getValue(Vector2u loc, ArrayType * outValue)".
    void FillFunc(Func getValue)
    {
        for (Vector2u loc; loc.y < height; ++loc.y)
            for (loc.x = 0; loc.x < width; ++loc.x)
                getValue(loc, &arrayVals[GetIndex(loc.x, loc.y)]);
    }

    //Sets the given array to be a rotated version of this array.
    //"useFastCopy" determines whether to use memcpy on the whole thing (faster)
    //    or the assignment operator on each element (slower).
    //The given array will be automatically resized to fit.
    //Rotates this array by the given number of 90-degree clockwise rotations.
    void RotateInto(int clockwiseRots, Array2D<ArrayType> & outArray, bool useFastCopy) const
    {
        //Wrap the value to the range [0, 3].
        while (clockwiseRots < 0) clockwiseRots += 1024;
        clockwiseRots %= 4;

        const Array2D<ArrayType> * thisA = this;

        switch (clockwiseRots)
        {
            case 0:
                outArray.Reset(width, height);
                outArray.Fill(arrayVals, useFastCopy);
                break;

            case 1:
                outArray.Reset(height, width);
                outArray.FillFunc([thisA](Vector2u loc, ArrayType * outValue)
                {
                    Vector2u locF(thisA->GetHeight() - 1 - loc.y, loc.x);
                    const ArrayType * value = &thisA->operator[](locF);
                    *outValue = *value;
                });
                break;

            case 2:
                outArray.Reset(height, width);
                outArray.FillFunc([thisA](Vector2u loc, ArrayType * outValue)
                {
                    *outValue = thisA->operator[](Vector2u(thisA->GetWidth() - 1 - loc.x,
                                                           thisA->GetHeight() - 1 - loc.y));
                });
                break;

            case 3:
                outArray.Reset(height, width);
                outArray.FillFunc([thisA](Vector2u loc, ArrayType * outValue)
                {
                    *outValue = thisA->operator[](Vector2u(loc.y, thisA->GetWidth() - 1 - loc.x));
                });
                break;

            default: assert(false);
        }
    }

    //Resizes this array to the given size, preserving all data
    //    (although some data will of course be lost if the array gets shortened).
    void Resize(unsigned int newWidth, unsigned int newHeight, const ArrayType & defaultVal)
    {
        if (width != newWidth || height != newHeight)
        {
            Array2D<ArrayType> newArr(newWidth, newHeight);
            newArr.Fill(*this, defaultVal);
            Reset(newWidth, newHeight);
            Fill(newArr, defaultVal);
        }
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

    unsigned int GetIndex(unsigned int x, unsigned int y) const
    {
        return x + (y * width);
    }
};

#pragma warning(default: 4018)