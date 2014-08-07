#include "XmlSerialization.h"

#include <assert.h>

using namespace tinyxml2;


std::string TinyXMLErrorToString(XMLError err)
{
    switch (err)
    {
        case XMLError::XML_NO_ERROR:
            return "";
        case XMLError::XML_NO_ATTRIBUTE:
            return "XML error: no attribute";
        case XMLError::XML_WRONG_ATTRIBUTE_TYPE:
            return "XML error: wrong attribute type";
        case XMLError::XML_ERROR_FILE_NOT_FOUND:
            return "File not found";
        case XMLError::XML_ERROR_FILE_COULD_NOT_BE_OPENED:
            return "File could not be opened";
        case XMLError::XML_ERROR_FILE_READ_ERROR:
            return "File could not be read";
        case XMLError::XML_ERROR_ELEMENT_MISMATCH:
            return "Element mismatch";
        case XMLError::XML_ERROR_PARSING_ELEMENT:
            return "Parsing element";
        case XMLError::XML_ERROR_PARSING_ATTRIBUTE:
            return "Parsing attribute";
        case XMLError::XML_ERROR_IDENTIFYING_TAG:
            return "Identifying tag";
        case XMLError::XML_ERROR_PARSING_TEXT:
            return "Parsing text";
        case XMLError::XML_ERROR_PARSING_CDATA:
            return "Parsing CData";
        case XMLError::XML_ERROR_PARSING_COMMENT:
            return "Parsing comment";
        case XMLError::XML_ERROR_PARSING_DECLARATION:
            return "Parsing declaration";
        case XMLError::XML_ERROR_PARSING_UNKNOWN:
            return "Parsing unknown";
        case XMLError::XML_ERROR_EMPTY_DOCUMENT:
            return "Empty document";
        case XMLError::XML_ERROR_MISMATCHED_ELEMENT:
            return "Mismatched element";
        case XMLError::XML_ERROR_PARSING:
            return "Parsing";
        case XMLError::XML_CAN_NOT_CONVERT_TEXT:
            return "Cannot convert text";
        case XMLError::XML_NO_TEXT_NODE:
            return "No text node";

        default: assert(false); return "Unknown error code " + std::to_string(err);
    }
}


XmlWriter::XmlWriter(std::string rootNodeName)
{
    currentRoot = doc.NewElement(rootNodeName.c_str());
    doc.InsertEndChild(currentRoot);
}
std::string XmlWriter::SaveData(std::string path)
{
    return TinyXMLErrorToString(doc.SaveFile(path.c_str()));
}


#define IMPL_WRITE_XML_DATA(dataType, dataTypeName, dataTypeString, valueToString) \
    bool XmlWriter::Write ## dataTypeName(dataType value, std::string name, std::string & outError) \
{ \
    XMLElement * child = doc.NewElement(dataTypeString); \
    child->SetAttribute("name", name.c_str()); \
    child->SetAttribute("value", valueToString.c_str()); \
    currentRoot->InsertEndChild(child); \
    \
    return true; \
}
IMPL_WRITE_XML_DATA(bool, Bool, "bool", (value ? std::string("true") : std::string("false")))
IMPL_WRITE_XML_DATA(unsigned char, Byte, "byte", std::to_string(value))
IMPL_WRITE_XML_DATA(int, Int, "int", std::to_string(value))
IMPL_WRITE_XML_DATA(unsigned int, UInt, "uint", std::to_string(value))
IMPL_WRITE_XML_DATA(float, Float, "float", std::to_string(value))
IMPL_WRITE_XML_DATA(double, Double, "double", std::to_string(value))
IMPL_WRITE_XML_DATA(const std::string &, String, "string", value)


