#include "TextureSettings.h"


std::string FilteringTypeToString(FilteringTypes filtType)
{
    switch (filtType)
    {
        case FT_NEAREST: return "Nearest";
        case FT_LINEAR: return "Linear";

        default:
            assert(false);
            return "Unknown filtering type: " + std::to_string(filtType);
    }
}
std::string WrappingTypeToString(WrappingTypes filtType)
{
    switch (filtType)
    {
        case WT_CLAMP: return "Clamp";
        case WT_WRAP: return "Wrap";

        default:
            assert(false);
            return "Unknown wrapping type: " + std::to_string(filtType);
    }
}

bool StringToFilteringType(const std::string & inStr, FilteringTypes & outVal)
{
    if (inStr.compare("Nearest") == 0)
    {
        outVal = FT_NEAREST;
        return true;
    }
    else if (inStr.compare("Linear") == 0)
    {
        outVal = FT_LINEAR;
        return true;
    }
    else return false;
}
bool StringToWrappingType(const std::string & inStr, WrappingTypes & outVal)
{
    if (inStr.compare("Clamp") == 0)
    {
        outVal = WT_CLAMP;
        return true;
    }
    else if (inStr.compare("Wrap") == 0)
    {
        outVal = WT_WRAP;
        return true;
    }
    else return false;
}


bool TextureSampleSettings2D::WriteData(DataWriter * writer, std::string & outError) const
{
    if (!writer->WriteString(FilteringTypeToString(MinFilter),
                             "Min Filter ('Nearest' or 'Linear')", outError))
    {
        outError = "Error writing min filter value: " + outError;
        return false;
    }
    if (!writer->WriteString(FilteringTypeToString(MagFilter),
                             "Mag Filter ('Nearest' or 'Linear')", outError))
    {
        outError = "Error writing mag filter value: " + outError;
        return false;
    }

    if (!writer->WriteString(WrappingTypeToString(HorzWrap),
                             "Horizontal wrapping ('Clamp' or 'Wrap')", outError))
    {
        outError = "Error writing horizontal wrapping value: " + outError;
        return false;
    }
    if (!writer->WriteString(WrappingTypeToString(VertWrap),
                             "Vertical wrapping ('Clamp' or 'Wrap')", outError))
    {
        outError = "Error writing vertical wrapping value: " + outError;
        return false;
    }

    return true;
}
bool TextureSampleSettings2D::ReadData(DataReader * reader, std::string & outError)
{
    MaybeValue<std::string> tryVal = reader->ReadString(outError);
    if (!tryVal.HasValue())
    {
        outError = "Error reading min filter: " + outError;
        return false;
    }
    if (!StringToFilteringType(tryVal.GetValue(), MinFilter))
    {
        outError = "Couldn't parse min filter value '" + tryVal.GetValue() + "'";
        return false;
    }

    tryVal = reader->ReadString(outError);
    if (!tryVal.HasValue())
    {
        outError = "Error reading mag filter: " + outError;
        return false;
    }
    if (!StringToFilteringType(tryVal.GetValue(), MagFilter))
    {
        outError = "Couldn't parse mag filter value '" + tryVal.GetValue() + "'";
        return false;
    }

    tryVal = reader->ReadString(outError);
    if (!tryVal.HasValue())
    {
        outError = "Error reading horizontal wrapping: " + outError;
        return false;
    }
    if (!StringToWrappingType(tryVal.GetValue(), HorzWrap))
    {
        outError = "Couldn't parse horizontal wrapping value '" + tryVal.GetValue() + "'";
        return false;
    }
    tryVal = reader->ReadString(outError);
    if (!tryVal.HasValue())
    {
        outError = "Error reading vertical wrapping: " + outError;
        return false;
    }
    if (!StringToWrappingType(tryVal.GetValue(), VertWrap))
    {
        outError = "Couldn't parse vertical wrapping value '" + tryVal.GetValue() + "'";
        return false;
    }


    return true;
}

bool TextureSampleSettings3D::WriteData(DataWriter * writer, std::string & outError) const
{
    if (!writer->WriteString(FilteringTypeToString(MinFilter),
                             "Min Filter ('Nearest' or 'Linear')", outError))
    {
        outError = "Error writing min filter value: " + outError;
        return false;
    }
    if (!writer->WriteString(FilteringTypeToString(MagFilter),
                             "Mag Filter ('Nearest' or 'Linear')", outError))
    {
        outError = "Error writing mag filter value: " + outError;
        return false;
    }

    if (!writer->WriteString(WrappingTypeToString(XWrap),
                             "X axis wrapping ('Clamp' or 'Wrap')", outError))
    {
        outError = "Error writing X-axis wrapping value: " + outError;
        return false;
    }
    if (!writer->WriteString(WrappingTypeToString(YWrap),
                             "Y axis wrapping ('Clamp' or 'Wrap')", outError))
    {
        outError = "Error writing Y-axis wrapping value: " + outError;
        return false;
    }
    if (!writer->WriteString(WrappingTypeToString(ZWrap),
                             "Z axis wrapping ('Clamp' or 'Wrap')", outError))
    {
        outError = "Error writing Z-axis wrapping value: " + outError;
        return false;
    }


    return true;
}
bool TextureSampleSettings3D::ReadData(DataReader * reader, std::string & outError)
{
    MaybeValue<std::string> tryVal = reader->ReadString(outError);
    if (!tryVal.HasValue())
    {
        outError = "Error reading min filter: " + outError;
        return false;
    }
    if (!StringToFilteringType(tryVal.GetValue(), MinFilter))
    {
        outError = "Couldn't parse min filter value '" + tryVal.GetValue() + "'";
        return false;
    }

    tryVal = reader->ReadString(outError);
    if (!tryVal.HasValue())
    {
        outError = "Error reading mag filter: " + outError;
        return false;
    }
    if (!StringToFilteringType(tryVal.GetValue(), MagFilter))
    {
        outError = "Couldn't parse mag filter value '" + tryVal.GetValue() + "'";
        return false;
    }

    tryVal = reader->ReadString(outError);
    if (!tryVal.HasValue())
    {
        outError = "Error reading X-axis wrapping: " + outError;
        return false;
    }
    if (!StringToWrappingType(tryVal.GetValue(), XWrap))
    {
        outError = "Couldn't parse X-axis wrapping value '" + tryVal.GetValue() + "'";
        return false;
    }
    tryVal = reader->ReadString(outError);
    if (!tryVal.HasValue())
    {
        outError = "Error reading Y-axis wrapping: " + outError;
        return false;
    }
    if (!StringToWrappingType(tryVal.GetValue(), YWrap))
    {
        outError = "Couldn't parse Y-axis wrapping value '" + tryVal.GetValue() + "'";
        return false;
    }
    tryVal = reader->ReadString(outError);
    if (!tryVal.HasValue())
    {
        outError = "Error reading Z-axis wrapping: " + outError;
        return false;
    }
    if (!StringToWrappingType(tryVal.GetValue(), ZWrap))
    {
        outError = "Couldn't parse Z-axis wrapping value '" + tryVal.GetValue() + "'";
        return false;
    }


    return true;
}


