#pragma once

#include <vector>
#include <assert.h>
#include <memory>
#include "../../../Math/Vectors.h"


class DataNode;
typedef std::shared_ptr<DataNode> DataNodePtr;


//Represents a collection of 1-4 floats.
//If a Vector has 0 floats, it is considered invalid.
struct Vector
{
public:

    //Gets the number of floats this Vector has.
    unsigned int GetSize(void) const { return size; }
    //The floats representing this Vector.
    const float * GetValue(void) const { return values; }

    void SetValue(float newVal)    { size = 1; values[0] = newVal; }
    void SetValue(Vector2f newVal) { size = 2; values[0] = newVal.x; values[1] = newVal.y; }
    void SetValue(Vector3f newVal) { size = 3; values[0] = newVal.x; values[1] = newVal.y; values[2] = newVal.z; }
    void SetValue(Vector4f newVal) { size = 4; values[0] = newVal.x; values[1] = newVal.y; values[2] = newVal.z; values[3] = newVal.w; }
    //Marks this Vector as invalid.
    void SetValue(void) { size = 0; }

    Vector(void) : size(0) { values[0] = 0.0f; values[1] = 0.0f; values[2] = 0.0f; values[3] = 0.0f; }
    Vector(float value) { SetValue(value); }
    Vector(Vector2f value) { SetValue(value); }
    Vector(Vector3f value) { SetValue(value); }
    Vector(Vector4f value) { SetValue(value); }
    Vector(unsigned int _size, float defaultValue = 0.0f) : size(_size) { for (unsigned int i = 0; i < size; ++i) values[i] = defaultValue; }
    Vector(const Vector & cpy) : size(cpy.size) { for (unsigned int i = 0; i < size; ++i) values[i] = cpy.values[i]; }

    //Assumes both vectors are the same size. Adds their components together.
    Vector operator+(const Vector & other) const;
    //Assumes both vectors are the same size. Subtracts the other's components from this one's components.
    Vector operator-(const Vector & other) const;
    //Assumes both vectors are the same size. Multiplies components together.
    Vector operator*(const Vector & other) const;
    //Assumes both vectors are the same size. Divides this Vector's components by the other's components.
    Vector operator/(const Vector & other) const;

    std::string GetGLSLType(void) const;

private:

    unsigned int size;
    float values[4];
};



//Represents an input into a DataNode.
struct DataLine
{
public:

    //This data line either feeds in a constant input, or gets the input value from a DataNode's output.
    bool IsConstant(void) const { return isConstantValue; }

    //Gets the number of floats in this data line.
    unsigned int GetDataLineSize(void) const { return (isConstantValue ? constantValue.GetSize() : nonConstantValue->GetOutputs()[nonConstantValueIndex]); }

    //This function is only valid if this DataLine has a constant input value.
    Vector GetConstantValue(void) const { assert(isConstantValue); return constantValue; }
    //This function is only valid if this DataLine has a DataNode input value.
    DataNodePtr GetDataNodeValue(void) const { assert(!isConstantValue); return nonConstantValue; }

    //Assuming this data line has a DataNode input value, gets the index of the DataNode input's data line.
    unsigned int GetDataNodeLineIndex(void) const { assert(!isConstantValue); return nonConstantValueIndex; }


    //Creates a DataLine that gets its input value from a DataNode.
    DataLine(DataNodePtr input, unsigned int outputLineIndex)
        : isConstantValue(false), nonConstantValue(input), nonConstantValueIndex(outputLineIndex) { }
    //Creates a DataLine with a constant input value.
    DataLine(const Vector & constantInput) : isConstantValue(true), constantValue(constantInput) { }


private:

    bool isConstantValue;

    Vector constantValue;

    DataNodePtr nonConstantValue;
    unsigned int nonConstantValueIndex;
};




//Represents a basic, atomic operation in a shader.
class DataNode
{
public:

    //Gets the identifier unique for this dataNode.
    unsigned int GetUniqueID(void) const { return id; }

    //Gets the name of this data node. Only used for commenting the generated shader code.
    virtual std::string GetName(void) { return "unknown DataNode"; }


    DataNode(const std::vector<DataLine> & _inputs, const std::vector<unsigned int> & outputSizes)
        : id(GetNextID()), inputs(_inputs), outputs(outputSizes) { }
    DataNode(const DataNode & cpy); // Intentionally left blank.


    //Gets any parameters/uniforms this node definesi.
    virtual void GetParameterDeclarations(std::vector<std::string> & outDecls) { }
    //Gets any GLSL helper function declarations this node needs to use.
    virtual void GetFunctionDeclarations(std::vector<std::string> & outDecls) { }
    //The bit of code that will actually compute the outputs given the inputs.
    std::string WriteOutputConnections(std::string * inputNames, std::string * outputNames) const
    {
        //TODO: FInish
    }


    //Gets this node's input lines.
    const std::vector<DataLine> & GetInputs(void) const { return inputs; }
    //Gets this node's output lines. Each element represents the size of that output line's Vector.
    const std::vector<unsigned int> & GetOutputs(void) const { return outputs; }


protected:

    static std::vector<DataLine> MakeVector(const DataLine & dat);
    static std::vector<DataLine> MakeVector(const DataLine & dat, const DataLine & dat2);
    static std::vector<DataLine> MakeVector(const DataLine & dat, const DataLine & dat2, const DataLine & dat3);

    static std::vector<unsigned int> MakeVector(unsigned int dat);
    static std::vector<unsigned int> MakeVector(unsigned int dat, unsigned int dat2);
    static std::vector<unsigned int> MakeVector(unsigned int dat, unsigned int dat2, unsigned int dat3);


    virtual std::string WriteMyOutputConnections(std::string * inputNames, std::string * outputNames) const = 0;


private:

    std::vector<DataLine> inputs;
    std::vector<unsigned int> outputs;


    static unsigned int nextID;
    static unsigned int GetNextID(void) { unsigned int id = nextID; nextID += 1; return id; }

    unsigned int id;
};