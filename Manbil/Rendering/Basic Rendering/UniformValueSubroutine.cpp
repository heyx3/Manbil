#include "UniformValueSubroutine.h"


std::string SubroutineDefinition::Parameter::GetDeclaration(void) const
{
    return (IsInt ? VectorI::GetGLSLType(Size) : VectorF::GetGLSLType(Size)) + " " + Name;
}


typedef UniformValueSubroutine UVS;

UVS::UniformValueSubroutine(SubroutineDefinition definition, std::vector<std::string> possibleValues,
                            unsigned int currentValueIndex)
    : Definition(definition), PossibleValues(possibleValues), ValueIndex(currentValueIndex)
{
    PossibleValueIDs.reserve(PossibleValues.size());
    for (unsigned int i = 0; i < PossibleValues.size(); ++i)
    {
        PossibleValueIDs.push_back(0);
    }
}