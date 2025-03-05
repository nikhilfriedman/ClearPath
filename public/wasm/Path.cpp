#include <emscripten.h>
#include <vector>
#include <queue>
#include <limits>
#include <cstdio>

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void dijkstra(int* graph, int numNodes, int startNode, int* distances, int* previous) {
        int visited[numNodes];
        
        // Initialize distances and previous nodes
        for (int i = 0; i < numNodes; i++) {
            distances[i] = INT_MAX; // Infinity
            previous[i] = -1; // No previous node
            visited[i] = 0; // Not visited
        }
    
        distances[startNode] = 0;
    
        for (int i = 0; i < numNodes - 1; i++) {
            int minDistance = INT_MAX, minIndex = -1;
    
            // Find the nearest unvisited node
            for (int j = 0; j < numNodes; j++) {
                if (!visited[j] && distances[j] <= minDistance) {
                    minDistance = distances[j];
                    minIndex = j;
                }
            }
    
            if (minIndex == -1) break; // No more reachable nodes
            visited[minIndex] = 1;
    
            // Update distances to neighbors
            for (int j = 0; j < numNodes; j++) {
                int edgeWeight = graph[minIndex * numNodes + j];
                if (edgeWeight > 0 && !visited[j] && distances[minIndex] + edgeWeight < distances[j]) {
                    distances[j] = distances[minIndex] + edgeWeight;
                    previous[j] = minIndex; // Store the previous node
                }
            }
        }
    }
    



    // void dijkstra(int* graph, int numNodes, int startNode, int* output) {
    //     // Priority queue: (distance, node)
    //     std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> pq;

    //     // Distance array, initialized to "infinity"
    //     std::vector<int> dist(numNodes, std::numeric_limits<int>::max());

    //     // Start from source
    //     dist[startNode] = 0;
    //     pq.push({0, startNode});

    //     while (!pq.empty()) {
    //         int currDist = pq.top().first;
    //         int node = pq.top().second;
    //         pq.pop();

    //         if (currDist > dist[node]) continue; // Skip outdated entries

    //         for (int neighbor = 0; neighbor < numNodes; neighbor++) {
    //             int weight = graph[node * numNodes + neighbor]; // Access adjacency matrix

    //             if (weight > 0) { // Check if edge exists
    //                 int newDist = currDist + weight;
    //                 if (newDist < dist[neighbor]) {
    //                     dist[neighbor] = newDist;
    //                     pq.push({newDist, neighbor});
    //                 }
    //             }
    //         }
    //     }

    //     // Copy results to output array
    //     for (int i = 0; i < numNodes; i++) {
    //         output[i] = dist[i];
    //     }
    // }
}
