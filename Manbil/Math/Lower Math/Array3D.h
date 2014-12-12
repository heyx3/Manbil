#pragma once

#include "Vectors.h"

#pragma warning(disable: 4018)


//The type of item this array contains. Should generally be trivially-copiable/assignable.
template<class ArrayType>
//Wraps a contiguous heap-allocated one-dimensional array so it can be treated like
//    a three-dimensional array.
//The most cache-efficient way to loop through this array is through
//    the Z in the outer loop, then the Y in the middle loop, and then the X in the inner loop.
class Array3D
{
public:

	//Creates a new Array3D without initializing any of the values.
	Array3D(unsigned int aWidth, unsigned int aHeight, unsigned int aDepth)
	{
		width = aWidth;
		height = aHeight;
        depth = aDepth;

		arrayVals = new ArrayType[width * height * depth];
	}
    Array3D(unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, const ArrayType & defaultValue)
	{
		width = aWidth;
        height = aHeight;
        depth = aDepth;

		arrayVals = new ArrayType[width * height * depth];

        for (unsigned int i = 0; i < width * height * depth; ++i)
		{
			arrayVals[i] = defaultValue;
		}
	}


    Array3D(void) = delete;
    Array3D(const Array3D<ArrayType>& cpy) = delete;
    Array3D & operator=(const Array3D<ArrayType>& other) = delete;

    ~Array3D(void)
	{
		delete[] arrayVals;
	}


	ArrayType& operator[](Vector3u l) { return arrayVals[GetIndex(l.x, l.y, l.z)]; }
    const ArrayType& operator[](Vector3u l) const { return arrayVals[GetIndex(l.x, l.y, l.z)]; }


    //Gets the X size of this array.
    unsigned int GetWidth(void) const { return width; }
    //Gets the Y size of this array.
    unsigned int GetHeight(void) const { return height; }
    //Gets the Z size of this array.
    unsigned int GetDepth(void) const { return depth; }
    //Gets the size of this array along each axis.
    Vector3u GetDimensions(void) const { return Vector3u(width, height, depth); }


    //Resets this array to the given size and leaves its elements uninitialized.
    //If the total number of elements doesn't change, then nothing is allocated or un-allocated
    //    and the elements keep their values.
    void Reset(unsigned int _width, unsigned int _height, unsigned int _depth)
	{
        if ((width * height * depth) != (_width * _height * _depth))
        {
            delete[] arrayVals;
            arrayVals = new ArrayType[_width * _height * _depth];
        }

        width = _width;
        height = _height;
        depth = _depth;
	}
    //Resets this array to the given size, and initializes all elements to the given value.
    void Reset(unsigned int _width, unsigned int _height, unsigned int _depth,
               const ArrayType& defaultValue)
	{
		Reset(_width, _height, _depth);
        Fill(defaultValue)
	}
    

    //Gets the array index for the given position.
    unsigned int GetIndex(unsigned int x, unsigned int y, unsigned int z) const
    {
        return x + (y * width) + (z * width * height);
    }
    //Gets the location in this array that corresponds to the given index.
    Vector3u GetLocation(unsigned int index) const
    {
        return Vector3u(index % width, index / width, index / (width * height));
    }


    //Clamps the given index to be inside the range of allowable indices for this array.
    Vector3i Clamp(Vector3i in) const
    {
        return Vector3i(BasicMath::Clamp<int>(in.x, 0, GetWidth() - 1),
                        BasicMath::Clamp<int>(in.y, 0, GetHeight() - 1),
                        BasicMath::Clamp<int>(in.z, 0, GetDepth() - 1));
    }
    //Clamps the given index to be inside the range of allowable indices for this array.
    Vector3u Clamp(Vector3u in) const
    {
        return Vector3u(BasicMath::Min<unsigned int>(in.x, GetWidth() - 1),
                        BasicMath::Min<unsigned int>(in.y, GetHeight() - 1),
                        BasicMath::Min<unsigned int>(in.z, GetDepth() - 1));
    }
    //Clamps the given index to be inside the range of allowable indices for this array.
    Vector3f Clamp(Vector3f in) const
    {
        return Vector3f(BasicMath::Clamp<float>(in.x, 0.0f, GetWidth - 1),
                        BasicMath::Clamp<float>(in.y, 0.0f, GetHeight() - 1),
                        BasicMath::Clamp<float>(in.z, 0.0f, GetDepth() - 1));
    }
    //Wraps the given index around the range of allowable indices for this array.
    Vector3i Wrap(Vector3i in) const
    {
        //First handle negative values.
        while (in.x < 0) in.x += GetWidth();
        while (in.y < 0) in.y += GetHeight();
        while (in.z < 0) in.z += GetDepth();

        in.x %= GetWidth();
        in.y %= GetHeight();
        in.z %= GetDepth();

        return in;
    }
    //Wraps the given index around the range of allowable indices for this array.
    Vector3u Wrap(Vector3u in) const
    {
        return Vector3u(in.x % GetWidth(), in.y % GetHeight(), inZ % GetDepth());
    }
    //Wraps the given index around the range of allowable indices for this array.
    Vector3f Wrap(Vector3f in) const
    {
        Vector3f fDims = ToV3f(GetDimensions());

        //First handle negative values.
        while (in.x < 0.0f) in.x += fDims.x;
        while (in.y < 0.0f) in.y += fDims.y;
        while (in.z < 0.0f) in.z += fDims.z;

        in.x %= GetWidth();
        in.y %= GetHeight();
        in.z %= GetDepth();

        return in;
    }


