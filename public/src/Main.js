// Set white background for now
document.body.style.backgroundColor = 'white';

// Initialize UI components
const topBar = new TopBar(0, 0);

const sideBar = new SideBar(0, 40, 300);
sideBar.addIntersection("Main St & 1st Ave");
sideBar.addIntersection("Broadway & 5th Ave");
sideBar.addVehicle("Ambulance");
sideBar.addVehicle("Firetruck");

const mapContainer = new MapContainer(200, 40, 300);
mapContainer.addIntersection(40.4193, -86.8936);