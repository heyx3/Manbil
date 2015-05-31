#pragma once

#include "GraphSearchGoal.h"


//"NodeType" is the type of data used to represent a single spot on a graph;
//    it should have == and != operators.
//"ExtraData" is the type of any extra data that the user may need for computations.
template<typename NodeType, typename ExtraData = void*>
//An abstract class that computes the cost of going straight from one node to another.
//If using the "AStarSearch" template, make sure to do the following with your Edge child class:
//   1. Define a constructor with the params "(NodeType start, NodeType end, ExtraData userData)".
//   2. Don't always assume that the nodes are actually connected in the graph!
struct Edge
{
public:

    NodeType Start, End;
    ExtraData UserData;


    Edge(NodeType start, NodeType end, ExtraData userData)
        : Start(start), End(end), UserData(userData) { }


    //Gets the cost of traversing this edge while searching for the given goal node(s).
    //This is used when calculating the shortest path through a graph.
    virtual float GetTraversalCost(const GraphSearchGoal<NodeType>& goal) const = 0;
    
    //Gets the cost of searching across this edge while searching for the given goal node(s).
    //This is used to limit the reach of a graph search algorithm.
    virtual float GetSearchCost(const GraphSearchGoal<NodeType>& goal) const = 0;
};