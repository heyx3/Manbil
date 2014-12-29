#include "SpaceConverterNode.h"

#include "../../MaterialData.h"


MAKE_NODE_READABLE_CPP(SpaceConverterNode, Vector3f(1.0f, 0.0f, 0.0f), ST_OBJECT, ST_SCREEN, DT_NORMAL)

unsigned int SpaceConverterNode::GetNumbOutputs(void) const
{
    switch (DataType)
    {
        case DT_NORMAL: return 1;
        case DT_POSITION: return 2;
        default:
            Assert(false, "Unknown coordinate data type '" + ToString(DataType));
            return 0;
    }
}

unsigned int SpaceConverterNode::GetOutputSize(unsigned int index) const
{
    return (index == 0 ? 3 : 4);
}
std::string SpaceConverterNode::GetOutputName(unsigned int index) const
{
    std::string nme = GetName() + "_" + ST_ToString(DestSpace) + DT_ToString(DataType);
    if (index == 1) nme += "_Homog";

    return nme;
}


SpaceConverterNode::SpaceConverterNode(const DataLine & inV, SpaceTypes src, SpaceTypes dest, DataTypes dat, std::string name)
    : DataNode(MakeVector(inV), name),
      SrcSpace(src), DestSpace(dest), DataType(dat)
{
    Assert(AssertValidSrcDestSpaces(), errorMsg);
}

#pragma warning(disable: 4100)
void SpaceConverterNode::SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const
{
    Assert(AssertValidSrcDestSpaces(), errorMsg);

    switch (SrcSpace)
    {
        case ST_OBJECT:
            switch (DestSpace)
            {
                case ST_WORLD:
                    flags.EnableFlag(MaterialUsageFlags::DNF_USES_WORLD_MAT);
                    break;
                case ST_VIEW:
                    //TODO: Make "WorldView" matrix.
                    flags.EnableFlag(MaterialUsageFlags::DNF_USES_WORLD_MAT);
                    flags.EnableFlag(MaterialUsageFlags::DNF_USES_VIEW_MAT);
                    break;
                case ST_SCREEN:
                    flags.EnableFlag(MaterialUsageFlags::DNF_USES_WVP_MAT);
                    break;
                default: Assert(false, "Invalid dest space " + ToString(DestSpace));
            }
            break;

        case ST_WORLD:
            switch (DestSpace)
            {
                case ST_VIEW:
                    flags.EnableFlag(MaterialUsageFlags::DNF_USES_VIEW_MAT);
                    break;
                case ST_SCREEN:
                    flags.EnableFlag(MaterialUsageFlags::DNF_USES_VIEWPROJ_MAT);
                    break;
                default: Assert(false, "Invalid dest space " + ToString(DestSpace));
            }
            break;

        case ST_VIEW:
            //Only valid option is converting from view to screen.
            flags.EnableFlag(MaterialUsageFlags::DNF_USES_PROJ_MAT);
            break;

        default: Assert(false, "Invalid src space " + ToString(SrcSpace));
    }
}
#pragma warning(default: 4100)

void SpaceConverterNode::WriteMyOutputs(std::string & outCode) const
{
    Assert(AssertValidSrcDestSpaces(), errorMsg);

    std::string matName;

    switch (SrcSpace)
    {
        case ST_OBJECT:
            switch (DestSpace)
            {
                case ST_WORLD:
                    matName = MaterialConstants::WorldMatName;
                    break;
                case ST_VIEW:
                    //TODO: After adding a "WorldView" matrix, update the following line to use it.
                    matName = "(" + MaterialConstants::ViewMatName + " * " + MaterialConstants::WorldMatName + ")";
                    break;
                case ST_SCREEN:
                    matName = MaterialConstants::WVPMatName;
                    break;
                default: Assert(false, "Invalid dest space " + ToString(DestSpace));
            }
            break;

        case ST_WORLD:
            switch (DestSpace)
            {
                case ST_VIEW:
                    matName = MaterialConstants::ViewMatName;
                    break;
                case ST_SCREEN:
                    matName = MaterialConstants::ViewProjMatName;
                    break;
                default: Assert(false, "Invalid dest space " + ToString(DestSpace));
            }
            break;

        case ST_VIEW:
            //Only valid option is converting from view to screen.
            matName = MaterialConstants::ProjMatName;
            break;

        default: Assert(false, "Invalid src space " + ToString(SrcSpace));
    }

    switch (DataType)
    {
        case DT_NORMAL:
            outCode += "\tvec3 " + GetOutputName(0) + " = normalize((" + matName + " * vec4(" + GetInputs()[0].GetValue() + ", 0.0f)).xyz);\n";
            break;
        case DT_POSITION:
            outCode += "\tvec4 " + GetOutputName(1) + " = (" + matName + " * vec4(" + GetInputs()[0].GetValue() + ", 1.0f));\n";
            outCode += "\tvec3 " + GetOutputName(0) + " = " + GetOutputName(1) + ".xyz / " + GetOutputName(1) + ".w;\n";
            break;
        default: Assert(false, "Invalid data type " + ToString(DataType));
    }
}

