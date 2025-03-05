// Set white background for now
document.body.style.backgroundColor = 'white';

const traffic_green = '#4ff08d';
const traffic_yellow = '#f0db5b';
const traffic_red = '#ed665c';

// Initialize UI components
const topBar = new TopBar(0, 0);

const sideBar = new SideBar(0, 40, 300);
sideBar.addIntersection("Main St & 1st Ave");
sideBar.addIntersection("Broadway & 5th Ave");
sideBar.addVehicle("Ambulance");
sideBar.addVehicle("Firetruck");

const mapContainer = new MapContainer(200, 40, 300);

// DO NOT CHANGE
traffic_lights = [
    [40.41912655975389, -86.89549348053828],
    [40.4182405143039, -86.89549348053828],
    [40.4191334282777, -86.8942755322012],
    [40.41823364568895, -86.89426199944191],
    [40.41913686253933, -86.89308013846299],
    [40.418223342765195, -86.89309818214204],
    [40.41916090236587, -86.89186670104569],
    [40.41824738291817, -86.89183963552709]
];

traffic_lights.forEach((element, index) => {
    console.log(index);
    mapContainer.addIntersection(element[0], element[1], index);
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

let adjacencyMatrix = createAdjacencyMatrix(traffic_lights);
console.log(adjacencyMatrix);

function flattenMatrix(matrix) {
    return matrix.flat();
}

let flattenedGraph = flattenMatrix(adjacencyMatrix);
let numNodes = adjacencyMatrix.length;

console.log(flattenedGraph);

let wasmReady = false;

document.addEventListener("DOMContentLoaded", function () {
    Module.onRuntimeInitialized = function () {
        console.log("WASM Module Loaded");
        wasmReady = true;
    };
});

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

document.addEventListener("mousedown", function () {
    if (!wasmReady) {
        console.warn("WASM is not ready yet!");
        return;
    }

    console.log("Mouse down detected. Running Dijkstra...");

    let adjacencyMatrix = createAdjacencyMatrix(traffic_lights, 120);
    let flattenedGraph = flattenMatrix(adjacencyMatrix);
    let numNodes = adjacencyMatrix.length;

    // Example: Find the shortest path from node 0 to node 6
    // let path = runDijkstra(flattenedGraph, numNodes, 0, 4);

    let path;
    for (i = 0; i <= 7; i++) {
        for (j = 0; j <= 7; j++) {
            path = runDijkstra(flattenedGraph, numNodes, i, j);
            console.log(i, " -> ", j, ": ", path);
        }
    }

    console.log("Shortest path from 0 to 6:", path);
});


/*
    WEB SOCKET
*/




// mapContainer.intersections[0].setLight(3, traffic_green);


// document.addEventListener("DOMContentLoaded", function () {
//     // Ensure Module is ready before calling test()
//     Module.onRuntimeInitialized = function () {
//         console.log("WASM Module Loaded");

//         // Call the test function
//         let testFunction = Module.cwrap('test', 'number', []);
//         // let result = testFunction();
//         // console.log("test() returned:", result);

//         testFunction();
//     };
// });


// document.addEventListener("DOMContentLoaded", function () {
//     Module.onRuntimeInitialized = function () {
//         console.log("WASM Module Loaded");

//         // Example Graph (Adjacency Matrix, Flattened)
//         let numNodes = 5;
//         let graph = [
//             0, 10, 3,  0, 0,
//             0,  0, 1,  2, 0,
//             0,  4, 0,  8, 2,
//             0,  0, 0,  0, 7,
//             0,  0, 0,  9, 0
//         ]; // 5x5 matrix (row-major)

//         let startNode = 0;

//         // Allocate memory in WASM heap
//         let graphPtr = Module._malloc(graph.length * 4); // 4 bytes per int
//         let outputPtr = Module._malloc(numNodes * 4);

//         // Copy graph data to WASM heap
//         Module.HEAP32.set(graph, graphPtr / 4);

//         // Call WASM function
//         let dijkstraFunction = Module.cwrap('dijkstra', null, ['number', 'number', 'number', 'number']);
//         dijkstraFunction(graphPtr, numNodes, startNode, outputPtr);

//         // Read output from WASM heap
//         let result = new Int32Array(Module.HEAP32.buffer, outputPtr, numNodes);
//         console.log("Shortest distances from node", startNode, ":", result);

//         // Free allocated memory
//         Module._free(graphPtr);
//         Module._free(outputPtr);
//     };
// });
