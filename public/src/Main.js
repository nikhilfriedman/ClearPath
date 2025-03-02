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
mapContainer.addIntersection(40.4193, -86.8936);

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


document.addEventListener("DOMContentLoaded", function () {
    Module.onRuntimeInitialized = function () {
        console.log("WASM Module Loaded");

        // Example Graph (Adjacency Matrix, Flattened)
        let numNodes = 5;
        let graph = [
            0, 10, 3,  0, 0,
            0,  0, 1,  2, 0,
            0,  4, 0,  8, 2,
            0,  0, 0,  0, 7,
            0,  0, 0,  9, 0
        ]; // 5x5 matrix (row-major)

        let startNode = 0;

        // Allocate memory in WASM heap
        let graphPtr = Module._malloc(graph.length * 4); // 4 bytes per int
        let outputPtr = Module._malloc(numNodes * 4);

        // Copy graph data to WASM heap
        Module.HEAP32.set(graph, graphPtr / 4);

        // Call WASM function
        let dijkstraFunction = Module.cwrap('dijkstra', null, ['number', 'number', 'number', 'number']);
        dijkstraFunction(graphPtr, numNodes, startNode, outputPtr);

        // Read output from WASM heap
        let result = new Int32Array(Module.HEAP32.buffer, outputPtr, numNodes);
        console.log("Shortest distances from node", startNode, ":", result);

        // Free allocated memory
        Module._free(graphPtr);
        Module._free(outputPtr);
    };
});
