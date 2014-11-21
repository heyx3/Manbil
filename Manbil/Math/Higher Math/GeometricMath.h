#pragma once

#include "../Lower Math/Vectors.h"

//Custom math functions.
class GeometricMath
{
private:
	
#pragma warning(disable: 4100)
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
	
	template<class Vector>
	static bool AreColinear(Vector p1, Vector p2, Vector p3)
	{
		//TODO: Test.
		const int roundDecimals = 2;

		Vector p12 = (p1 - p2).Normalized(),
				 p13 = (p1 - p3).Normalized(),
				 p23 = (p2 - p3).Normalized();
		float roundD_12_13 = BasicMath::Round(p12.Dot(p13), roundDecimals),
			  roundD_12_23 = BasicMath::Round(p12.Dot(p23), roundDecimals);
		return (roundD_12_13 == 1.0f || roundD_12_13 == -1.0f) &&
			   (roundD_12_23 == 1.0f || roundD_12_23 == -1.0f);
		

		/*
		if (BasicMath::Round(p1.x, roundDecimals) == BasicMath::Round(p2.x, roundDecimals) &&
			BasicMath::Round(p2.x, roundDecimals) == BasicMath::Round(p3.x, roundDecimals))
		{
			return true;
		}

		float slope1, slope2;
		slope1 = (p1.y - p2.y) / (p1.x - p2.x);
		slope2 = (p1.y - p3.y) / (p1.x - p3.x);

		return (BasicMath::Round(BasicMath::Abs(slope1), roundDecimals) == BasicMath::Round(BasicMath::Abs(slope2), roundDecimals));
		*/
	}

	template<class Vector>
	static bool AreCoplanar(Vector p1, Vector p2, Vector p3, Vector p4)
	{
		//TODO: Test.
		const int roundDecimals = 2;

		Vector p12 = (p1 - p2).Normalized(),
			   p13 = (p1 - p3).Normalized(),
			   p14 = (p1 - p4).Normalized(),
			   crossed1 = (p12.Cross(p13)).Normalized().Rounded(roundDecimals),
			   crossed2 = (p12.Cross(p14)).Normalized().Rounded(roundDecimals);

		return (crossed1 == crossed2 || crossed1 == -crossed2);
	}



	template<class Vector>
	static float TriangleArea(Vector p1, Vector p2, Vector p3)
	{
		if (p1.Equals(p2) || p2.Equals(p3) || p3.Equals(p1) ||
			AreColinear(p1, p2, p3))
		{
			return 0.0f;
		}
		
		return (0.5f * p1.Distance(p2) * p1.Distance(p3) *
				sinf((p2 - p1).AngleBetween(p3 - p1)));
	}
	
	template<class Vector>
	static float TriangleInterpolate(Vector p1, float val1, Vector p2, float val2, Vector p3, float val3, Vector interpPos)
	{
		if (val1 == val2 && val2 == val3) return val1;

		if (interpPos.Equals(p1)) return val1;
		if (interpPos.Equals(p2)) return val2;
		if (interpPos.Equals(p3)) return val3;

		float area1 = TriangleArea(p2, p3, interpPos),
			  area2 = TriangleArea(p1, p3, interpPos),
			  area3 = TriangleArea(p1, p2, interpPos),
			  areaTotal = TriangleArea(p1, p2, p3),
			  invAreaTotal = 1.0f / areaTotal,

			  percent1 = area1 * invAreaTotal,
			  percent2 = area2 * invAreaTotal,
			  percent3 = area3 * invAreaTotal;

		return (percent1 * val1) + (percent2 * val2) + (percent3 * val3);
	}


    //Gets the largest-magnitude axis of the given vector.
    template<class Vector>
    static unsigned int GetLongestAxis(Vector v)
    {
        unsigned int size = NumbDimensions(v),
                     largestAxis = 0,
                     float largestValue = BasicMath::Abs(v[0]);
        for (unsigned int axis = 1; axis < size; ++axis)
        {
            float tempValue = BasicMath::Abs(v[axis]);
            if (tempValue > largestValue)
            {
                largestAxis = axis;
                largestValue = tempValue;
            }
        }
        return largestAxis;
    }
    template<>
    static unsigned int GetLongestAxis(Vector2f v)
    {
        Vector2f abs(BasicMath::Abs(v.x), BasicMath::Abs(v.y));
        if (abs.x > abs.y) return 0;
        return 1;
    }
    template<>
    static unsigned int GetLongestAxis(Vector3f v)
    {
        Vector3f abs(BasicMath::Abs(v.x), BasicMath::Abs(v.y), BasicMath::Abs(v.z));
        if (abs.x > abs.y && abs.x > abs.z) return 0;
        else if (abs.y > abs.x && abs.y > abs.z) return 1;
        else return 2;
    }
    template<>
    static unsigned int GetLongestAxis(Vector4f v)
    {
        Vector4f abs(BasicMath::Abs(v.x), BasicMath::Abs(v.y), BasicMath::Abs(v.z), BasicMath::Abs(v.w));
        if (abs.x > abs.y && abs.x > abs.z && abs.x > abs.w) return 0;
        else if (abs.y > abs.x && abs.y > abs.z && abs.y > abs.w) return 1;
        else if (abs.z > abs.x && abs.z > abs.y && abs.z > abs.w) return 2;
        else return 3;
    }


