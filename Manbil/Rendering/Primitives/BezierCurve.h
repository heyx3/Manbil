#pragma once

#include "../Materials/Data Nodes/DataNode.h"
#include "CurveVertex.h"


//Generates and positions a Bezier curve for the GPU OR CPU.
//This class expects the vertex data to have a Vector2f describing its position in the line:
//   the X component describing (from 0 to 1) where along the line it lies, and
//   the Y component describing (from -1 to 1) where along the line's perpendicular it lies.
//This class is a DataNode; it takes as input the position of the start/end points and control points,
//   as well as the normal, and outputs:
// 0 -- The world position of the vertex (vec3).
// 1 -- The line value at this vertex (vec3).
//Output 1 is offset from output 2 along the perpendicular of the line.
class BezierCurve : public DataNode
{
public:

    struct BezierPointData
    {
    public:
        Vector3f Pos, Perp;
        BezierPointData(Vector3f pos = Vector3f(), Vector3f perp = Vector3f())
            : Pos(pos), Perp(perp) { }
    };


    //Calculates the positions and perpendicular vectors for all the given t values.
    static void CalculatePoints(Vector3f startP, Vector3f endP, Vector3f startSlope, Vector3f endSlope,
                                Vector3f norm, const std::vector<float>& tValues,
                                std::vector<BezierPointData>& outData);
    //Calculates the positions for all the given t values.
    static void CalculatePoints(Vector3f startP, Vector3f endP, Vector3f startSlope, Vector3f endSlope,
                                const std::vector<float>& tValues, std::vector<Vector3f>& outData);

    
    //Which vertex attribute corresponds to the Vector2f describing the vertex's position in the line?
    //For example, if using vertices of type "CurveVertex", this value should be 0.
    unsigned int LinePosLerpIndex;


    //"lineSurfaceNormal" is used to determine which direction to thicken the line in.
    BezierCurve(DataLine startPos, DataLine endPos, DataLine startSlope, DataLine endSlope,
                DataLine lineSurfaceNormal, DataLine lineThickness,
                unsigned int linePosLerpInputIndex = 0, std::string name = "");


    virtual unsigned int GetNumbOutputs(void) const override { return 2; }

    virtual std::string GetOutputName(unsigned int outputIndex) const override;

#pragma warning(disable: 4100)
    virtual unsigned int GetOutputSize(unsigned int outputIndex) const override { return 3; }
#pragma warning(default: 4100)


    const DataLine& GetInput_StartPos(void) const { return GetInputs()[0]; }
    const DataLine& GetInput_EndPos(void) const { return GetInputs()[1]; }
    const DataLine& GetInput_StartSlope(void) const { return GetInputs()[2]; }
    const DataLine& GetInput_EndSlope(void) const { return GetInputs()[3]; }
    const DataLine& GetInput_LineSurfaceNormal(void) const { return GetInputs()[4]; }
    const DataLine& GetInput_LineThickness(void) const { return GetInputs()[5]; }


protected:

    virtual void AssertMyInputsValid(void) const override;

    virtual void GetMyFunctionDeclarations(std::vector<std::string>& outDecls) const override;
    virtual void WriteMyOutputs(std::string& outCode) const override;

    virtual void WriteExtraData(DataWriter* writer) const override;
    virtual void ReadExtraData(DataReader* reader) override;

    virtual std::string GetInputDescription(unsigned int index) const override;


private:

    static std::vector<DataLine> MakeVector(DataLine sp, DataLine ep, DataLine ss,
                                            DataLine es, DataLine norm, DataLine thickness);


    ADD_NODE_REFLECTION_DATA_H(BezierCurve)
};