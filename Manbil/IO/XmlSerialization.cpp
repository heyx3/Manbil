#include "XmlSerialization.h"

#include <assert.h>

using namespace tinyxml2;




//The following stuff converts between binary data and hex data.


const char valueToHex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                            'A', 'B', 'C', 'D', 'E', 'F' };
const unsigned char hexToValue(char hex)
{
    switch (hex)
    {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'A': return 10;
        case 'B': return 11;
        case 'C': return 12;
        case 'D': return 13;
        case 'E': return 14;
        case 'F': return 15;

        default:
            assert(false);
            return 0;
    }
}

//Converts byte data into two-digit hex numbers. The hex numbers are combined and output into a string.
void BytesToHex(const unsigned char* byteData, unsigned int nBytes, std::string& outHex)
{
    outHex.reserve(outHex.size() + nBytes);
    for (unsigned int i = 0; i < nBytes; ++i)
    {
        unsigned char data = byteData[i];
        outHex += valueToHex[data / 16];
        outHex += valueToHex[data % 16];
    }
}
//Converts a sequence of two-digit hex numbers into byte data.
//Outputs the byte data to the end of the given vector.
void HexToBytes(const std::string& hexData, std::vector<unsigned char>& outBytes, std::string& outError)
{
    if (hexData.size() % 2 != 0)
    {
        outError = std::string("Hex string should be filled with two-digit numbers ") +
                        "but it has an odd number of digits!";
        throw XmlReader::EXCEPTION_FAILURE;
    }

    outBytes.reserve(outBytes.size() + (hexData.size() / 2));
    for (unsigned int i = 0; i < hexData.size(); i += 2)
    {
        unsigned char sixteensPlace = hexToValue(hexData[i]),
                      onesPlace = hexToValue(hexData[i + 1]);
        outBytes.push_back((16 * sixteensPlace) + onesPlace);
    }
}


//Converts a TinyXML error code into an error message.
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

        default:
            assert(false);
            return "Unknown error code " + std::to_string(err);
    }
}


XmlWriter::XmlWriter(std::string rootNodeName)
{
    currentRoot = doc.NewElement(rootNodeName.c_str());
    doc.InsertEndChild(currentRoot);
}

#include <fstream>
std::string XmlWriter::SaveData(const std::string& path)
{
    //Create the file, or overwrite it with an empty file.
    errno = 0;
    FILE* file = fopen(path.c_str(), "w");
    if (file == 0)
    {
        return "The file doesn't exist to be written to, or it cannot be opened.";
    }
    
    fclose(file);
    
    return TinyXMLErrorToString(doc.SaveFile(path.c_str()));
}


#define IMPL_WRITE_XML_DATA(dataType, dataTypeName, dataTypeString, valueToString) \
    void XmlWriter::Write ## dataTypeName(dataType value, const std::string& name) \
    { \
        XMLElement* child = doc.NewElement(dataTypeString); \
        child->SetAttribute("name", name.c_str()); \
        child->SetAttribute("value", valueToString.c_str()); \
        currentRoot->InsertEndChild(child); \
    }
IMPL_WRITE_XML_DATA(bool, Bool, "bool", (value ? std::string("true") : std::string("false")))
IMPL_WRITE_XML_DATA(unsigned char, Byte, "byte", std::to_string((unsigned int)value))
IMPL_WRITE_XML_DATA(int, Int, "int", std::to_string(value))
IMPL_WRITE_XML_DATA(unsigned int, UInt, "uint", std::to_string(value))
IMPL_WRITE_XML_DATA(float, Float, "float", std::to_string(value))
IMPL_WRITE_XML_DATA(double, Double, "double", std::to_string(value))
IMPL_WRITE_XML_DATA(const std::string&, String, "string", value)


void XmlWriter::WriteBytes(const unsigned char* bytes, unsigned int nBytes, const std::string& name)
{
    XMLElement* child = doc.NewElement("byteData");
    child->SetAttribute("name", name.c_str());

    std::string value;
    BytesToHex(bytes, nBytes, value);
    child->SetAttribute("value", value.c_str());

    currentRoot->InsertEndChild(child);
}


