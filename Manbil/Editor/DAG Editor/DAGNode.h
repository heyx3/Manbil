#pragma once

#include "../../IO/DataSerialization.h"
#include "../IEditable.h"
#include "DAGInput.h"



template<typename DAGNodeType, typename DAGInputType>
//A base class for the visitor pattern being applied to a DAG from a root node.
//Note that after visiting each DAG node, this visitor will remember and always skip that node.
class DAGVisitor
{
public:

    virtual ~DAGVisitor(void) { }
    
    //Clears this visitor's node history so that it forgets any nodes it's already visited.
    void ClearNodeHistory(void) { visitedAlready.clear(); }
    void Visit(DAGNodeType * node)
    {
        if (std::find(visitedAlready.begin(), visitedAlready.end(), node.GetName()) == visitedAlready.end())
        {
            visitedAlready.insert(visitedAlready.end(), node.GetName());
            ActOnNode(node);

            const std::vector<DAGInputType> & children = node.GetChildren();
            for (unsigned int i = 0; i < children.size(); ++i)
                if (!children[i].IsConstant())
                    this->Visit((DAGNodeType*)children[i].GetDAGNode());
        }
    }

protected:

    //Visits the given node and does some action on it.
    virtual void ActOnNode(DAGNodeType * node) = 0;

private:

    std::vector<void*> visitedAlready;
};



//The type of DAG connection this node uses.
//The data type should be easily copyable.
template<typename DAGInputType>
//A node in a DAG. Can generate a GUI to edit it and be serialized to a file.
class DAGNode : public ISerializable, public IEditable
{
public:


    DAGNode(const std::vector<DAGInputType> inputs)



protected:

    



private:

    std::vector<DAGInputType> inputs;
};