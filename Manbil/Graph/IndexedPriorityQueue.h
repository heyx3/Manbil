#pragma once

#include <vector>
#include <assert.h>


//The item being stored in the queue.
//It is recommended to use a type with a trivial copy constructor.
template<typename T>
//A queue structure that automatically keeps its items sorted.
class IndexedPriorityQueue
{
public:

    struct ItemAndCost { T Item; float Cost; };


    //If "sortAscending" is true, the front of the queue will always contain the SMALLEST-cost item.
    IndexedPriorityQueue(bool sortAscending) : isAscending(sortAscending) { }


    //Returns whether the front of the queue contains the SMALLEST-cost item (as opposed to
    //    the LARGEST-cost item).
    bool IsAscending(void) const { return isAscending; }

    unsigned int GetSize(void) const { return items.size(); }

    const T* GetItems(void) const { return items.data(); }
    const float* GetCosts(void) const { return costs.data(); }


    //Adds the given item with the given associated cost to this queue.
    void Enqueue(const T& item, float cost)
    {
        for (unsigned int i = 0; i < items.size(); ++i)
        {
            if ((isAscending && cost > costs[i]) ||
                (!isAscending && cost < costs[i]))
            {
                items.insert(items.begin() + i, item);
                costs.insert(costs.begin() + i, cost);
                return;
            }
        }

        items.push_back(item);
        costs.push_back(cost);
    }
    //Gets the item at the front of the queue and removes it.
    ItemAndCost Dequeue(void)
    {
        assert(GetSize() > 0);

        ItemAndCost ret;
        ret.Item = items[items.size() - 1];
        ret.Cost = costs[costs.size() - 1];
        
        items.erase(items.begin() + (items.size() - 1));
        costs.erase(costs.begin() + (costs.size() - 1));

        return ret;
    }

    //Sets whether this queue should sort by ascending or descending order.
    void SetIsAscending(bool useAscending)
    {
        //If the value is actually changing, reverse the order of the items.
        if (useAscending != isAscending)
        {
            isAscending = useAscending;

            std::reverse(items.begin(), items.end());
            std::reverse(costs.begin(), costs.end());
        }
    }


private:

    bool isAscending;

    //The front of the queue is stored at the end of the vector for performance reasons.
    std::vector<T> items;

    std::vector<float> costs;
};