void XmlWriter::WriteCollection(ElementWriter writerFunc, const std::string& name,
                                unsigned int bytesPerElement,
                                const void* collection, unsigned int collectionSize,
                                void* optionalData)
{
    //Create the root of the collection.
    XMLElement * collectionRoot = doc.NewElement("collection");
    collectionRoot->SetAttribute("name", name.c_str());
    currentRoot->InsertEndChild(collectionRoot);

    //Go through every element and create a child node for it.
    for (unsigned int i = 0; i < collectionSize; ++i)
    {
        //Set up the element's node.
        XMLElement* collElement = doc.NewElement("element");
        collElement->SetAttribute("index", std::to_string(i).c_str());
        collectionRoot->InsertEndChild(collElement);

        //Move into this element's node and write the data.
        currentRoot = collElement;
        const void* element = ((const char*)collection) + (i * bytesPerElement);
        writerFunc(this, element, i, optionalData);
    }

    //Move out of the current XML element, back into the original "currentRoot".
    currentRoot = collectionRoot->Parent()->ToElement();
}

void XmlWriter::WriteDataStructure(const IWritable& toSerialize, const std::string& name)
{
    //Create the structure's root.
    XMLElement* dataStructureRoot = doc.NewElement("dataStructure");
    dataStructureRoot->SetAttribute("name", name.c_str());
    currentRoot->InsertEndChild(dataStructureRoot);

    //Move into that root and write the structure.
    currentRoot = dataStructureRoot;
    toSerialize.WriteData(this);

    //Move out of that root, back into its parent.
    currentRoot = dataStructureRoot->Parent()->ToElement();
}



XmlReader::XmlReader(const std::string& filePath)
{
    Reload(filePath, ErrorMessage);
}
void XmlReader::Reload(const std::string& filePath, std::string& err)
{
    doc.Clear();

    err = TinyXMLErrorToString(doc.LoadFile(filePath.c_str()));

    currentRoot = 0;
    currentChild = 0;

    if (err.empty())
    {
        //Find the root element.
        for (XMLNode* child = doc.FirstChild(); child != 0; child = child->NextSibling())
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
            err = "Couldn't find a root XML element in the document";
            return;
        }

        //Get the first child.
        currentChild = currentRoot->FirstChildElement();
    }
}


//Throws an exception if the "currentChild" field in XML reader doesn't use the given element tag.
#define XML_READ_ERROR_CHECK(dataTypeStr) \
    if (currentChild == 0) \
    { \
        ErrorMessage = std::string("No more data in the structure '") + \
                           currentRoot->Attribute("name") + "'"; \
        throw EXCEPTION_FAILURE; \
    } \
    \
    if (std::string(currentChild->Name()).compare(dataTypeStr) != 0) \
    { \
        ErrorMessage = std::string("The next data in this structure '") + \
                           currentRoot->Attribute("name") + \
                           "' is the " + currentChild->Name() + " '" + \
                           currentChild->Attribute("name") + "', not a " + dataTypeStr; \
        throw EXCEPTION_FAILURE; \
    }

void XmlReader::ReadBool(bool& outB)
{
    XML_READ_ERROR_CHECK("bool")

    std::string val = currentChild->Attribute("value");
    if (val.compare("true") == 0)
    {
        currentChild = currentChild->NextSiblingElement();
        outB = true;
    }
    else if (val.compare("false") == 0)
    {
        currentChild = currentChild->NextSiblingElement();
        outB = false;
    }
    else
    {
        ErrorMessage = std::string("bool data '") + currentChild->Name() +
                           "' has an invalid value of '" + currentChild->Attribute("value") + "'";
        throw EXCEPTION_FAILURE;
    }
}
void XmlReader::ReadString(std::string& outStr)
{
    XML_READ_ERROR_CHECK("string")

    outStr = currentChild->Attribute("value");
    currentChild = currentChild->NextSiblingElement();
}