GLint ToGLInt(FilteringTypes tf, bool minFilter, bool genMips)
{
    return (tf == FilteringTypes::FT_LINEAR) ?
                ((minFilter && genMips) ?
                     GL_LINEAR_MIPMAP_LINEAR :
                     GL_LINEAR) :
                ((minFilter && genMips) ?
                     GL_NEAREST_MIPMAP_LINEAR :
                     GL_NEAREST);
}
GLint ToGLInt(WrappingTypes twa)
{
    return (twa == WrappingTypes::WT_CLAMP) ?
                GL_CLAMP_TO_EDGE :
                GL_REPEAT;
}



bool IsPixelSizeColor(PixelSizes pixelSize)
{
    switch (pixelSize)
    {
        case PS_8U:
        case PS_16U:
        case PS_16F:
        case PS_32F:
            return true;

        case PS_8U_GREYSCALE:
        case PS_16U_GREYSCALE:
        case PS_32F_GREYSCALE:
        case PS_16U_DEPTH:
        case PS_24U_DEPTH:
        case PS_32F_DEPTH:
            return false;

        default: assert(false); return false;
    }
}
bool IsPixelSizeGreyscale(PixelSizes pixelSize)
{
    switch (pixelSize)
    {
        case PS_8U_GREYSCALE:
        case PS_16U_GREYSCALE:
        case PS_32F_GREYSCALE:
            return true;

        case PS_8U:
        case PS_16U:
        case PS_16F:
        case PS_32F:
        case PS_16U_DEPTH:
        case PS_24U_DEPTH:
        case PS_32F_DEPTH:
            return false;

        default: assert(false); return false;
    }
}
bool IsPixelSizeDepth(PixelSizes pixelSize)
{
    switch (pixelSize)
    {
        case PS_16U_DEPTH:
        case PS_24U_DEPTH:
        case PS_32F_DEPTH:
            return true;

        case PS_8U:
        case PS_16U:
        case PS_16F:
        case PS_32F:
        case PS_8U_GREYSCALE:
        case PS_16U_GREYSCALE:
        case PS_32F_GREYSCALE:
            return false;

        default: assert(false); return false;
    }
}
bool IsPixelSizeFloat(PixelSizes size)
{
    switch (size)
    {
        case PS_16F:
        case PS_32F:
        case PS_32F_GREYSCALE:
        case PS_32F_DEPTH:
            return true;

        case PS_8U:
        case PS_16U:
        case PS_8U_GREYSCALE:
        case PS_16U_GREYSCALE:
        case PS_16U_DEPTH:
        case PS_24U_DEPTH:
            return false;

        default: assert(false); return false;
    }
}
unsigned int GetComponentSize(PixelSizes size)
{
    switch (size)
    {
        case PS_8U:
        case PS_8U_GREYSCALE:
            return 8;

        case PS_16F:
        case PS_16U:
        case PS_16U_GREYSCALE:
        case PS_16U_DEPTH:
            return 16;

        case PS_24U_DEPTH:
            return 24;

        case PS_32F:
        case PS_32F_GREYSCALE:
        case PS_32F_DEPTH:
            return 32;

        default: assert(false); return 0;
    }
}
GLenum ToGLenum(PixelSizes pixelSize)
{
    switch (pixelSize)
    {
        case PS_8U: return GL_RGBA8;
        case PS_16U: return GL_RGBA16;
        case PS_16F: return GL_RGBA16F;
        case PS_32F: return GL_RGBA32F;
        case PS_8U_GREYSCALE: return GL_R8;
        case PS_16U_GREYSCALE: return GL_R16;
        case PS_32F_GREYSCALE: return GL_R32F;
        case PS_16U_DEPTH: return GL_DEPTH_COMPONENT16;
        case PS_24U_DEPTH: return GL_DEPTH_COMPONENT24;
        case PS_32F_DEPTH: return GL_DEPTH_COMPONENT32F;

        default: assert(false); return GL_INVALID_ENUM;
    }
}