#pragma once

#include "../Lower Math/Vectors.h"


//Custom math functions. Generalized for a float vector of any dimensionality.
class Geometryf
{
private:
	
    static inline bool WithinError(float f1, float f2, float error)
    {
        return Mathf::Abs(f1 - f2) <= error;
    }


#pragma warning(disable: 4100)
    //A vector class with no data beyond its components.
	template<class Vector>
	static int NumbDimensions(Vector v) { return sizeof(v) / sizeof(float); }
	template<>
	static int NumbDimensions<Vector2f>(Vector2f v) { return 2; }
	template<>
	static int NumbDimensions<Vector3f>(Vector3f v) { return 3; }
	template<>
	static int NumbDimensions<Vector4f>(Vector4f v) { return 4; }
#pragma warning(default: 4100)

public:

    //A vector class that has "Normalized" and "Dot" methods.
	template<class Vector>
	static bool AreColinear(Vector p1, Vector p2, Vector p3, float errorMargin = 0.1f)
	{
		Vector p12 = (p1 - p2).Normalized(),
			   p13 = (p1 - p3).Normalized(),
			   p23 = (p2 - p3).Normalized();
        return (WithinError(Mathf::Abs(p12.Dot(p13)), 1.0f, errorMargin) &&
                WithinError(Mathf::Abs(p12.Dot(p23)), 1.0f, errorMargin));
    }
    //A vector class that has "FastNormalized" and "Dot" methods.
    template<class Vector>
    //Like "AreColinear", but using a good, fast approximation for vector normalization.
    static bool AreColinearFast(Vector p1, Vector p2, Vector p3, float errorMargin = 0.1f)
    {
		Vector p12 = (p1 - p2).FastNormalized(),
			   p13 = (p1 - p3).FastNormalized(),
			   p23 = (p2 - p3).FastNormalized();
        return (WithinError(Mathf::Abs(p12.Dot(p13)), 1.0f, errorMargin) &&
                WithinError(Mathf::Abs(p12.Dot(p23)), 1.0f, errorMargin));
    }


    //A vector class that has "Normalized", "Dot", and "Cross" methods.
	template<class Vector>
    static bool AreCoplanar(Vector p1, Vector p2, Vector p3, Vector p4, float errorMargin = 0.1f)
	{
		Vector p12 = (p1 - p2).Normalized(),
			   p13 = (p1 - p3).Normalized(),
			   p14 = (p1 - p4).Normalized(),
			   crossed1 = (p12.Cross(p13)).Normalized(),
			   crossed2 = (p12.Cross(p14)).Normalized();

        return WithinError(Mathf::Abs(crossed1.Dot(crossed2)), 1.0f, errorMargin);
    }
    //A vector class that has "FastNormalized", "Dot", and "Cross" methods.
    template<class Vector>
    //Like "AreCoplanar", but using a good, fast approximation for vector normalization.
    static bool AreCoplanarFast(Vector p1, Vector p2, Vector p3, Vector p4,
                                float(*dotProduct)(const Vector& v1, const Vector& v2),
                                float errorMargin = 0.1f)
    {
		Vector p12 = (p1 - p2).FastNormalized(),
			   p13 = (p1 - p3).FastNormalized(),
			   p14 = (p1 - p4).FastNormalized(),
			   crossed1 = (p12.Cross(p13)).FastNormalized(),
			   crossed2 = (p12.Cross(p14)).FastNormalized();

        return WithinError(Mathf::Abs(crossed1.Dot(crossed2)), 1.0f, errorMargin);
    }


    //A vector class that has "Distance" and "AngleBetween" methods.
	template<class Vector>
    //Gets the area of the given triangle.
	static float TriangleArea(Vector p1, Vector p2, Vector p3)
	{
		return (0.5f * p1.Distance(p2) * p1.Distance(p3) *
				sinf((p2 - p1).AngleBetween(p3 - p1)));
	}
	
