#pragma once

#include "../Materials/Data Nodes/DataNode.h"
#include "CurveVertex.h"


//Generates and positions a Bezier curve. Uses vertices of type "CurveVertex".
//This class is a DataNode and uses the "LinePosLerp" vertex input
//   as well as four vec3 uniforms that describe the spline.
//It has two outputs:
//1) The position of this vertex (vec3).
//2) The line value at this vertex (vec3).
//Output 1 is offset from output 2 along the perpendicular of the line.
class BezierCurve : public DataNode
{
public:

    struct BezierPointData { public: Vector3f Pos, Perp; };
    //Calculates the positions and perpendicular vectors for all the given t values.
    static void CalculatePoints(Vector3f startP, Vector3f endP, Vector3f startSlope, Vector3f endSlope,
                                Vector3f norm, const std::vector<float> & tValues,
                                std::vector<BezierPointData> & outData);
    //Calculates the positions for all the given t values.
    static void CalculatePoints(Vector3f startP, Vector3f endP, Vector3f startSlope, Vector3f endSlope,
                                const std::vector<float> & tValues, std::vector<Vector3f> & outData);


    


    //The vertex input index for the "LinePosLerp" value.
    //If the vertices are BezierVertex instances, this value should be 0.
    unsigned int LinePosLerpIndex;


    //"lineSurfaceNormal" is used to determine which direction to thicken the line in.
    BezierCurve(DataLine startPos, DataLine endPos, DataLine startSlope, DataLine endSlope,
                   DataLine lineSurfaceNormal, DataLine lineThickness,
                   unsigned int linePosLerpInputIndex = 0, std::string name = "")
        : LinePosLerpIndex(linePosLerpInputIndex),
          DataNode(MakeVector(startPos, endPos, startSlope, endSlope, lineSurfaceNormal, lineThickness), name) { }

    virtual unsigned int GetNumbOutputs(void) const override { return 2; }

    virtual std::string GetOutputName(unsigned int outputIndex) const override;
    virtual unsigned int GetOutputSize(unsigned int outputIndex) const override { return 3; }


    const DataLine & GetInput_StartPos(void) const { return GetInputs()[0]; }
    const DataLine & GetInput_EndPos(void) const { return GetInputs()[1]; }
    const DataLine & GetInput_StartSlope(void) const { return GetInputs()[2]; }
    const DataLine & GetInput_EndSlope(void) const { return GetInputs()[3]; }
    const DataLine & GetInput_LineSurfaceNormal(void) const { return GetInputs()[4]; }
    const DataLine & GetInput_LineThickness(void) const { return GetInputs()[5]; }


protected:

    virtual void AssertMyInputsValid(void) const override;

    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const override;
    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual bool WriteExtraData(DataWriter * writer, std::string & outError) const override;
    virtual bool ReadExtraData(DataReader * reader, std::string & outError) override;

    virtual std::string GetInputDescription(unsigned int index) const override;


private:

    static std::vector<DataLine> MakeVector(DataLine sp, DataLine ep, DataLine ss, DataLine es, DataLine norm, DataLine thickness);

    MAKE_NODE_READABLE_H(BezierCurve)
};