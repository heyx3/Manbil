#include "BezierCurve.h"

#include "../Materials/Data Nodes/DataNodeIncludes.h"



MAKE_NODE_READABLE_CPP(BezierCurve, Vector3f(), Vector3f(), Vector3f(), Vector3f(), Vector3f(), 1.0f);


//TODO: Thread these two functions.
void BezierCurve::CalculatePoints(Vector3f startP, Vector3f endP, Vector3f startSlope, Vector3f endSlope,
                                  Vector3f norm, const std::vector<float> & tValues,
                                  std::vector<BezierPointData> & outData)
{
    Vector3f ss_sp = startSlope - startP,
             es_ss = endSlope - startSlope,
             ep_es = endP - endSlope;

    outData.resize(tValues.size());
    for (unsigned int i = 0; i < tValues.size(); ++i)
    {
        float t = tValues[i],
              tSqr = t * t,
              oneMinusT = 1.0f - t,
              oneMinusTSqr = oneMinusT * oneMinusT;
        outData[i].Pos = (startP * (oneMinusTSqr * oneMinusT)) +
                         (startSlope * (3.0f * oneMinusTSqr * t)) +
                         (endSlope * (3.0f * oneMinusT * tSqr)) +
                         (endP * (tSqr * t));
        Vector3f toNext = (ss_sp * (3.0f * oneMinusTSqr)) +
                          (es_ss * (6.0f * oneMinusT * t)) +
                          (ep_es * (3.0f * tSqr));
        outData[i].Perp = norm.Cross(toNext.Normalized());
    }
}
void BezierCurve::CalculatePoints(Vector3f startP, Vector3f endP, Vector3f startSlope, Vector3f endSlope,
                                  const std::vector<float> & tValues, std::vector<Vector3f> & outData)
{
    outData.resize(tValues.size());
    for (unsigned int i = 0; i < tValues.size(); ++i)
    {
        float t = tValues[i],
              tSqr = t * t,
              oneMinusT = 1.0f - t,
              oneMinusTSqr = oneMinusT * oneMinusT;
        outData[i] = (startP * (oneMinusTSqr * oneMinusT)) +
                     (startSlope * (3.0f * oneMinusTSqr * t)) +
                     (endSlope * (3.0f * oneMinusT * tSqr)) +
                     (endP * (tSqr * t));
    }
}


void BezierCurve::GenerateSplineVertices(std::vector<BezierVertex> & outVerts, unsigned int lineSegments)
{
    assert(lineSegments > 1);

    float increment = 1.0f / (float)(lineSegments - 1);
    for (unsigned int seg = 0; seg < lineSegments; ++seg)
    {
        float x = (float)seg * increment;
        outVerts.insert(outVerts.end(), BezierVertex(Vector2f(x, -1.0f)));
        outVerts.insert(outVerts.end(), BezierVertex(Vector2f(x, 1.0f)));
    }
}

std::string BezierCurve::GetOutputName(unsigned int index) const
{
    return GetName() + "_" + (index == 0 ? "vertPos" : "linePos");
}