bool XmlWriter::WriteCollection(std::string name, ElementWriter writerFunc, const void* collection,
                                unsigned int collectionSize, std::string & outError, void * optionalData)
{
    //Create the root of the collection.
    XMLElement * collectionRoot = doc.NewElement("collection");
    collectionRoot->SetAttribute("name", name.c_str());
    currentRoot->InsertEndChild(collectionRoot);

    //Go through every element and create a child node for it.
    for (unsigned int i = 0; i < collectionSize; ++i)
    {
        //Set up the element's node.
        XMLElement * collElement = doc.NewElement("element");
        collElement->SetAttribute("index", std::to_string(i).c_str());
        collectionRoot->InsertEndChild(collElement);

        //Move into this element's node and write the data.
        currentRoot = collElement;
        if (!writerFunc(collection, i, this, outError, optionalData))
            return false;
    }

    //Move out of the current XML element, back into the original "currentRoot".
    currentRoot = collectionRoot->Parent()->ToElement();
    return true;
}

bool XmlWriter::WriteDataStructure(const ISerializable & toSerialize, std::string name, std::string & outError)
{
    //Create the structure's root.
    XMLElement * dataStructureRoot = doc.NewElement("dataStructure");
    dataStructureRoot->SetAttribute("name", name.c_str());
    currentRoot->InsertEndChild(dataStructureRoot);

    //Move into that root and write the structure.
    currentRoot = dataStructureRoot;
    if (!toSerialize.WriteData(this, outError))
        return false;

    //Move out of that root, back into its parent.
    currentRoot = dataStructureRoot->Parent()->ToElement();

    return true;
}



XmlReader::XmlReader(std::string filePath, std::string & outErrorMsg)
{
    outErrorMsg = TinyXMLErrorToString(doc.LoadFile(filePath.c_str()));

    currentRoot = 0;
    currentChild = 0;

    if (outErrorMsg.empty())
    {
        //Find the root element.
        for (XMLNode * child = doc.FirstChild(); child != 0; child = child->NextSibling())
        {
            if (child->ToElement() != 0)
            {
                currentRoot = child->ToElement();
                break;
            }
        }

        //If no element was found, error.
        if (currentRoot == 0)
        {
            outErrorMsg = "Couldn't find a root XML element in the document";
            return;
        }

        //Get the first child.
        currentChild = currentRoot->FirstChildElement();
    }
}


#define XML_READ_ERROR_CHECK(dataType, dataTypeStr) \
    if (currentChild == 0) \
    { \
        outError = std::string("No more data in the structure '") + currentRoot->Attribute("name") + "'"; \
        return MaybeValue<dataType>(); \
    } \
    \
    if (std::string(currentChild->Name()).compare(dataTypeStr) != 0) \
    { \
        outError = std::string("The next data in this structure '") + currentRoot->Attribute("name") + \
                       "' is the " + currentChild->Name() + " '" + \
                       currentChild->Attribute("name") + "', not a " + dataTypeStr; \
        return MaybeValue<dataType>(); \
    }

MaybeValue<bool> XmlReader::ReadBool(std::string & outError)
{
    XML_READ_ERROR_CHECK(bool, "bool")

    std::string val = currentChild->Attribute("value");
    if (val.compare("true") == 0)
    {
        currentChild = currentChild->NextSiblingElement();
        return MaybeValue<bool>(true);
    }
    else if (val.compare("false") == 0)
    {
        currentChild = currentChild->NextSiblingElement();
        return MaybeValue<bool>(false);
    }
    else
    {
        outError = std::string("bool data '") + currentChild->Name() +
                       "' has an invalid value of '" + currentChild->Attribute("value") + "'";
        return MaybeValue<bool>();
    }
}
MaybeValue<std::string> XmlReader::ReadString(std::string & outError)
{
    XML_READ_ERROR_CHECK(std::string, "string")

    std::string val = currentChild->Attribute("value");
    currentChild = currentChild->NextSiblingElement();
    return MaybeValue<std::string>(val);
}


