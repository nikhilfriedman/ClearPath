#include <emscripten.h>
#include <vector>
#include <queue>
#include <limits>
#include <cstdio>

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void dijkstra(int* graph, int numNodes, int startNode, int* output) {
        // Priority queue: (distance, node)
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> pq;

        // Distance array, initialized to "infinity"
        std::vector<int> dist(numNodes, std::numeric_limits<int>::max());

        // Start from source
        dist[startNode] = 0;
        pq.push({0, startNode});

        while (!pq.empty()) {
            int currDist = pq.top().first;
            int node = pq.top().second;
            pq.pop();

            if (currDist > dist[node]) continue; // Skip outdated entries

            for (int neighbor = 0; neighbor < numNodes; neighbor++) {
                int weight = graph[node * numNodes + neighbor]; // Access adjacency matrix

                if (weight > 0) { // Check if edge exists
                    int newDist = currDist + weight;
                    if (newDist < dist[neighbor]) {
                        dist[neighbor] = newDist;
                        pq.push({newDist, neighbor});
                    }
                }
            }
        }

        // Copy results to output array
        for (int i = 0; i < numNodes; i++) {
            output[i] = dist[i];
        }
    }
}