	template<class Vector>
	//Given a line/segment and a separate third point, finds the point on the line/segment closest to the third point.
	static Vector ClosestToLine(Vector l1, Vector l2, Vector separatePoint, bool isLineInfinite)
	{
		//TODO: Test.

		Vector AP = separatePoint - l1,
			   AB = l2 - l1;
		float ab2 = AB.Dot(AB),
			  ap_ab = AP.Dot(AB),
			  t = ap_ab / ab2;

		if (!isLineInfinite)
		{
			if (t < 0.0f) t = 0.0f;
			else if (t > 1.0f) t = 1.0f;
		}

		Vector closest = l1 + (AB * t);
		return closest;
	}

	template<class Vector>
	//The return value for "ClosestToIntersection".
	struct ClosestValues { Vector OnFirstLine, OnSecondLine; ClosestValues(Vector first, Vector second) : OnFirstLine(first), OnSecondLine(second) { } };
	template<class Vector>
	//Given two lines/segments, finds the closest points on each line/segment to the other.
	static ClosestValues<Vector> ClosestToIntersection(Vector firstLine_1, Vector firstLine_2, Vector secondLine_1, Vector secondLine_2, bool areLinesInfinite)
	{
		//TODO: Test.

		//Googled this algorithm. Only vaguely remember the reasoning behind it, but it just uses some basic vector math.

		Vector p0 = firstLine_1, q0 = secondLine_1,
			   u = (firstLine_2 - firstLine_1),
			   v = (secondLine_2 - secondLine_1);

		//If the lines are parallel, their distance is constant. Just use the midpoints of the given lines.
		if (u == v || u == -v) return ClosestValues<Vector>((p0 + firstLine_2) * 0.5f, (q0 + secondLine_2) * 0.5f);

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
			sc = BasicMath::Clamp(sc, 0.0f, 1.0f);
            tc = BasicMath::Clamp(tc, 0.0f, 1.0f);
		}

