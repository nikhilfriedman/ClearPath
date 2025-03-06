/*
    LOAD WASM FIRST !!
*/

let wasmReady = false;

document.addEventListener("DOMContentLoaded", function () {
    Module.onRuntimeInitialized = function () {
        console.log("WASM Module Loaded");
        wasmReady = true;
    };
});

function haversine(lat1, lon1, lat2, lon2) {
    const R = 6371e3; // Earth's radius in meters
    const toRad = x => x * Math.PI / 180;

    let dLat = toRad(lat2 - lat1);
    let dLon = toRad(lon2 - lon1);
    let a = Math.sin(dLat / 2) * Math.sin(dLat / 2) +
            Math.cos(toRad(lat1)) * Math.cos(toRad(lat2)) *
            Math.sin(dLon / 2) * Math.sin(dLon / 2);
    let c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));

    return R * c; // Distance in meters
}

function createAdjacencyMatrix(traffic_lights, maxDistance = 120) {
    let n = traffic_lights.length;
    let matrix = Array.from({ length: n }, () => Array(n).fill(Infinity));

    for (let i = 0; i < n; i++) {
        for (let j = 0; j < n; j++) {
            if (i === j) {
                matrix[i][j] = 0; // Distance to self is 0
            } else {
                let [lat1, lon1] = traffic_lights[i];
                let [lat2, lon2] = traffic_lights[j];
                let distance = haversine(lat1, lon1, lat2, lon2);

                if (distance <= maxDistance) {
                    matrix[i][j] = distance; // Only connect nearby lights
                }
            }
        }
    }
    return matrix;
}

function flattenMatrix(matrix) {
    return matrix.flat();
}

function runDijkstra(graph, numNodes, startNode, endNode) {
    if (!wasmReady) {
        console.warn("WASM is not ready yet!");
        return;
    }

    let graphSize = graph.length * 4;
    let outputSize = numNodes * 4;

    let graphPtr = Module._malloc(graphSize);
    let distPtr = Module._malloc(outputSize);
    let prevPtr = Module._malloc(outputSize); // New array to store previous nodes

    if (!graphPtr || !distPtr || !prevPtr) {
        console.error("Failed to allocate memory in WASM.");
        return;
    }

    // Copy adjacency matrix to WASM heap
    Module.HEAP32.set(graph, graphPtr / 4);

    // Get function reference from WASM
    let dijkstraFunction = Module.cwrap('dijkstra', null, ['number', 'number', 'number', 'number', 'number']);

    // Call the WASM function
    dijkstraFunction(graphPtr, numNodes, startNode, distPtr, prevPtr);

    // Read results from WASM memory
    let distances = new Int32Array(Module.HEAP32.buffer, distPtr, numNodes);
    let previous = new Int32Array(Module.HEAP32.buffer, prevPtr, numNodes);

    console.log("Shortest distances:", distances);
    console.log("Previous nodes:", previous);

    // Free allocated memory
    Module._free(graphPtr);
    Module._free(distPtr);
    Module._free(prevPtr);

    // Construct the shortest path from startNode to endNode
    return reconstructPath(previous, startNode, endNode);
}

function reconstructPath(previous, startNode, endNode) {
    let path = [];
    let currentNode = endNode;

    while (currentNode !== -1) {
        path.push(currentNode);
        currentNode = previous[currentNode]; // Move to the previous node
    }

    path.reverse(); // Reverse to get path from start → end

    if (path[0] !== startNode) {
        console.warn("No valid path found.");
        return [];
    }

    return path;
}