//Other implementations of "Read" functions are all nearly identical.
#define IMPL_XML_READ(dataType, dataTypeName, queryAttributeData, dataTypeStr, preDataType) \
    void XmlReader::Read ## dataTypeName(dataType & outData) \
    { \
        XML_READ_ERROR_CHECK(dataTypeStr); \
        \
        preDataType value; \
        ErrorMessage = TinyXMLErrorToString(currentChild-> ## queryAttributeData("value", &value)); \
        \
        if (ErrorMessage.empty()) \
        { \
            currentChild = currentChild->NextSiblingElement(); \
            outData = (dataType)value; \
        } \
        else \
        { \
            ErrorMessage = std::string("Invalid ") + dataTypeStr + " value of '" + \
                               currentChild->Attribute("value") + ": " + ErrorMessage; \
            throw EXCEPTION_FAILURE; \
        } \
    }

IMPL_XML_READ(unsigned char, Byte, QueryUnsignedAttribute, "byte", unsigned int)
IMPL_XML_READ(int, Int, QueryIntAttribute, "int", int)
IMPL_XML_READ(unsigned int, UInt, QueryUnsignedAttribute, "uint", unsigned int)
IMPL_XML_READ(float, Float, QueryFloatAttribute, "float", float)
IMPL_XML_READ(double, Double, QueryDoubleAttribute, "double", double)

void XmlReader::ReadBytes(std::vector<unsigned char>& outBytes)
{
    XML_READ_ERROR_CHECK("byteData");

    std::string hexNumbers = currentChild->Attribute("value");
    currentChild = currentChild->NextSiblingElement();

    HexToBytes(hexNumbers, outBytes, ErrorMessage);
}

void XmlReader::ReadCollection(ElementReader readerFunc, CollectionResizer resizer,
                               void* pCollection, void* optionalData)
{
    if (currentChild == 0)
    {
        ErrorMessage = std::string("No more data in the structure '") +
                           currentRoot->Attribute("name") + "'";
        throw EXCEPTION_FAILURE;
    }
    
    if (std::string(currentChild->Name()).compare("collection") != 0)
    {
        ErrorMessage = std::string("The next data in this structure '") +
                           currentRoot->Attribute("name") +
                           "' is the " + currentChild->Name() + " '" +
                           currentChild->Attribute("name") + "', not a collection";
        throw EXCEPTION_FAILURE;
    }
    //TODO: Use that XML_READ_ERROR_CHECK macro instead of the above two conditionals.

    //Hold on to the new values of this instance's fields after this operation is done.
    XMLElement* oldRoot = currentRoot;
    XMLElement* nextChild = currentChild->NextSiblingElement();

    currentChild = currentChild->FirstChildElement();

    unsigned int index = 0;
    while (currentChild != 0)
    {
        //Prepare the data to be read.
        resizer(pCollection, index + 1);

        //Try to read the data.
        currentRoot = currentChild;
        XMLElement* oldChild = currentChild;
        currentChild = currentRoot->FirstChildElement();
        readerFunc(this, pCollection, index, optionalData);

        currentChild = oldChild->NextSiblingElement();
        index += 1;
    }

    //Reset this reader's position to the next child after the collection.
    currentRoot = oldRoot;
    currentChild = nextChild;
}

void XmlReader::ReadDataStructure(IReadable& toSerialize)
{
    //Make sure the structure's root exists.
    if (currentChild == 0)
    {
        ErrorMessage = std::string("No more data in the structure '") +
                           currentRoot->Attribute("name") + "'";
        throw EXCEPTION_FAILURE;
    }
    if (std::string(currentChild->Name()).compare("dataStructure") != 0)
    {
        ErrorMessage = std::string("The next data in this structure '") +
                           currentRoot->Attribute("name") + "' is the " + currentChild->Name() + " '" +
                           currentChild->Attribute("name") + "', not a 'dataStructure'.";
        throw EXCEPTION_FAILURE;
    }
    //TODO: Use that XML_READ_ERROR_CHECK macro instead of the above two conditionals.


    //Keep hold of this instance's new field data after this function is done.
    XMLElement* oldRoot = currentRoot;
    XMLElement* nextChild = currentChild->NextSiblingElement();

    //Try writing to the structure.
    currentRoot = currentChild;
    currentChild = currentRoot->FirstChildElement();
    toSerialize.ReadData(this);

    //Move out of that structure's root, back into its parent.
    currentRoot = oldRoot;
    currentChild = nextChild;
}