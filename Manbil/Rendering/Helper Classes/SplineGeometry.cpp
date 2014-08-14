#include "SplineGeometry.h"

#include "../Materials/Data Nodes/DataNodeIncludes.h"



MAKE_NODE_READABLE_CPP(SplineGeometry, Vector3f(), Vector3f(), Vector3f(), Vector3f(), Vector3f(), 1.0f);



void SplineGeometry::GenerateSplineVertices(std::vector<SplineVertex> & outVerts, unsigned int lineSegments)
{
    assert(lineSegments > 1);

    float increment = 1.0f / (float)(lineSegments - 1);
    for (unsigned int seg = 0; seg < lineSegments; ++seg)
    {
        float x = (float)seg * increment;
        outVerts.insert(outVerts.end(), SplineVertex(Vector2f(x, -1.0f)));
        outVerts.insert(outVerts.end(), SplineVertex(Vector2f(x, 1.0f)));
    }
}

std::string SplineGeometry::GetOutputName(unsigned int index) const
{
    return GetName() + "_" + (index == 0 ? "linePos" : "vertPos");
}

void SplineGeometry::AssertMyInputsValid(void) const
{
    Assert(GetInput_StartPos().GetSize() == 3, "'Start Pos' input must be size 3!");
    Assert(GetInput_EndPos().GetSize() == 3, "'End Pos' input must be size 3!");
    Assert(GetInput_StartSlope().GetSize() == 3, "'Start Slope' input must be size 3!");
    Assert(GetInput_EndSlope().GetSize() == 3, "'End Slope' input must be size 3!");
    Assert(GetInput_LineSurfaceNormal().GetSize() == 3, "'Line Surface Normal' input must be size 3!");
    Assert(GetInput_LineThickness().GetSize() == 1, "'Line Thickness' input must be size 1!");
}
void SplineGeometry::GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const
{
    std::string decl;
    const std::string structDecl = "struct SplinePos { vec3 Point, Perp; };";

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
    outDecls.insert(outDecls.end(), std::string() +
"SplinePos " + GetName() + "_interpSpline(vec3 sp, vec3 ep, vec3 ss, vec3 es, vec3 norm, float t) \n\
{                                                                                                 \n\
    SplinePos ret;                                                                                \n\
    float oneMinusT = 1.0f - t;                                                                   \n\
    float oneMinusTSqr = oneMinusT * oneMinusT;                                                   \n\
    float tSqr = t * t;                                                                           \n\
    ret.Pos = (oneMinusTSqr * oneMinusT * sp) + (3.0f * oneMinusTSqr * t * ss) +                  \n\
              (3.0f * oneMinusT * tSqr * es) + (tSqr * t * ep);                                   \n\
                                                                                                  \n\
    vec3 toNext = (3.0f * oneMinusTSqr * (ss - sp)) + (6.0f * oneMinusT * t * (es - ss)) +        \n\
                  (3.0f * tSqr * (ep - es));                                                      \n\
    ret.Perp = cross(normalize(toNext), norm);                                                    \n\
    return ret;                                                                                   \n\
}\n\n");
}
void SplineGeometry::WriteMyOutputs(std::string & outCode) const
{
    Assert(CurrentShader == ShaderHandler::SH_Vertex_Shader,
           "SplineGeometry nodes only work in vertex shaders, not '" +
               ToString(CurrentShader) + "'!");
    std::string temp = GetName() + "_splinePos";
    outCode += "SplinePos " + temp + " = " + GetName() + "_interpSpline(" +
                    GetInput_StartPos().GetValue() + ", " + GetInput_EndPos().GetValue() + ", " +
                    GetInput_StartSlope().GetValue() + ", " + GetInput_EndSlope().GetValue() + ", " +
                    GetInput_LineSurfaceNormal().GetValue() + ", " +
                    VertexIns.GetAttributeName(LinePosLerpIndex) + ");\n";
    outCode += "\tvec3 " + GetOutputName(0) + " = " + temp + ".Pos;\n";
    outCode += "\tvec3 " + GetOutputName(1) + " = " + temp + ".Pos + (" + temp + ".Perp * " + GetInput_LineThickness().GetValue() + ");\n";
}

bool SplineGeometry::WriteExtraData(DataWriter * writer, std::string & outError) const
{
    if (!writer->WriteUInt(LinePosLerpIndex, "'Line Pos Lerp' vertex input index", outError))
    {
        outError = "Error writing 'line pos lerp' input index value (" + ToString(LinePosLerpIndex) + "): " + outError;
        return false;
    }

    return true;
}
bool SplineGeometry::ReadExtraData(DataReader * reader, std::string & outError)
{
    MaybeValue<unsigned int> tryInd = reader->ReadUInt(outError);
    if (!tryInd.HasValue())
    {
        outError = "Error reading 'line pos lerp' input index value: " + outError;
        return false;
    }

    return true;
}

std::string SplineGeometry::GetInputDescription(unsigned int index) const
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

std::vector<DataLine> SplineGeometry::MakeVector(DataLine sp, DataLine ep, DataLine ss, DataLine es, DataLine norm, DataLine thick)
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