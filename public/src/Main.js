// Set white background for now
document.body.style.backgroundColor = 'white';

// traffic colors
const traffic_green = '#4ff08d';
const traffic_yellow = '#f0db5b';
const traffic_red = '#ed665c';

let current_path = [];

// Initialize UI components
const topBar = new TopBar(0, 0);

const sideBar = new SideBar(0, 40, 300);

const mapContainer = new MapContainer(200, 40, 300);
console.log("Map container instance:", mapContainer);
console.log("Map instance inside MapContainer:", mapContainer.map);

// DO NOT CHANGE
traffic_lights = [
    [40.41912655975389, -86.89549348053828],
    [40.4182405143039, -86.89549348053828],
    [40.41912655975389, -86.8942755322012],
    [40.4182405143039, -86.8942755322012],
    [40.41912655975389, -86.89308013846299],
    [40.4182405143039, -86.89308013846299],
    [40.41912655975389, -86.89186670104569],
    [40.4182405143039, -86.89186670104569]
];


traffic_lights.forEach((element, index) => {
    console.log(index);
    mapContainer.addIntersection(element[0], element[1], index);
});

mapContainer.addVehicle(40.41732303838001, -86.89548276315818, "car");

const wsUrl = 'ws://172.20.10.7/';
const socket = new WebSocket(wsUrl);

// Called when the connection is established
socket.onopen = () => {
    console.log("WebSocket connection established with ESP32");
    // Example: send a message to the ESP32
    socket.send("Hello ESP32");
};

// Called when a message is received from the ESP32
socket.onmessage = (event) => {
    console.log("Message from ESP32:", event.data);
};

// Called on errors
socket.onerror = (error) => {
    console.error("WebSocket error:", error);
};

// Called when the connection is closed
socket.onclose = (event) => {
    console.log("WebSocket connection closed");
};