    //A vector class that has "Distance" and "AngleBetween" methods.
	template<class Vector>
    //Interpolates between three vertices of a triangle.
	static float TriangleInterpolate(Vector p1, float val1, Vector p2, float val2,
                                     Vector p3, float val3, Vector interpPos)
	{
		float area1 = TriangleArea(p2, p3, interpPos),
			  area2 = TriangleArea(p1, p3, interpPos),
			  area3 = TriangleArea(p1, p2, interpPos),
			  areaTotal = TriangleArea(p1, p2, p3);

        if (areaTotal == 0.0f)
            return (val1 + val2 + val3) * (1.0f / 3.0f);
	    
        float invAreaTotal = 1.0f / areaTotal,
			  percent1 = area1 * invAreaTotal,
			  percent2 = area2 * invAreaTotal,
			  percent3 = area3 * invAreaTotal;

		return (percent1 * val1) + (percent2 * val2) + (percent3 * val3);
	}


    //A vector class with a "[]" operator to access its individual components.
    template<class Vector>
    //Gets the axis of the given vector with the largest magnitude.
    static unsigned int GetLongestAxis(Vector v)
    {
        unsigned int size = NumbDimensions(v);

        unsigned int currentLargestAxis = 0;
        float currentLargestValue = Mathf::Abs(v[currentLargestAxis]);

        for (unsigned int axis = 1; axis < size; ++axis)
        {
            float tempValue = Mathf::Abs(v[axis]);
            if (tempValue > currentLargestValue)
            {
                currentLargestAxis = axis;
                currentLargestValue = tempValue;
            }
        }

        return largestAxis;
    }
    //Gets the axis of the given vector with the largest magnitude.
    static unsigned int GetLongestAxis(Vector2f v)
    {
        v = v.Abs();
        return (v.x > v.y) ? 0 : 1;
    }
    //Gets the axis of the given vector with the largest magnitude.
    static unsigned int GetLongestAxis(Vector3f v)
    {
        v = v.Abs();
        if (v.x > v.y && v.x > v.z) return 0;
        else if (v.y > v.x && v.y > v.z) return 1;
        else return 2;
    }
    //Gets the axis of the given vector with the largest magnitude.
    static unsigned int GetLongestAxis(Vector4f v)
    {
        v = v.Abs();
        if (v.x > v.y && v.x > v.z && v.x > v.w) return 0;
        else if (v.y > v.x && v.y > v.z && v.y > v.w) return 1;
        else if (v.z > v.x && v.z > v.y && v.z > v.w) return 2;
        else return 3;
    }


    //A vector class that has a "Dot" method and "+"/"-" operators.
	template<class Vector>
	//Given a line/segment and a separate third point,
    //    finds the point on the line/segment closest to the third point.
	static Vector ClosestToLine(Vector l1, Vector l2, Vector separatePoint, bool isLineInfinite)
	{
		Vector l1ToSeparatePoint = separatePoint - l1,
			   l1ToL2 = l2 - l1;
        float ab2 = l1ToL2.Dot(l1ToL2),
              ap_ab = l1ToSeparatePoint.Dot(l1ToL2),
			  t = ap_ab / ab2;
		if (!isLineInfinite)
		{
            if (t < 0.0f)
            {
                t = 0.0f;
            }
            else if (t > 1.0f)
            {
                t = 1.0f;
            }
		}

        Vector closest = l1 + (l1ToL2 * t);
		return closest;
	}


	template<class Vector>
	//The return value for "ClosestToIntersection".
	struct ClosestValues
    {
    public:
        Vector OnFirstLine, OnSecondLine;
        ClosestValues(Vector first, Vector second)
            : OnFirstLine(first), OnSecondLine(second) { }
    };