//Other implementations of "Read" functions are all nearly identical.
#define IMPL_XML_READ(dataType, dataTypeName, queryAttributeData, dataTypeStr, preDataType) \
    MaybeValue<dataType> XmlReader::Read ## dataTypeName(std::string & outError) \
    { \
        XML_READ_ERROR_CHECK(dataType, dataTypeStr); \
        \
        preDataType value; \
        outError = TinyXMLErrorToString(currentChild-> ## queryAttributeData("value", &value)); \
        \
        if (outError.empty()) \
        { \
            currentChild = currentChild->NextSiblingElement(); \
            return MaybeValue<dataType>((dataType)value); \
        } \
        else \
        { \
            outError = std::string("Invalid ") + dataTypeStr + " value of '" + currentChild->Attribute("value") + ": " + outError; \
            return MaybeValue<dataType>(); \
        } \
    }

IMPL_XML_READ(unsigned char, Byte, QueryUnsignedAttribute, "byte", unsigned int)
IMPL_XML_READ(int, Int, QueryIntAttribute, "int", int)
IMPL_XML_READ(unsigned int, UInt, QueryUnsignedAttribute, "uint", unsigned int)
IMPL_XML_READ(float, Float, QueryFloatAttribute, "float", float)
IMPL_XML_READ(double, Double, QueryDoubleAttribute, "double", double)

bool XmlReader::ReadCollection(ElementReader readerFunc, unsigned int bytesPerElement, std::string & outError,
                               std::vector<unsigned char> & outData, void * optionalData)
{
    if (currentChild == 0)
    {
        outError = std::string("No more data in the structure '") + currentRoot->Attribute("name") + "'";
        return false;
    }
    
    if (std::string(currentChild->Name()).compare("collection") != 0)
    {
        outError = std::string("The next data in this structure '") + currentRoot->Attribute("name") +
            "' is the " + currentChild->Name() + " '" +
            currentChild->Attribute("name") + "', not a collection";
        return false;
    }

    //Hold on to the new values of this instance's fields after this operation is done.
    XMLElement * oldRoot = currentRoot;
    XMLElement * nextChild = currentChild->NextSiblingElement();

    currentChild = currentChild->FirstChildElement();

    unsigned int index = 0;
    while (currentChild != 0)
    {
        //Prepare the data to be read.
        outData.reserve(outData.size() + bytesPerElement);
        for (unsigned int byte = 0; byte < bytesPerElement; ++byte)
            outData.insert(outData.end(), 0);

        //Try to read the data.
        currentRoot = currentChild;
        XMLElement * oldChild = currentChild;
        currentChild = currentRoot->FirstChildElement();
        if (!readerFunc(outData.data(), index, this, outError, optionalData))
            return false;

        currentChild = oldChild->NextSiblingElement();
        index += 1;
    }

    //Reset this reader's position to the next child after the collection.
    currentRoot = oldRoot;
    currentChild = nextChild;

    return true;
}

bool XmlReader::ReadDataStructure(ISerializable & toSerialize, std::string & outError)
{
    //Make sure the structure's root exists.
    if (currentChild == 0)
    {
        outError = std::string("No more data in the structure '") + currentRoot->Attribute("name") + "'";
        return false;
    }
    if (std::string(currentChild->Name()).compare("dataStructure") != 0)
    {
        outError = std::string("The next data in this structure '") + currentRoot->Attribute("name") +
                       "' is the " + currentChild->Name() + " '" +
                       currentChild->Attribute("name") + "', not a 'dataStructure'.";
        return false;
    }


    //Keep hold of this instance's new field data after this function is done.
    XMLElement * oldRoot = currentRoot;
    XMLElement * nextChild = currentChild->NextSiblingElement();

    //Try writing to the structure.
    currentRoot = currentChild;
    currentChild = currentRoot->FirstChildElement();
    if (!toSerialize.ReadData(this, outError))
        return false;

    //Move out of that structure's root, back into its parent.
    currentRoot = oldRoot;
    currentChild = nextChild;

    return true;
}