#include "DataLine.h"

#include "DataNode.h"


unsigned int DataLine::GetDataLineSize(void) const
{
    return (isConstantValue ? constantValue.GetSize() :
                              nonConstantValue->GetOutputs()[nonConstantValueIndex]);
}