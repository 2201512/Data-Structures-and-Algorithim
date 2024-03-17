#include "ALGraph.h"
#include <vector>
#include <queue>
#include <limits>
#include <stdexcept>
#include <list>
#include <algorithm>

// Constructor
ALGraph::ALGraph(unsigned size) : size_(size), adjList_(size) {
    // Initialize the adjacency list
    for (unsigned i = 0; i < size; ++i) {
        adjList_[i].clear();
    }
}

// Destructor
ALGraph::~ALGraph() {
    // No dynamic memory allocation, so nothing specific to clean up
}

void ALGraph::addDEdge(unsigned source, unsigned destination, unsigned weight) {
    // Check if the source and destination vertices are within the valid range (1..size)
    if (source > 0 && destination > 0 && source <= size_ && destination <= size_) {
        // Create an AdjInfo object for the destination vertex and the specified weight
        AdjInfo edge{destination, weight};
        // Find the position to insert the edge in sorted order by weight
        auto insertPosition = adjList_[source - 1].begin();
        while (insertPosition != adjList_[source - 1].end() && insertPosition->weight <= weight) {
            ++insertPosition;
        }
        // Insert the edge into the source vertex's adjacency list at the calculated position
        adjList_[source - 1].insert(insertPosition, edge);
    } 
}


void ALGraph::addUEdge(unsigned node1, unsigned node2, unsigned weight) {
    if (node1 != node2) {
        addDEdge(node1, node2, weight);
        addDEdge(node2, node1, weight);
    }
}



std::vector<DijkstraInfo> ALGraph::dijkstra(unsigned start) const {
    // Initialize DijkstraInfo for all nodes
    std::vector<DijkstraInfo> dijkstraInfoList(size_);
    for (unsigned i = 0; i < size_; ++i) {
        dijkstraInfoList[i].cost = std::numeric_limits<unsigned>::max(); // Set initial cost to infinity
        dijkstraInfoList[i].path.clear(); // Clear path
    }

    // Create a priority queue to select the node with the lowest cost
    std::priority_queue<std::pair<unsigned, unsigned>> pq; // {cost, node}

    // Initialize the start node
    dijkstraInfoList[start - 1].cost = 0;
    dijkstraInfoList[start - 1].path.push_back(start);
    pq.push({0, start});

    while (!pq.empty()) {
        unsigned currentNode = pq.top().second;
        pq.pop();

        // Iterate through neighbors of currentNode
        for (const AdjInfo &neighbor : adjList_[currentNode - 1]) {
            unsigned newCost = dijkstraInfoList[currentNode - 1].cost + neighbor.weight;

            if (newCost < dijkstraInfoList[neighbor.id - 1].cost) {
                // Update cost and path for the neighbor
                dijkstraInfoList[neighbor.id - 1].cost = newCost;
                dijkstraInfoList[neighbor.id - 1].path = dijkstraInfoList[currentNode - 1].path;
                dijkstraInfoList[neighbor.id - 1].path.push_back(neighbor.id);

                // Add the neighbor to the priority queue
                pq.push({newCost, neighbor.id});
            }
        }
    }

    return dijkstraInfoList;
}

// Get the adjacency list
AdjList ALGraph::getAdjList() const {
    return adjList_;
}
