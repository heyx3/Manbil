#include "ColorGradient.h"

char ColorGradient::Exception_NoStartingNode = 1,
	 ColorGradient::Exception_NoEndingNode = 2,
	 ColorGradient::Exception_NodesNotOrdered = 4;

void ColorGradient::CheckErrors(void) const
{
	if (OrderedNodes[0].Position != 0.0f)
	{
		throw Exception_NoStartingNode;
	}
	if (OrderedNodes[OrderedNodes.size() - 1].Position != 1.0f)
	{
		throw Exception_NoEndingNode;
	}
	
	for (int index = OrderedNodes.size() - 1; index >= 0; --index)
	{
		if (index < OrderedNodes.size() - 1 &&
			OrderedNodes[index].Position >= OrderedNodes[index + 1].Position)
		{
			throw Exception_NodesNotOrdered;
		}
	}
}

void ColorGradient::GetColors(Vector4f * outColors, const float * noiseValues, unsigned int numbElements) const
{
	CheckErrors();

    for (unsigned int i = 0; i < numbElements; ++i)
        outColors[i] = GetColorWOErrorChecking(noiseValues[i]);
}
Vector4f ColorGradient::GetColorWOErrorChecking(float f) const
{
    f = BasicMath::Clamp(f, OrderedNodes[0].Position, OrderedNodes[OrderedNodes.size() - 1].Position);

	//Edge cases. Choose a random node side if the given value sits right on a Node.
	if (f == OrderedNodes[0].Position)
	{
		return ((rand() % 9) < 5) ? OrderedNodes[0].LeftColor : OrderedNodes[0].RightColor;
	}
	if (f == OrderedNodes[OrderedNodes.size() - 1].Position)
	{
		return ((rand() % 9) < 5) ? OrderedNodes[OrderedNodes.size() - 1].LeftColor : OrderedNodes[OrderedNodes.size() - 1].RightColor;
	}

	//Count to the first node above the given position.
	int index;
	for (index = OrderedNodes.size() - 1; OrderedNodes[index].Position > f && index > 0; --index) { }

	//Lerp between the two color values.

	Vector4f leftCol = OrderedNodes[index].RightColor;
	Vector4f rightCol = OrderedNodes[index + 1].LeftColor;
	Interval posInt(OrderedNodes[index].Position, OrderedNodes[index + 1].Position, 0.001f, true, true);
	Vector4b needReversed((unsigned char)(leftCol.x > rightCol.x), leftCol.y > rightCol.y, leftCol.z > rightCol.z, leftCol.w > rightCol.w);
	float ref = posInt.Reflect(f);
	float rx = (needReversed.x) ? ref : f,
		  ry = (needReversed.y) ? ref : f,
		  rz = (needReversed.z) ? ref : f,
		  rw = (needReversed.w) ? ref : f;
	Interval ix(leftCol.x, rightCol.x, 0.001f, true, true),
			 iy(leftCol.y, rightCol.y, 0.001f, true, true),
			 iz(leftCol.z, rightCol.z, 0.001f, true, true),
			 iw(leftCol.w, rightCol.w, 0.001f, true, true);
	rx = posInt.MapValue(ix, rx);
	ry = posInt.MapValue(iy, ry);
	rz = posInt.MapValue(iz, rz);
	rw = posInt.MapValue(iw, rw);
	Vector4f ret(rx, ry, rz, rw);

	return ret;
}