std::string SpaceConverterNode::GetInputDescription(unsigned int index) const
{
    switch (DataType)
    {
        case DT_NORMAL:
            return "Source Normal";
        case DT_POSITION:
            return "Source Position";
        default:
            Assert(false, "Unknown data type " + ToString(DataType));
            return "UNKNOWN_DATA_TYPE";
    }
}

bool SpaceConverterNode::WriteExtraData(DataWriter * writer, std::string & outError) const
{
    if (!writer->WriteString(DT_ToString(DataType), "DataType (Normal or Pos)", outError))
    {
        outError = "Error writing out the data type '" + DT_ToString(DataType) + ": " + outError;
        return false;
    }
    if (!writer->WriteString(ST_ToString(SrcSpace), "Source space (Obj, World, or View)", outError))
    {
        outError = "Error writing out the source space '" + ST_ToString(SrcSpace) + ": " + outError;
        return false;
    }
    if (!writer->WriteString(ST_ToString(DestSpace), "Destination space (World, View, or Screen)", outError))
    {
        outError = "Error writing out the dest space '" + ST_ToString(DestSpace) + ": " + outError;
        return false;
    }

    return true;
}
bool SpaceConverterNode::ReadExtraData(DataReader * reader, std::string & outError)
{
    MaybeValue<std::string> tryStr;

    tryStr = reader->ReadString(outError);
    if (!tryStr.HasValue())
    {
        outError = "Error reading in the data type: " + outError;
        return false;
    }
    if (!DT_FromString(tryStr.GetValue(), DataType))
    {
        outError = "Unknown coordinate data type '" + tryStr.GetValue() + "'";
        return false;
    }

    tryStr = reader->ReadString(outError);
    if (!tryStr.HasValue())
    {
        outError = "Error reading in the src space: " + outError;
        return false;
    }
    if (!ST_FromString(tryStr.GetValue(), SrcSpace))
    {
        outError = "Unknown src space '" + tryStr.GetValue() + "'";
        return false;
    }

    tryStr = reader->ReadString(outError);
    if (!tryStr.HasValue())
    {
        outError = "Error reading in the dest space: " + outError;
        return false;
    }
    if (!ST_FromString(tryStr.GetValue(), DestSpace))
    {
        outError = "Unknown dest space '" + tryStr.GetValue() + "'";
        return false;
    }

    return AssertValidSrcDestSpaces();
}


bool SpaceConverterNode::AssertValidSrcDestSpaces(void) const
{
    switch (SrcSpace)
    {
        case ST_OBJECT:
            if (DestSpace == ST_OBJECT)
            {
                errorMsg = "SrcSpace is 'object', but dest space is also 'object'!";
                return false;
            }
            else return true;

        case ST_WORLD:
            if (DestSpace != ST_SCREEN && DestSpace != ST_VIEW)
            {
                errorMsg = "SrcSpace is 'world', but dest space isn't 'view' or 'screen'!";
                return false;
            }
            else return true;

        case ST_VIEW:
            if (DestSpace != ST_SCREEN)
            {
                errorMsg = "SrcSpace is 'view', but dest space isn't 'screen'!";
                return false;
            }
            else return true;

        case ST_SCREEN:
            errorMsg = "SrcSpace cannot be 'screen'!";
            return false;

        default:
            errorMsg = "Unknown src space " + ToString(SrcSpace);
            return false;
    }
}

std::string SpaceConverterNode::ST_ToString(SpaceTypes st) const
{
    switch (st)
    {
        case ST_OBJECT: return "Obj";
        case ST_WORLD: return "World";
        case ST_VIEW: return "View";
        case ST_SCREEN: return "Screen";
        default:
            Assert(false, "Unknown space type " + ToString(st));
            return "UNKNOWN_SPACE_TYPE";
    }
}
std::string SpaceConverterNode::DT_ToString(DataTypes dt) const
{
    switch (dt)
    {
        case DT_NORMAL: return "Normal";
        case DT_POSITION: return "Pos";
        default:
            Assert(false, "Unknown data type " + ToString(dt));
            return "UNKNOWN_DATA_TYPE";
    }
}

bool SpaceConverterNode::ST_FromString(std::string str, SpaceTypes & outSt)
{
    if (str.compare("Obj") == 0)
    {
        outSt = ST_OBJECT;
        return true;
    }
    else if (str.compare("World") == 0)
    {
        outSt = ST_WORLD;
        return true;
    }
    else if (str.compare("View") == 0)
    {
        outSt = ST_VIEW;
        return true;
    }
    else if (str.compare("Screen") == 0)
    {
        outSt = ST_SCREEN;
        return true;
    }
    return false;
}
bool SpaceConverterNode::DT_FromString(std::string str, DataTypes & outDt)
{
    if (str.compare("Normal") == 0)
    {
        outDt = DT_NORMAL;
        return true;
    }
    else if (str.compare("Pos") == 0)
    {
        outDt = DT_POSITION;
        return true;
    }
    return false;
}


void SpaceConverterNode::AssertMyInputsValid(void) const
{
    Assert(AssertValidSrcDestSpaces(), errorMsg);
}