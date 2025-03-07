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
sideBar.addIntersection("Main St & 1st Ave");
sideBar.addIntersection("Broadway & 5th Ave");

const mapContainer = new MapContainer(200, 40, 300);
console.log("Map container instance:", mapContainer);
console.log("Map instance inside MapContainer:", mapContainer.map);

// DO NOT CHANGE
// traffic_lights = [
//     [40.41912655975389, -86.89549348053828],
//     [40.4182405143039, -86.89549348053828],
//     [40.4191334282777, -86.8942755322012],
//     [40.41823364568895, -86.89426199944191],
//     [40.41913686253933, -86.89308013846299],
//     [40.418223342765195, -86.89309818214204],
//     [40.41916090236587, -86.89186670104569],
//     [40.41824738291817, -86.89183963552709]
// ];

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