void BezierCurve::AssertMyInputsValid(void) const
{
    Assert(GetInput_StartPos().GetSize() == 3, "'Start Pos' input must be size 3!");
    Assert(GetInput_EndPos().GetSize() == 3, "'End Pos' input must be size 3!");
    Assert(GetInput_StartSlope().GetSize() == 3, "'Start Slope' input must be size 3!");
    Assert(GetInput_EndSlope().GetSize() == 3, "'End Slope' input must be size 3!");
    Assert(GetInput_LineSurfaceNormal().GetSize() == 3, "'Line Surface Normal' input must be size 3!");
    Assert(GetInput_LineThickness().GetSize() == 1, "'Line Thickness' input must be size 1!");
}
void BezierCurve::GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const
{
    std::string decl;
    const std::string structDecl = "struct SplinePos { vec3 Pos, Perp; };\n";

    bool foundDecl = false;
    for (unsigned int i = 0; i < outDecls.size(); ++i)
    {
        if (outDecls[i].substr(0, structDecl.size()).compare(structDecl) == 0)
        {
            foundDecl = true;
            break;
        }
    }
    if (!foundDecl)
    {
        decl += structDecl;
    }
    outDecls.insert(outDecls.end(), decl +
"SplinePos " + GetName() + "_interpSpline(vec3 sp, vec3 ep, vec3 ss, vec3 es, vec3 norm, float t) \n\
{                                                                                                 \n\
    SplinePos ret;                                                                                \n\
                                                                                                  \n\
    float oneMinusT = 1.0f - t;                                                                   \n\
    float oneMinusTSqr = oneMinusT * oneMinusT;                                                   \n\
    float tSqr = t * t;                                                                           \n\
    ret.Pos = (oneMinusTSqr * oneMinusT * sp) + (3.0f * oneMinusTSqr * t * ss) +                  \n\
              (3.0f * oneMinusT * tSqr * es) + (tSqr * t * ep);                                   \n\
                                                                                                  \n\
    vec3 toNext = (3.0f * oneMinusTSqr * (ss - sp)) + (6.0f * oneMinusT * t * (es - ss)) +        \n\
                  (3.0f * tSqr * (ep - es));                                                      \n\
    ret.Perp = cross(normalize(toNext), norm);                                                    \n\
                                                                                                  \n\
    return ret;                                                                                   \n\
}\n\n");
}
void BezierCurve::WriteMyOutputs(std::string & outCode) const
{
    Assert(CurrentShader == ShaderHandler::SH_Vertex_Shader,
           "BezierCurve nodes only work in vertex shaders, not '" +
               ToString(CurrentShader) + "'!");
    std::string temp = GetName() + "_splinePos";
    outCode += "SplinePos " + temp + " = " + GetName() + "_interpSpline(" +
                    GetInput_StartPos().GetValue() + ", " + GetInput_EndPos().GetValue() + ",\n\t\t" +
                    GetInput_StartSlope().GetValue() + ", " + GetInput_EndSlope().GetValue() + ",\n\t\t" +
                    GetInput_LineSurfaceNormal().GetValue() + ", " +
                    VertexIns.GetAttributeName(LinePosLerpIndex) + ".x);\n";
    outCode += "\tvec3 " + GetOutputName(1) + " = " + temp + ".Pos;\n";
    outCode += "\tvec3 " + GetOutputName(0) + " = " + temp + ".Pos + " +
                  "(" + temp + ".Perp * " + GetInput_LineThickness().GetValue() + " * " +
                        VertexIns.GetAttributeName(LinePosLerpIndex) + ".y);\n";
}

bool BezierCurve::WriteExtraData(DataWriter * writer, std::string & outError) const
{
    if (!writer->WriteUInt(LinePosLerpIndex, "'Line Pos Lerp' vertex input index", outError))
    {
        outError = "Error writing 'line pos lerp' input index value (" + ToString(LinePosLerpIndex) + "): " + outError;
        return false;
    }

    return true;
}
bool BezierCurve::ReadExtraData(DataReader * reader, std::string & outError)
{
    MaybeValue<unsigned int> tryInd = reader->ReadUInt(outError);
    if (!tryInd.HasValue())
    {
        outError = "Error reading 'line pos lerp' input index value: " + outError;
        return false;
    }

    return true;
}

std::string BezierCurve::GetInputDescription(unsigned int index) const
{
    switch (index)
    {
        case 0: return "Start Pos";
        case 1: return "End Pos";
        case 2: return "Start Slope Point";
        case 3: return "End Slope Point";
        case 4: return "Line Surface Normal";
        case 5: return "Line Thickness";
        default:
            Assert(false, "waaaaaaaaaaaat!!!???");
            return "=>nobody_should_EVER_see_this<=";
    }
}

std::vector<DataLine> BezierCurve::MakeVector(DataLine sp, DataLine ep, DataLine ss, DataLine es, DataLine norm, DataLine thick)
{
    std::vector<DataLine> ret;
    ret.insert(ret.end(), sp);
    ret.insert(ret.end(), ep);
    ret.insert(ret.end(), ss);
    ret.insert(ret.end(), es);
    ret.insert(ret.end(), norm);
    ret.insert(ret.end(), thick);
    return ret;
}