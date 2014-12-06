#pragma once

#include "Vectors.h"

#pragma warning(disable: 4018)


//The type of item this array contains. Should generally be trivially-copiable/assignable.
template<class ArrayType>
//Wraps a contiguous heap-allocated one-dimensional array
//    so it can be treated like a two-dimensional array.
//The most cache-efficient way to loop through this array is through
//    the Y in the outer loop and then the X in the inner loop.
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

    Array2D(void) = delete;
    Array2D(const Array2D<ArrayType>& cpy) = delete;
    Array2D & operator=(const Array2D<ArrayType>& other) = delete;

	~Array2D(void)
	{
		delete[] arrayVals;
	}


	ArrayType& operator[](Vector2u l) { return arrayVals[GetIndex(l.x, l.y)]; }
	const ArrayType& operator[](Vector2u l) const { return arrayVals[GetIndex(l.x, l.y)]; }


    //Gets the X size of this array.
    unsigned int GetWidth(void) const { return width; }
    //Gets the Y size of this array.
    unsigned int GetHeight(void) const { return height; }
    //Gets the size of this array along each axis.
    Vector2u GetDimensions(void) const { return Vector2u(width, height); }


    //Resets this array to the given size and leaves its elements uninitialized.
    //If the total number of elements doesn't change, then nothing is allocated or un-allocated
    //    and the elements keep their values.
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
    //Resets this array to the given size and initializes all elements to the given value.
    void Reset(unsigned int _width, unsigned int _height, const ArrayType& defaultValue)
	{
		Reset(_width, _height);
        Fill(defaultValue);
	}


    //Gets the array index for the given position.
    unsigned int GetIndex(unsigned int x, unsigned int y) const
    {
        return x + (y * width);
    }
    //Gets the location in this array that corresponds to the given array index.
    Vector2u GetLocation(unsigned int index) const
    {
        return Vector2u(index % width, index / width);
    }


    //Clamps the given index to be inside the range of allowable indices for this array.
    Vector2i Clamp(Vector2i in) const
    {
        return Vector2i(BasicMath::Clamp<int>(in.x, 0, GetWidth() - 1),
                        BasicMath::Clamp<int>(in.y, 0, GetHeight() - 1));
    }
    //Clamps the given index to be inside the range of allowable indices for this array.
    Vector2u Clamp(Vector2u in) const
    {
        return Vector2u(BasicMath::Min<unsigned int>(in.x, GetWidth() - 1),
                        BasicMath::Min<unsigned int>(in.y, GetHeight() - 1));
    }
    //Clamps the given index to be inside the range of allowable indices for this array.
    Vector2f Clamp(Vector2f in) const
    {
        return Vector2f(BasicMath::Clamp<float>(in.x, 0.0f, GetWidth() - 1),
                        BasicMath::Clamp<float>(in.y, 0.0f, GetHeight() - 1));
    }
    //Wraps the given index around the range of allowable indices for this array.
    Vector2i Wrap(Vector2i in) const
    {
        //First handle negative values.
        while (in.x < 0) in.x += GetWidth();
        while (in.y < 0) in.y += GetHeight();
        
        in.x %= GetWidth();
        in.y %= GetHeight();

        return in;
    }
    //Wraps the given index around the range of allowable indices for this array.
    Vector2u Wrap(Vector2u in) const
    {
        return Vector2u(in.x % GetWidth(), in.y % GetHeight());
    }
    //Wraps the given index around the range of allowable indices for this array.
    Vector2f Wrap(Vector2f in) const
    {
        Vector2f fDims = ToV2f(GetDimensions());

        //First handle negative values.
        while (in.x < 0.0f) in.x += fDims.x;
        while (in.y < 0.0f) in.y += fDims.y;

        in.x = fmodf(in.x, fDims.x);
        in.y = fmodf(in.y, fDims.y);

        return in;
    }


    //Gets whether this array's width and height are the same.
    bool IsSquare(void) const { return width == height; }

    //Gets whether this array has the same dimensions as the given one.
    bool HasSameDimensions(const Array2D<ArrayType>& other) const
    {
        return (width == other.width) && (height == other.height);
    }


    //Gets the total number of elements contained by this array.
    unsigned int GetNumbElements(void) const { return width * height; }


	//Fills every element with the given value.
	void Fill(const ArrayType& value)
	{
        for (unsigned int i = 0; i < width * height; ++i)
			arrayVals[i] = value;
	}
    //Copies the given array into this one. The given array may be offset a certain amount.
    //Any values of the given array that don't correspond to a value in this array are ignored.
	void Fill(const Array2D<ArrayType>& toCopy, Vector2i copyOffset = Vector2i(0, 0))
	{
		Vector2i offsetLoc;

        for (Vector2i loc; loc.y < toCopy.height; ++loc.y)
        {
            offsetLoc.y = loc.y + copyOffset.y;

            if (offsetLoc.y < 0)
                continue;
            if (offsetLoc.y >= height)
                break;

            for (loc.x = 0; loc.x < toCopy.width; ++loc.x)
            {
                offsetLoc.x = loc.x + copyOffset.x;

                if (offsetLoc.x >= width)
                    break;

                if (offsetLoc.x >= 0)
                    operator[](loc) = toCopy(ToV2u(offsetLoc));
            }
        }
	}
    //Copies the given elements to this array.
    //Assumes that the size of this array matches with the given one.
    //If "useMemcpy" is true, this array will have its exact binary data copied quickly using memcpy.
    //Otherwise, each element will be set using its assignment operator.
    void Fill(const ArrayType* values, bool useMemcpy)
    {
        if (useMemcpy)
            memcpy(arrayVals, values, width * height * sizeof(ArrayType));

        else for (unsigned int i = 0; i < (width * height); ++i)
            arrayVals[i] = values[i];
    }

    //A function with signature "void GetValue(Vector2u index, ArrayType* outNewValue)".
    template<typename Func>
    //Fills every element using the given function.
    void FillFunc(Func getValue)
    {
        Vector2u loc;
        for (loc.y = 0; loc.y < height; ++loc.y)
            for (loc.x = 0; loc.x < width; ++loc.x)
                getValue(loc, &arrayVals[GetIndex(loc.x, loc.y)]);
    }

    //Sets the given array to be a rotated version of this array.
    //The number of 90-degree rotations is specified by "clockwiseRots".
    //"useFastCopy" determines whether to use memcpy on the whole thing (faster)
    //    or the assignment operator on each element (slower).
    //The given array will be automatically resized to match this one's size if needed.
    void RotateInto(int clockwiseRots, Array2D<ArrayType>& outArray, bool useFastCopy) const
    {
        //Wrap the value to the range [0, 3].
        //Annoyingly, % operator doesn't work as expected for negative integers.
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
    //If the array is being extended, the new elements will be filled with the given default value.
    void Resize(unsigned int newWidth, unsigned int newHeight, const ArrayType& defaultVal)
    {
        if (with != newWidth || height != newHeight)
        {
            //Create a new version of this array with the new values.
            Array2D<ArrayType> newArr(newWidth, newHeight);
            newArr.Fill(*this, defaultVal);

            //Fill this array with those values.
            Reset(newWidth, newHeight);
            Fill(newArr, defaultVal);
        }
    }


    //Gets a pointer to the first element in this array.
    const ArrayType* GetArray(void) const { return arrayVals; }
    //Gets a pointer to the first element in this array.
    ArrayType* GetArray(void) { return arrayVals; }

    //Copies this array into the given one. Assumes it is the same size as this array.
	void CopyInto(ArrayType* outValues) const
	{
        for (unsigned int i = 0; i < width * height; ++i)
			outValues[i] = arrayVals[i];
	}


private:

    unsigned int width, height;
	ArrayType* arrayVals;
};

#pragma warning(default: 4018)