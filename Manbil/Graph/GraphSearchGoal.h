#pragma once

#include "../OptionalValue.h"



//The type of item representing a single spot on a graph;
//    it should implement the == and != operators.
template<typename NodeType>
//A function that tests whether the given node passes some test.
using NodeTester = bool(*)(const NodeType& node);



//The type of item representing a single spot on a graph;
//    it should implement the == and != operators.
template<typename NodeType>
//The representation of a goal for almost any graph search.
//Has two different potential goals:
// 1) A specific end node.
// 2) Any node that meets a certain criteria.
//If both goals exist, then they both will be used in a graph search:
//    the specific end node will take priority in search heuristics,
//    but the search may also end on any node that satisfies the criteria instead.
//If neither goal exists, then the instance is invalid.
struct GraphSearchGoal
{
public:

    OptionalValue<NodeType> SpecificEnd;

    //Returns true if the given node is a suitable end point.
    NodeTester<NodeType> EndNodeCriteria;


    GraphSearchGoal(NodeType specificEnd)
        : SpecificEnd(specificEnd), EndNodeCriteria(0) { }

    GraphSearchGoal(NodeTester<NodeType> endNodeCriteria)
        : SpecificEnd(), EndNodeCriteria(isNodeAnEnd) { }

    GraphSearchGoal(NodeType specificEnd, NodeTester<NodeType> endNodeCriteria)
        : SpecificEnd(specificEnd), EndNodeCriteria(endNodeCriteria) { }
};