    //A vector class that has a "Dot" method and "+"/"-" operators.
	template<class Vector>
	//Given two lines/segments, finds the closest point on each line/segment to the other.
	static ClosestValues<Vector> ClosestToIntersection(Vector firstLine_1, Vector firstLine_2,
                                                       Vector secondLine_1, Vector secondLine_2,
                                                       bool areLinesInfinite)
	{
		//TODO: Verify.

		//Googled this algorithm. Only vaguely remember the reasoning behind it,
        //    but it just uses some basic vector math.

		Vector p0 = firstLine_1,
               q0 = secondLine_1,
			   u = firstLine_2 - firstLine_1,
			   v = secondLine_2 - secondLine_1;

		//If the lines are parallel, their distance is constant.
        //Just use the midpoints of the given lines.
        if (u == v || u == -v)
        {
            return ClosestValues<Vector>((p0 + firstLine_2) * 0.5f,
                                         (q0 + secondLine_2) * 0.5f);
        }

		Vector w0 = p0 - q0;
		float a = u.Dot(u),
			  b = u.Dot(v),
			  c = v.Dot(v),
			  d = u.Dot(w0),
			  e = v.Dot(w0);

		float denominator = (a * c) - (b * b);
		float sc = ((b * e) - (c * d)) / denominator,
			  tc = ((a * e) - (b * d)) / denominator;

		if (!areLinesInfinite)
		{
			sc = Mathf::Clamp(sc, 0.0f, 1.0f);
            tc = Mathf::Clamp(tc, 0.0f, 1.0f);
		}

		return ClosestValues<Vector>(p0 + (u * sc), q0 + (v * tc));
	}


	template<class Vector>
    //The return value of "PointOnLineAtValueResult".
	struct PointOnLineAtValueResult
    {
        Vector Point;
        float t;
        PointOnLineAtValueResult(Vector p, float _t)
            : Point(p), t(_t) { }
    };
	template<class Vector>
	//Given a line, an axis, and a target value, gets "p" and "t", where:
	//1) p = vOnLine + (vLineDir * t)
	//2) p[axis] = targetValue
	//For example, this function could be used to find the point with Y value 5.25f on a given line.
	//Assumes that "vLineDir" is not 0 in the given dimension.
	static PointOnLineAtValueResult<Vector> GetPointOnLineAtValue(Vector vOnLine, Vector vLineDir,
                                                                  int axis, float targetValue)
	{
        assert(vLineDir[axis] != 0.0f);
		float t = (targetValue - vOnLine[axis]) / vLineDir[axis];
		return PointOnLineAtValueResult<Vector>(vOnLine + (vLineDir * t), t);
	}



    template<typename VertexType>
    //Calculates normals for a collection of triangles.
    //Takes in a function that finds whether a given normal for a given vertex is facing the wrong way,
    //    as well as getters for a vertex's normal and position.
    static void CalculateNormals(VertexType* vertices, unsigned int nVertices,
                                 const unsigned int* indices, unsigned int nIndices,
                                 Vector3f&(*getNormal)(VertexType& vertex),
                                 Vector3f(*getPos)(const VertexType& vertex),
                                 bool(*shouldFlipNormal)(const Vector3f& normal,
                                                         const VertexType& vert,
                                                         void* pData),
                                 void* extraData = 0)
    {
        //Start all normals at 0.0f.
        for (unsigned int vert = 0; vert < nVertices; ++vert)
            getNormal(vertices[vert]) = Vector3f();

        //Get the normals for each triangle.
        for (unsigned int tri = 0; tri < nIndices; tri += 3)
        {
            VertexType& v1 = vertices[indices[tri]],
                        v2 = vertices[indices[(tri + 1)]],
                        v3 = vertices[indices[(tri + 2)]];
            Vector3f v_1_2 = getPos(v1) - getPos(v2),
                     v_1_3 = getPos(v1) - getPos(v3);
            Vector3f norm = v_1_2.Normalized().Cross(v_1_3.Normalized());

            if (shouldFlipNormal(norm, v1, extraData))
            {
                norm = -norm;
            }

            getNormal(v1) += norm;
            getNormal(v2) += norm;
            getNormal(v3) += norm;
        }

        //Get the average of each vertex's normals.
        for (unsigned int vert = 0; vert < nVertices; ++vert)
            getNormal(vertices[vert]).Normalize();
    }
    template<typename VertexType>
    static void CalculateNormals(VertexType* vertices, unsigned int nVertices,
                                 const unsigned int* indices, unsigned int nIndices,
                                 bool(*shouldFlipNormal)(const Vector3f& pos,
                                                         const VertexType& vert,
                                                         void* pData),
                                 void* extraData = 0)
    {
        CalculateNormals<VertexType>(vertices, nVertices, indices, nIndices,
                                     [](VertexType& vert) -> Vector3f& { return vert.Normal; },
                                     [](const VertexType& vert) -> Vector3f { return vert.Pos; },
                                     shouldFlipNormal, extraData);
    }
};