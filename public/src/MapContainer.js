let nextId = 0;

class MapContainer {
    constructor(x, y, width) {
        this.x = x;
        this.y = y;
        this.width = width;

        this.intersections = [];
        this.vehicles = [];

        this.container = document.getElementById("map");
        Object.assign(this.container.style, {
            height: `calc(100vh - 40px)`, // Full height minus the top bar
            width: `calc(100vw - ${width}px)`, // Full width minus the sidebar
            position: "absolute",
            top: "40px",
            left: `${this.width}px`,
        });

        this.map = L.map('map').setView([40.4187, -86.8937], 17.5); // lafayette court house

        L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
            attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
        }).addTo(this.map);

        // Bind the update function correctly
        this.updateElements = this.updateElements.bind(this);
        this.map.on("move", this.updateElements);
        this.map.on("zoom", this.updateElements);
        this.updateElements(); 
    }

    addIntersection(lat, lng, id) {
        this.intersections.push(new TrafficLight(lat, lng, id));
    
        this.updateElements(); // Update positions
    }
    

    addVehicle(lat, lng) {
        const id = `vehicle-${nextId++}`;
        this.vehicles.push({lat: lat, lng: lng, id: id});

        const element = document.createElement("div");
        element.className = "map-vehicle";
        element.id = id;
        element.innerText = "V";
        document.body.appendChild(element);

        Object.assign(element.style, {
            zIndex: '9999',
            position: "absolute",
            width: "20px",
            height: "20px",
            backgroundColor: "blue",
            borderRadius: "50%",
            color: "white",
            textAlign: "center",
            lineHeight: "20px",
            fontSize: "14px",
            transform: "translate(-50%, -50%)",
            pointerEvents: "none",
        });

        this.updateElements(); // Ensure it's positioned immediately
    }

    updateElements() {
        const mapBounds = this.map.getContainer().getBoundingClientRect(); // Get map position on the page
        const latLngBounds = this.map.getBounds(); // Get lat/lng boundaries of the visible area
    
        const zoom = this.map.getZoom();

        this.intersections.forEach(trafficLight => {
            const screenPos = this.map.latLngToContainerPoint([trafficLight.lat, trafficLight.lng]);
    
            if (latLngBounds.contains([trafficLight.lat, trafficLight.lng])) {
                trafficLight.updatePosition(screenPos.x + mapBounds.left, screenPos.y + mapBounds.top, zoom);
                trafficLight.elements.forEach(el => el.style.display = "block");
                trafficLight.triangles.forEach(el => el.style.display = "block");
                trafficLight.background.style.display = "block";
            } else {
                trafficLight.elements.forEach(el => el.style.display = "none");
                trafficLight.triangles.forEach(el => el.style.display = "none");
                trafficLight.background.style.display = "none";
            }
        });
    
        this.vehicles.forEach(point => {
            const screenPos = this.map.latLngToContainerPoint([point.lat, point.lng]);
            const element = document.getElementById(point.id);
            
            if (element) {
                if (latLngBounds.contains([point.lat, point.lng])) {
                    element.style.left = `${screenPos.x + mapBounds.left}px`;
                    element.style.top = `${screenPos.y + mapBounds.top}px`;
                    element.style.display = "block"; // Ensure it's visible
                } else {
                    element.style.display = "none"; // Hide if out of bounds
                }
            }
        });
    }
    


}

// // Example usage
// const mapContainer = new MapContainer(200, 40, window.innerWidth - 200, window.innerHeight - 40);
// mapContainer.addIntersection(40.4189, -86.8939);
// mapContainer.addVehicle(40.4190, -86.8940);
