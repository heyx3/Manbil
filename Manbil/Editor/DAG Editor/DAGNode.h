#pragma once


#include "../../IO/DataSerialization.h"
#include "../IEditable.h"
#include "DAGInput.h"


//The type of DAG connection this node uses.
//The data type should be easily copyable.
template<typename DAGConnectionType>
//A node in a DAG. Can generate a GUI to edit it and be serialized to a file.
class DAGNode : public ISerializable, public IEditable
{
public:


protected:

    

private:

    std::vector<DAGConnectionType> inputs;
};