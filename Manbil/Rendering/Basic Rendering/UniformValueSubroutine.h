#pragma once

#include <vector>
#include <memory>
#include "OpenGLIncludes.h"
#include "GLVectors.h"


//Subroutines are essentially OpenGL's version of function pointers,
//    allowing you to swap out behaviors at run-time.
//They are a bit of a headache to set up, but can be useful in some situations.



//The definition of the function signature for a subroutine -- its arguments and return value.
//Each input is assumed to be a floating-point vector (unless "IsInt" is true, in which case
//    it is a signed integer vector).
//The output is always a floating-point vector.
struct SubroutineDefinition
{
public:
    
    //A potential input into the subroutine.
    //If "IsInt" is false, it is a float vector. Otherwise, it is a signed int vector.
    struct Parameter
    {
        unsigned int Size;
        std::string Name;
        bool IsInt;

        Parameter(unsigned int size = 0, std::string name = "", bool isInt = false)
            : Size(size), Name(name), IsInt(isInt) { }

        std::string GetDeclaration(void) const;

        bool operator==(const Parameter& other) const
        {
            return Size == other.Size && Name == other.Name && IsInt == other.IsInt;
        }
    };


    //The name of this subroutine definition.
    std::string Name;

    //The inputs to this subroutine.
    std::vector<Parameter> Params;

    //The size of the subroutine's output (float, vec2, vec3, or vec4).
    unsigned int ReturnValueSize;

    //The shader this uniform resides in.
    Shaders Shader;


    SubroutineDefinition(Shaders shader = Shaders::SH_VERTEX, unsigned int returnValSize = 0,
                         std::string name = "", std::vector<Parameter> params = std::vector<Parameter>())
        : Shader(shader), ReturnValueSize(returnValSize), Name(name), Params(params) { }


    std::string GetDefinition(void) const;


    bool operator==(const SubroutineDefinition& other) const
    {
        return Name == other.Name && Params == other.Params &&
               ReturnValueSize == other.ReturnValueSize && Shader == other.Shader;
    }
};



//Represents a subroutine -- its signature and the different functions it can point to.
struct UniformValueSubroutine
{
public:

    //The definition of the subroutine.
    SubroutineDefinition Definition;

    //The name of the uniform parameter.
    std::string Name;

    //The values (i.e. function names) this subroutine uniform can have.
    std::vector<std::string> PossibleValues;
    //The index in the "PossibleValues" collection corresponding to the current value of the subroutine.
    unsigned int ValueIndex = 0;

    //The location of the uniform in the GLSL program.
    UniformLocation Location;
    //The ID of each uniform value in the GLSL program.
    std::vector<RenderObjHandle> PossibleValueIDs;


    //Creates a new instance. Fills in the "PossibleValueIDs" vector with default values of 0.
    UniformValueSubroutine(SubroutineDefinition definition = SubroutineDefinition(),
                           std::vector<std::string> possibleValues = std::vector<std::string>(),
                           unsigned int currentValueIndex = 0, std::string name = "",
                           UniformLocation location = 0);


    std::string GetDeclaration(void) const;


    bool operator==(const UniformValueSubroutine& other) const
    {
        return Definition == other.Definition && Name == other.Name &&
               PossibleValues == other.PossibleValues && ValueIndex == other.ValueIndex &&
               Location == other.Location && PossibleValueIDs == other.PossibleValueIDs;
    }
};
