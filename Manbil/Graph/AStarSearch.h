#pragma once

#include <unordered_map>

#include "Graph.h"
#include "IndexedPriorityQueue.h"



//A graph-searching system that supports both A* and the more generic Dijkstra search.


//"NodeType" is the type of node used in this pathfinder.
//"EdgeType" should inherit from "Edge<NodeType, ExtraData>".
//"SearchGoalType" is the representation of this search class's goals.
//"ExtraData" is the type of user-specified data
//    that can be passed into edges' cost-calculation functions.
template<typename NodeType, typename EdgeType,
         typename SearchGoalType = GraphSearchGoal<NodeType>,
         typename ExtraData = void*>
//Searches a graph for the shortest path from a start node to some end node.
class AStarSearch
{
public:

    typedef Graph<NodeType, EdgeType>* GraphPtr;
    
    
    //User-specified data that will get passed into edges' cost-calculation methods.
    ExtraData UserData;

    //The graph being searched.
    GraphPtr GraphToSearch;


    AStarSearch(GraphPtr graph) : GraphToSearch(graph) { }
    AStarSearch(GraphPtr graph, ExtraData userData) : GraphToSearch(graph), UserData(userData) { }


    //Gets the shortest path from the given start to the given end.
    //Optionally takes in a limit to the max search cost of the path.
    //Returns whether the search was cut off by the search limit before finding an end to the path.
    bool Search(NodeType start, SearchGoalType endGoal, std::vector<NodeType>& outPath,
                float maxSearchCost = -1.0f) const
    {
        //Each node will be indexed to the connecting node that takes you back towards the start node.
        std::unordered_map<NodeType, NodeType> pathTree;
        //Each node will be indexed by the cost to traverse to it from the start node.
        std::unordered_map<NodeType, float> costToTraverseToNode;
        //Each node will be indexed by the cost to search to it from the start node.
        std::unordered_map<NodeType, float> costToSearchToNode;

        //The actual end of the path.
        OptionalValue<NodeType> finalDestination;

        //All nodes that have been searched already.
        std::vector<NodeType> consideredNodes;

        //A temp list used inside the main loop.
        std::vector<EdgeType> tempConnections;

        //The connections that need to be searched next.
        IndexedPriorityQueue<EdgeType> edgesToSearch;


        //Initialize the search loop.
        edgesToSearch.Enqueue(EdgeType(start, start), 0.0f);
        pathTree[start] = start;
        costToTraverseToNode[start] = 0.0f;
        costToSearchToNode[start] = 0.0f;

        
        //Keep searching until we run out of connections to search through.
        while (edgesToSearch.GetSize() > 0)
        {
            //Get info about the node being searched.
            IndexedPriorityQueue<EdgeType>::ItemAndCost toSearch = edgesToSearch.Dequeue();
            float costToTraverse = toSearch.Cost;
            EdgeType edgeToSearch = toSearch.Item;
            float costToSearch = costToSearchToNode[edgeToSearch.End];

            //Put the node into the path tree if it hasn't been already
            //    (if it has been, then this is guaranteed to be a longer path).
            if (pathTree.find(edgeToSearch.End) == pathTree.end())
                pathTree[edgeToSearch.End] = edgeToSearch.Start;


            //If this node is a valid goal, make the path and exit.
            if ((endGoal.SpecificEnd.HasValue() &&
                 endGoal.SpecificEnd.GetValue() == edgeToSearch.End) ||
                (endGoal.EndNodeCriteria != 0 &&
                 endGoal.EndNodeCriteria(edgeToSearch.End)))
            {
                finalDestination = edgeToSearch.End;
                BuildPath(start, finalDestination.GetValue(), pathTree, outPath);
                return true;
            }


            //If the search cost of this node is too high, don't continue to search past it.
            if (maxSearchCost >= 0.0f && costToSearch >= maxSearchCost)
            {
                continue;
            }

            //Get all connections and mark them to be searched.
            tempConnections.clear();
            GraphToSearch.GetConnectedEdges(edgeToSearch.End, tempConnections);
            for (unsigned int i = 0; i < tempConnections.size(); ++i)
            {
                const EdgeType& tempConn = tempConnections[i];
                float tempTraversalCost = costToTraverse + tempConn.GetTraversalCost(endGoal);

                //Make sure that searching this connection isn't too expensive.
                float tempSearchCost = costToSearch + tempConn.GetSearchCost(endGoal);
                if (maxSearchCost < 0.0f || tempSearchCost <= maxSearchCost)
                {
                    //Find whether this node has been searched already.
                    auto iteratorLoc = std::find(consideredNodes.begin(), consideredNodes.end(),
                                                 tempConn.End);

                    bool alreadySearched = (iteratorLoc != consideredNodes.end());


                    //If it hasn't been searched already, add it to the search frontier.
                    if (!alreadySearched)
                    {
                        edgesToSearch.Enqueue(tempConn, tempTraversalCost);
                        consideredNodes.push_back(tempConn.End);
                    }

                    //If it hasn't been searched yet OR it has but this route is cheaper,
                    //    update the cached pathing data to use this path.
                    if (!alreadySearched || tempTraversalCost <= costToTraverseToNode[tempConn.End])
                    {
                        costToTraverseToNode[tempConn.End] = tempTraversalCost;
                        costToSearchToNode[tempConn.End] = tempSearchCost;
                    }

                    //If is HAS been searched already and this route is cheaper, update the path tree.
                    if (alreadySearched && tempTraversalCost <= costToTraverseToNode[tempConn.End])
                    {
                        pathTree[tempConn.End] = tempConn.Start;
                    }
                }
            }
        }


        //We couldn't find any end nodes, so get an estimation of the right way to go.
        if (pathTree.size() == 0)
        {
            finalDestination = start;
        }
        else
        {
            float bestDist = std::numeric_limits<float>::max();
            float tempDist;

            for (auto iterator = pathTree.begin(); iterator != pathTree.end(); ++iterator)
            {
                tempDist = EdgeType(iterator->second, )
            }
        }
    }


private:

    //Builds a path between the given start/end nodes using the given "path tree" (a dictionary which
    //    associates each node key with the next node to travel to in order to get back to the start). 
    void BuildPath(NodeType start, NodeType end,
                   const std::unordered_map<NodeType, NodeType>& pathTree,
                   std::vector<NodeType> outPath) const
    {

    }
};