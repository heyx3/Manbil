#pragma once

#include "Vectors.h"

#pragma warning(disable: 4018)


template<class ArrayType>
//Wraps a contiguous heap-allocated one-dimensional array so it can be treated like a three-dimensional array.
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
    Array3D(const Array3D<ArrayType> & cpy); //Don't implement this function -- prevent accidental copying.
    Array3D & operator=(const Array3D<ArrayType> & other); //Don't implement this one either.

    ~Array3D(void)
	{
		delete[] arrayVals;
	}

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
    void Reset(unsigned int _width, unsigned int _height, unsigned int _depth, const ArrayType & defaultValue)
	{
		Reset(_width, _height, _depth);

        for (unsigned int i = 0; i < width * height * depth; ++i)
		{
			arrayVals[i] = defaultValue;
		}
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

    Vector3i Clamp(Vector3i in) const { return Vector3i(BasicMath::Clamp<int>(in.x, 0, GetWidth() - 1),
                                                        BasicMath::Clamp<int>(in.y, 0, GetHeight() - 1),
                                                        BasicMath::Clamp<int>(in.z, 0, GetDepth() - 1));
    }
    Vector3u Clamp(Vector3u in) const
    {
        return Vector3u(BasicMath::Min<unsigned int>(in.x, GetWidth() - 1),
                        BasicMath::Min<unsigned int>(in.y, GetHeight() - 1),
                        BasicMath::Min<unsigned int>(in.z, GetDepth() - 1));
    }
    Vector3f Clamp(Vector3f in) const { return Vector3f(BasicMath::Clamp<float>(in.x, 0.0f, GetWidth - 1),
                                                        BasicMath::Clamp<float>(in.y, 0.0f, GetHeight() - 1),
                                                        BasicMath::Clamp<float>(in.z, 0.0f, GetDepth() - 1)); }
    Vector3i Wrap(Vector3i in) const
    {
        while (in.x < 0) in.x += GetWidth();
        while (in.x >= GetWidth()) in.x -= GetWidth();
        while (in.y < 0) in.y += GetHeight();
        while (in.y >= GetHeight()) in.y -= GetHeight();
        while (in.z < 0) in.z += GetDepth();
        while (in.z >= GetDepth()) in.z -= GetDepth();

        return in;
    }
    Vector3u Wrap(Vector3u in) const
    {
        while (in.x >= GetWidth()) in.x -= GetWidth();
        while (in.y >= GetHeight()) in.y -= GetHeight();
        while (in.z >= GetDepth()) in.z -= GetDepth();

        return in;
    }
    Vector3f Wrap(Vector3f in) const
    {
        while (in.x < 0.0f) in.x += GetWidth();
        while (in.x >= GetWidth()) in.x -= GetWidth();
        while (in.y < 0.0f) in.y += GetHeight();
        while (in.y >= GetHeight()) in.y -= GetHeight();
        while (in.z < 0.0f) in.z += GetDepth();
        while (in.z >= GetDepth()) in.z -= GetDepth();

        return in;
    }

	ArrayType& operator[](Vector3u l) { return arrayVals[GetIndex(l.x, l.y, l.z)]; }
    const ArrayType& operator[](Vector3u l) const { return arrayVals[GetIndex(l.x, l.y, l.z)]; }

    bool HasSameDimensions(const Array3D<ArrayType> & other) const
    {
        return width == other.width && height == other.height && depth == other.depth;
    }

    unsigned int GetWidth(void) const { return width; }
    unsigned int GetHeight(void) const { return height; }
    unsigned int GetDepth(void) const { return depth; }
    Vector3u GetDimensions(void) const { return Vector3u(width, height, depth); }
    unsigned int GetVolume(void) const { return width * height * depth; }

    unsigned int GetNumbElements(void) const { return width * height * depth; }

	//Fills every element with the given value.
	void Fill(const ArrayType & value)
	{
        for (int i = (width * height * depth) - 1; i >= 0; --i)
		{
			arrayVals[i] = value;
		}
	}
	//Copies the given array into this one. Optionally specifies an offset for the min position of "toCopy".
    void Fill(const Array3D<ArrayType> & toCopy, const ArrayType & defaultValue, Vector3i copyOffset = Vector3i(0, 0, 0))
    {
        Vector3i offsetLoc;

        for (Vector3u loc; loc.z < depth; ++loc.z)
        {
            offsetLoc.z = loc.z - copyOffset.z;

            for (loc.y = 0; loc.y < height; ++loc.y)
            {
                offsetLoc.y = loc.y - copyOffset.y;

                for (loc.x = 0; loc.x < width; ++loc.x)
                {
                    offsetLoc.x = loc.x - copyOffset.x;

                    bool inToCopy = (offsetLoc.x >= 0) && (offsetLoc.y >= 0) && (offsetLoc.z >= 0);
                    operator[](loc) = (inToCopy ? toCopy[Vector3u(offsetLoc.x, offsetLoc.y, offsetLoc.z)] : defaultValue);
                }
            }
        }
    }
    //Copies the given elements to this array. Assumes that the size of this array matches with the given one.
    void Fill(const ArrayType * values)
    {
        for (unsigned int i = 0; i < (width * height); ++i)
            arrayVals[i] = values[i];
    }

    template<typename Func>
    //Fills every element using the given function.
    //The function must have signature "void getValue(Vector3u loc, ArrayType * outValue)".
    void FillFunc(Func getValue)
    {
        Vector3u loc;
        for (loc.z = 0; loc.z < depth; ++loc.z)
        {
            for (loc.y = 0; loc.y < height; ++loc.y)
            {
                for (loc.x = 0; loc.x < width; ++loc.x)
                {
                    getValue(loc, &arrayVals[GetIndex(loc.x, loc.y, loc.z)]);
                }
            }
        }
    }

    //Resizes this array to the given size, preserving all data
    //    (although some data will of course be lost if the array gets shortened).
    void Resize(unsigned int newWidth, unsigned int newHeight, unsigned int newDepth, const ArrayType & defaultVal)
    {
        if (width != newWidth || height != newHeight || depth != newDepth)
        {
            Array3D<ArrayType> newArr(newWidth, newHeight, newDepth);
            newArr.Fill(*this, defaultVal);
            Reset(newWidth, newHeight);
            Fill(newArr, defaultVal);
        }
    }

	ArrayType * GetArray(void) const { return arrayVals; }
	void GetArrayCopy(ArrayType * outValues) const
	{
        for (unsigned int i = 0; i < width * height * depth; ++i)
		{
			outValues[i] = arrayVals[i];
		}
	}

private:

    unsigned int width, height, depth;
	ArrayType * arrayVals;
};

#pragma warning(default: 4018)