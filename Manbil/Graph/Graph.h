#pragma once

#include <vector>
#include "Edge.h"


//"NodeType" is the type of item representing a single spot on a graph;
//    it should implement the == and != operators.
//"EdgeType" is the type of edge connecting two nodes; it should inherit from "Edge<NodeType, ...>"
template<typename NodeType, typename EdgeType>
//Represents a collection of "nodes" connected by "edges".
class Graph
{
public:

    //Gets all edges connecting the given node to another node.
    virtual void GetConnectedEdges(NodeType startNode, std::vector<EdgeType>& outConnections) const = 0;
};