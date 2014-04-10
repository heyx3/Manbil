#pragma once

#include "Vectors.h"

template<class ArrayType>
//Wraps a contiguous heap-allocated one-dimensional array so it can be treated like a three-dimensional array.
class Fake3DArray
{
public:

	//Creates a new Fake3DArray without initializing any of the values.
	Fake3DArray(unsigned int aWidth, unsigned int aHeight, unsigned int aDepth)
	{
		width = aWidth;
		height = aHeight;
        depth = aDepth;

		arrayVals = new ArrayType[width * height * depth];
	}
    Fake3DArray(unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, const ArrayType & defaultValue)
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
    Fake3DArray(const Fake3DArray<ArrayType> & cpy); //Don't implement this function -- prevent accidental copying.
    Fake3DArray & operator=(const Fake3DArray<ArrayType> & other); //Don't implement this one either.

    ~Fake3DArray(void)
	{
		delete[] arrayVals;
	}

    void Reset(unsigned int _width, unsigned int _height, unsigned int _depth)
	{
		width = _width;
		height = _height;
        depth = _depth;

		delete[] arrayVals;
		arrayVals = new ArrayType[width * height * depth];
	}
    void Reset(unsigned int _width, unsigned int _height, unsigned int _depth, const ArrayType & defaultValue)
	{
		Reset(_width, _height, _depth);

        for (unsigned int i = 0; i < width * height * depth; ++i)
		{
			arrayVals[i] = defaultValue;
		}
	}

    Vector3i Clamp(Vector3i in) const { return Vector2i(BasicMath::Max(0, BasicMath::Min(GetWidth() - 1, in.x)),
                                                        BasicMath::Max(0, BasicMath::Min(GetHeight() - 1, in.y)),
                                                        BasicMath::Max(0, BasicMath::Min(GetDepth() - 1, in.z))); }
    Vector3f Clamp(Vector3f in) const { return Vector2i(BasicMath::Max(0, BasicMath::Min(GetWidth() - 1, in.x)),
                                                        BasicMath::Max(0, BasicMath::Min(GetHeight() - 1, in.y)),
                                                        BasicMath::Max(0, BasicMath::Min(GetDepth() - 1, in.z)));
    }
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

	ArrayType& operator[](Vector3i l) { return arrayVals[GetIndex(l.x, l.y, l.z)]; }
	const ArrayType& operator[](Vector3i l) const { return arrayVals[GetIndex(l.x, l.y, l.z)]; }

    unsigned int GetWidth(void) const { return width; }
    unsigned int GetHeight(void) const { return height; }
    unsigned int GetDepth(void) const { return depth; }

	//Fills every element with the given value.
	void Fill(const ArrayType & value)
	{
        for (int i = (width * height * depth) - 1; i >= 0; --i)
		{
			arrayVals[i] = value;
		}
	}
	//Copies the given array into this one. Optionally specifies an offset for the min position of "toCopy".
	void Fill(const Fake3DArray<ArrayType> & toCopy, const ArrayType & defaultValue, Vector3i copyOffset = Vector3i(0, 0, 0))
	{
        unsigned int x, y, z;
		Vector3i loc, offsetLoc;
		for (x = 0; x < width; ++x)
		{
			loc.x = x;

			for (y = 0; y < height; ++y)
			{
				loc.y = y;

                for (z = 0; z < depth; ++z)
                {
                    offsetLoc = loc + copyOffset;
                    if (offsetLoc.x < 0 || offsetLoc.y < 0 || offsetLoc.z < 0 ||
                        offsetLoc.x > width || offsetLoc.y > height || offsetLoc.z > depth)
                        operator[](loc) = defaultValue;
                    else operator[](loc) = toCopy[loc + copyOffset];
                }
			}
		}
	}
	template<typename Func>
	//Fills every element using the given function.
	//The function must have signature "void getValue(Vector3i loc, ArrayType * outValue)".
	void Fill(Func getValue)
	{
        unsigned int x, y, z;
		for (x = 0; x < width; ++x)
		{
			for (y = 0; y < height; ++y)
			{
                for (z = 0; z < depth; ++z)
                {
                    getValue(Vector2i(x, y, z), &arrayVals[GetIndex(x, y, z)]);
                }
			}
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

    unsigned int GetIndex(unsigned int x, unsigned int y, unsigned int z) const
    {
        return x + (y * width) + (z * width * height);
    }
};