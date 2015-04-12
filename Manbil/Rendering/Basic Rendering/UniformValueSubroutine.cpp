#include "UniformValueSubroutine.h"


std::string SubroutineDefinition::Parameter::GetDeclaration(void) const
{
    return (IsInt ? VectorI::GetGLSLType(Size) : VectorF::GetGLSLType(Size)) + " " + Name;
}

std::string SubroutineDefinition::GetDefinition(void) const
{
    std::string ret = "subroutine ";

    ret += VectorF::GetGLSLType(ReturnValueSize) + " " + Name + "(";
    for (unsigned int i = 0; i < Params.size(); ++i)
    {
        if (i > 0) ret += ", ";
        ret += Params[i].GetDeclaration();
    }
    ret += ");";

    return ret;
}


typedef UniformValueSubroutine UVS;

std::string UVS::GetDeclaration(void) const
{
    return "subroutine uniform " + Definition.Name + " " + Name + ";";
}

UVS::UniformValueSubroutine(SubroutineDefinition definition, std::vector<std::string> possibleValues,
                            unsigned int currentValueIndex, std::string name, UniformLocation location)
    : Definition(definition), PossibleValues(possibleValues),
      ValueIndex(currentValueIndex), Name(name), Location(location)
{
    PossibleValueIDs.reserve(PossibleValues.size());
    for (unsigned int i = 0; i < PossibleValues.size(); ++i)
    {
        PossibleValueIDs.push_back(0);
    }
}