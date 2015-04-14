#pragma once

#include "GraphSearchGoal.h"


//"NodeType" is the type of data used to represent a single spot on a graph;
//    it should have == and != operators.
//"ExtraData" is the type of any extra data that the user may need for computations.
template<typename NodeType, typename ExtraData = void*>
//An abstract class that computes the cost of going straight from one node to another.
//Keep in mind the "AStarSearch" class assumes any types inheriting from "Edge" provide a constructor
//    with the arguments "(NodeType start, NodeType end, ExtraData userData)".
struct Edge
{
public:

    NodeType Start, End;
    ExtraData UserData;


    Edge(NodeType start, NodeType end, ExtraData userData)
        : Start(start), End(end), UserData(userData) { }


    //Gets the cost of traversing this edge while searching for the given goal node(s).
    //This is used when calculating the shortest path through a graph.
    virtual float GetTraversalCost(GraphSearchGoal<NodeType>& goal) = 0;
    
    //Gets the cost of searching across this edge while searching for the given goal node(s).
    //This is used to limit the reach of a graph search algorithm.
    virtual float GetSearchCost(GraphSearchGoal<NodeType>& goal) = 0;
};