		return ClosestValues<Vector>(p0 + (u * sc), q0 + (v * tc));
	}



	template<class Vector>
	struct ClosestToPlaneData { float Distance; Vector PointOnPlane; ClosestToPlaneData(float dist, Vector onPlane) : Distance(dist), PointOnPlane(onPlane) { } };
	template<class Vector>
	//Gets the closest point on a given infinite plane to the given point.
	static ClosestToPlaneData<Vector> ClosestToPlane(Vector plane1, Vector plane2, Vector plane3, Vector outsidePoint)
	{
		//TODO: Test.

		if (AreCoplanar(plane1, plane2, plane3, outsidePoint))
		{
			return ClosestToPlaneData<Vector>(0.0f, outsidePoint);
		}
		
		ClosestToPlaneData<Vector> ret(0.0f, plane1);

		Vector perp = (plane1 - plane2).Cross(plane1 - plane3).Normalized();
		ret.Distance = perp.Dot(plane1 - outsidePoint);
		if (ret.Distance < 0.0f)
		{
			perp = -perp;
			ret.Distance = -ret.Distance;
		}

		ret.PointOnPlane = outsidePoint + (-perp * ret.Distance);

		return ret;
	}
	//Gets the closest point on a given closed square plane to the given point.
	template<class Vector>
	static ClosestToPlaneData<Vector> ClosestToPlane(Vector plane1, Vector plane2, Vector plane3, Vector plane4, Vector outsidePoint)
	{
		ClosestToPlaneData<Vector> initial = ClosestToPlane(plane1, plane2, plane3, outsidePoint);

		int dimensions = NumbDimensions(plane1);

		//See if the point is inside the plane's boundaries.
		Vector v0, v1, v2, v3;
		v0 = plane1 - outsidePoint;
		v1 = plane2 - outsidePoint;
		v2 = plane3 - outsidePoint;
		v3 = plane4 - outsidePoint;

		if (v0.Dot(v1) < 0.0f)
		{
			initial.PointOnPlane = ClosestToLine(plane1, plane2, initial.PointOnPlane, false);
			v0 = plane1 - initial.PointOnPlane;
			v1 = plane2 - initial.PointOnPlane;
			v2 = plane3 - initial.PointOnPlane;
			v3 = plane4 - initial.PointOnPlane;
		}
		if (v1.Dot(v2) < 0.0f)
		{
			initial.PointOnPlane = ClosestToLine(plane2, plane3, initial.PointOnPlane, false);
			v0 = plane1 - initial.PointOnPlane;
			v2 = plane3 - initial.PointOnPlane;
			v3 = plane4 - initial.PointOnPlane;
		}
		if (v2.Dot(v3) < 0.0f)
		{
			initial.PointOnPlane = ClosestToLine(plane3, plane4, initial.PointOnPlane, false);
			v0 = plane1 - initial.PointOnPlane;
			v3 = plane4 - initial.PointOnPlane;
		}
		if (v3.Dot(v0) < 0.0f)
		{
			initial.PointOnPlane = ClosestToLine(plane4, plane1, initial.PointOnPlane, false);
		}

		return initial;
	}

	template<class Vector>
	struct LineClosestToPlaneData { float Distance; Vector PointOnPlane, PointOnLine; LineClosestToPlaneData(Vector pOnPlane, Vector pOnLine, float dist) : Distance(dist), PointOnPlane(pOnPlane), PointOnLine(pOnLine) { } };
	template<class Vector>
	//Given a line/segment and an infinite plane, finds the spot on the line closest to the plane and the spot on the plane closest to the line.
	static LineClosestToPlaneData<Vector> LineClosestToPlane(Vector plane1, Vector plane2, Vector plane3, Vector line1, Vector line2, bool isLineInfinite)
	{

	}
	template<class Vector>
	//Given a line/segment and a closed square plane, finds the spot on the line closest to the plane and the spot on the plane closest to the line.
	static LineClosestToPlaneData<Vector> LineClosestToPlane(Vector plane1, Vector plane2, Vector plane3, Vector plane4, Vector line1, Vector line2, bool isLineInfinite)
	{

	}

	template<class Vector>
	struct PointOnLineAtValueResult { Vector Point; float t; PointOnLineAtValueResult(Vector p, float _t) : Point(p), t(_t) { } };
	template<class Vector>
	//Given a line, a target dimension and a target value, gets "p" and "t", where:
	//1) p = vOnLine + (vLineDir * t)
	//2) p[dimension] = targetValue
	//For example, this function could be used to find the point with Y value 5.25f for some line.
	//0 = X axis, 1 = Y axis, 2 = Z axis, 3 = W axis.
	//Assumes that "vLineDir" is not 0 in the given dimension.
	static PointOnLineAtValueResult<Vector> GetPointOnLineAtValue(Vector vOnLine, Vector vLineDir, int dimension, float targetValue)
	{
		float t = (targetValue - vOnLine[dimension]) / vLineDir[dimension];
		return PointOnLineAtValueResult<Vector>(vOnLine + (vLineDir * t), t);
	}

    //Calculates normals for a list of triangles.
    //Takes in four functions: the first gets the given vertex's normal.
    //The second sets the given vertex's normal to the given value.
    //The third gets the given vertex's position.
    //The fourth calculates whether the given normal should be flipped.
    //Takes in a function that can set the normal of the given vertex to the given value.
    static void CalculateNormals(void* vertices, unsigned int sizeofVertices, unsigned int nVertices,
                                 const unsigned int * indices, unsigned int nIndices,
                                 Vector3f(*getNormal)(const void* vertex),
                                 void(*setNormal)(void* vertex, Vector3f normal),
                                 Vector3f(*getPos)(const void* vertex),
                                 bool(*shouldFlipNormal)(Vector3f normal, void* vertex, void* extraData), void* extraData = 0);

    template<typename VertexType>
    //Calculates normals for a list of triangles.
    //The vertex must have Vector3f fields named "Pos" and "Normal".
    static void CalculateNormals(VertexType * vertices, unsigned int nVertices,
                                 const unsigned int * indices, unsigned int nIndices,
                                 bool (*shouldFlipNormal)(Vector3f normal, void* vertex, void* extraData), void* extraData = 0)
    {
        CalculateNormals(vertices, sizeof(VertexType), nVertices, indices, nIndices,
                         [](const void* pV) { return ((VertexType*)pV)->Normal; },
                         [](void* pV, Vector3f norm) { ((VertexType*)pV)->Normal = norm; },
                         [](const void* pV) { return ((VertexType*)pV)->Pos; },
                         shouldFlipNormal, extraData);
    }
};