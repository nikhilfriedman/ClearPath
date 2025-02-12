// Set white background for now
document.body.style.backgroundColor = 'white';

// Initialize UI components
const topBar = new TopBar(0, 0);
const sideBar = new SideBar(0, 40, 200);

// Wait until the DOM is fully loaded
document.addEventListener("DOMContentLoaded", function () {
    // Select elements
    const mapContainer = document.getElementById("map");

    // Apply styles dynamically using Object.assign()
    Object.assign(mapContainer.style, {
        height: `calc(100vh - 40px)`, // Full height minus the top bar
        width: `calc(100vw - 200px)`, // Full width minus the sidebar
        position: "absolute",
        top: "40px",
        left: "200px",
    });

40.428794434313645, -86.91199293989558
    // Initialize Leaflet Map
    const map = L.map('map').setView([40.4288, -86.9120], 20); // Default to NYC

    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
        attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
    }).addTo(map);
});