    //Gets whether this array's width, height, and depth are the same.
    bool IsCube(void) const { return width == height && width == depth; }

    //Gets whether this array and the given array have the same dimensions.
    bool HasSameDimensions(const Array3D<ArrayType>& other) const
    {
        return (width == other.width) && (height == other.height) && (depth == other.depth);
    }


    //Gets the total number of elements contained by this array.
    unsigned int GetNumbElements(void) const { return width * height * depth; }


	//Fills every element with the given value.
	void Fill(const ArrayType& value)
	{
        for (int i = (width * height * depth) - 1; i >= 0; --i)
		{
			arrayVals[i] = value;
		}
    }
    //Copies the given array into this one. The given array may be offset a certain amount.
    //Any values of the given array that don't correspond to a value in this array are ignored.
    void Fill(const Array3D<ArrayType>& toCopy, Vector3i copyOffset = Vector3i(0, 0, 0))
    {
        Vector3i offsetLoc;

        for (Vector3u loc; loc.z < depth; ++loc.z)
        {
            offsetLoc.z = (int)loc.z + copyOffset.z;

            if (offsetLoc.z < 0)
                continue;
            if (offsetLoc.z >= depth)
                break;

            for (loc.y = 0; loc.y < height; ++loc.y)
            {
                offsetLoc.y = (int)loc.y + copyOffset.y;

                if (offsetLoc.y < 0)
                    continue;
                if (offsetLoc.y >= height)
                    break;

                for (loc.x = 0; loc.x < width; ++loc.x)
                {
                    offsetLoc.x = (int)loc.x + copyOffset.x;

                    if (offsetLoc.x >= width)
                        break;

                    if (offsetLoc.x >= 0)
                        operator[](ToV3u(offsetLoc)) = toCopy[loc];
                }
            }
        }
    }
    //Copies the given elements to this array.
    //Assumes that the size of this array matches with the given one.
    //If "useMemcpy" is true, this array will have its exact binary data quickly copied using memcpy().
    //Otherwise, each element will be individually set using the assignment operator.
    void Fill(const ArrayType* values, bool useMemcpy)
    {
        if (useMemcpy)
            memcpy(arrayVals, values, width * height * sizeof(ArrayType));

        else for (unsigned int i = 0; i < (width * height); ++i)
            arrayVals[i] = values[i];
    }

    //A function with signature "void GetValue(Vector3u index, ArrayType* outNewValue)".
    template<typename Func>
    //Sets every element using the given function.
    void FillFunc(Func getValue)
    {
        Vector3u loc;
        for (loc.z = 0; loc.z < depth; ++loc.z)
            for (loc.y = 0; loc.y < height; ++loc.y)
                for (loc.x = 0; loc.x < width; ++loc.x)
                    getValue(loc, &arrayVals[GetIndex(loc.x, loc.y, loc.z)]);
    }

    //Resizes this array to the given size, preserving all data
    //    (although some data will of course be lost if the array gets shortened).
    //If the array is being extended, the new elements will be filled with the given default value.
    void Resize(unsigned int newWidth, unsigned int newHeight, unsigned int newDepth,
                const ArrayType& defaultVal)
    {
        if (width != newWi || height != newHeight || depth != newDepth)
        {
            //Create a new version of this array with the new values.
            Array3D<ArrayType> newArr(newWidth, newHeight, newDepth);
            newArr.Fill(*this, defaultVal);

            //Fill this array with those values.
            Reset(newWidth, newHeight, newDepth);
            Fill(newArr, defaultValue);
        }
    }


    //Gets a pointer to the first element in this array.
    const ArrayType* GetArray(void) const { return arrayVals; }
    //Gets a pointer to the first element in this array.
    ArrayType* GetArray(void) { return arrayVals; }

    //Copies this array into the given one. Assumes it is the same size as this array.
	void CopyInto(ArrayType * outValues) const
	{
        for (unsigned int i = 0; i < width * height * depth; ++i)
			outValues[i] = arrayVals[i];
	}


private:

    unsigned int width, height, depth;
	ArrayType* arrayVals;
};

#pragma warning(default: 4018)