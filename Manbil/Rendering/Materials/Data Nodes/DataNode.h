#pragma once

#include <vector>
#include <memory>

class DataNode;
typedef std::shared_ptr<DataNode> DataNodePtr;


//Represents a basic, atomic operation in a shader.
class DataNode
{
public:

    
    static const int MAXLINES = 12;

    //The input and output lines. Each node may take in up to 9 lines, and output up to 9 lines.
    unsigned int InputConnections[MAXLINES],
                 OutputConnections[MAXLINES];

    virtual unsigned int GetNumbInputLines(void) const = 0;
    virtual unsigned int GetNumbOutputLines(void) const = 0;

    //Gets any parameters/uniforms this node wants to define.
    virtual void GetParameterDeclarations(std::vector<std::string> & outDecls) = 0;
    //Gets any GLSL function declarations this node needs to use.
    virtual void GetFunctionDeclarations(std::vector<std::string> & outDecls) = 0;
    //The bit of code that will actually compute the outputs given the input lines.
    virtual std::string WriteOutputConnections(std::string inputNames[MAXLINES], std::string outputNames[MAXLINES]) = 0;

    //Puts the data nodes this node outputs to into "outOutputs".
    virtual void GetOutputs(std::vector<DataNodePtr> & outOuputs) = 0;
    //Puts the data nodes this node gets input from into "outInputs".
    virtual void GetInputs(std::vector<DataNodePtr> & outInputs) = 0;
};