let nextId = 0;

class MapContainer {
    constructor(x, y, width) {
        this.x = x;
        this.y = y;
        this.width = width;

        this.intersections = [];
        this.vehicles = [];

        this.startNode = null;
        this.endNode = null;
        this.currentPath = null;

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

        this.map.on("click", this.handleMapClick.bind(this));
        document.body.addEventListener("keydown", this.handleKeyEvent.bind(this));
    }

    handleKeyEvent(event) {
        const delta = 0.00005; // Amount to modify lat/lng
    
        console.log("Key Pressed: ", event.key);

        let vehicle = this.vehicles[0];

        switch (event.key) {
            case "w":
                vehicle.updateLatLng(delta, 0); // Increase latitude
                break;
            case 's':
                vehicle.updateLatLng(-delta, 0); // Decrease latitude
                break;
            case "a":
                vehicle.updateLatLng(0, -delta); // Decrease longitude
                break;
            case "d":
                vehicle.updateLatLng(0, delta); // Increase longitude
                break;
            default:
                console.log("def case");
                return; // Ignore other keys
        }
    
        const mapBounds = this.map.getContainer().getBoundingClientRect(); // Get map position on the page
        const latLngBounds = this.map.getBounds(); // Get lat/lng boundaries of the visible area
        const screenPos = this.map.latLngToContainerPoint([vehicle.lat, vehicle.lng]);
        const zoom = this.map.getZoom();

        if (latLngBounds.contains([vehicle.lat, vehicle.lng])) {
            vehicle.updatePosition(screenPos.x + mapBounds.left, screenPos.y + mapBounds.top, zoom);
            vehicle.background.style.display = "block";
        } else {
            vehicle.background.style.display = "none";
        }            


        this.vehicles[0].updatePosition()

        // Prevent default browser behavior (e.g., scrolling)
        event.preventDefault();
    }
    
    handleMapClick(event) {
        let latlng = event.latlng; // Get lat/lon from click event
        console.log("Clicked at:", latlng);

        if (this.currentPath != null) {
            this.map.removeLayer(this.currentPath);
            this.currentPath = null;
        }

        // Find the closest node
        let clickedNode = this.findClosestNode(latlng.lat, latlng.lng);

        if (clickedNode !== null) {
            if (this.startNode === null) {
                this.startNode = clickedNode;
                console.log(`Start node selected: ${clickedNode}`);
            } else if (this.endNode === null) {
                this.endNode = clickedNode;
                console.log(`End node selected: ${clickedNode}`);

                // Run Dijkstra and draw the path
                this.calculateAndDrawPath();
                
                // Reset for the next selection
                this.startNode = null;
                this.endNode = null;
            }
        }
    }

    findClosestNode(lat, lon) {
        let minDistance = Infinity;
        let closestNode = null;

        traffic_lights.forEach(([nodeLat, nodeLon], index) => {
            let distance = haversine(lat, lon, nodeLat, nodeLon);
            if (distance < minDistance) {
                minDistance = distance;
                closestNode = index;
            }
        });

        return closestNode;
    }

    addIntersection(lat, lng, id) {
        this.intersections.push(new TrafficLight(lat, lng, id));
        this.updateElements(); // Update positions
    }
    

    addVehicle(lat, lng, id) {
        this.vehicles.push(new Vehicle(lat, lng, id));
        this.updateElements();
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
    
        this.vehicles.forEach(vehicle => {
            const screenPos = this.map.latLngToContainerPoint([vehicle.lat, vehicle.lng]);
    
            if (latLngBounds.contains([vehicle.lat, vehicle.lng])) {
                vehicle.updatePosition(screenPos.x + mapBounds.left, screenPos.y + mapBounds.top, zoom);
                vehicle.background.style.display = "block";
            } else {
                vehicle.background.style.display = "none";
            }            
        });
    }

    drawPath(path) {
        if (!this.map) {
            console.error("Map instance is not available.");
            return;
        }

        if (!Array.isArray(path) || path.length < 2) {
            console.error("Invalid path array:", path);
            return;
        }

        // Convert node indices to lat/lon coordinates
        let coordinates = path.map(index => {
            if (index < 0 || index >= traffic_lights.length) {
                console.error(`Invalid node index: ${index}`);
                return null;
            }
            return traffic_lights[index]; // Convert index to lat/lon
        }).filter(coord => coord !== null); // Remove invalid nodes

        if (coordinates.length < 2) {
            console.error("Not enough valid coordinates to draw a path.");
            return;
        }

        console.log("Drawing path with coordinates:", coordinates);

        // Draw the polyline
        this.currentPath = L.polyline(coordinates, { color: 'blue', weight: 10 });

        // Add polyline to the correct map instance
        this.currentPath.addTo(this.map);
    }
    
    calculateAndDrawPath() {
        if (this.startNode === null || this.endNode === null) {
            console.error("Start or end node is not set.");
            return;
        }

        console.log(`Calculating path from ${this.startNode} to ${this.endNode}...`);

        let adjacencyMatrix = createAdjacencyMatrix(traffic_lights, 120);
        let flattenedGraph = flattenMatrix(adjacencyMatrix);
        let numNodes = adjacencyMatrix.length;

        current_path = runDijkstra(flattenedGraph, numNodes, this.startNode, this.endNode);

        console.log("Computed path:", current_path);

        if (Array.isArray(current_path) && current_path.length > 1) {
            this.drawPath(current_path);
        } else {
            console.error("Invalid path returned from Dijkstra:", current_path);
        }
    }


}

// // Example usage
// const mapContainer = new MapContainer(200, 40, window.innerWidth - 200, window.innerHeight - 40);
// mapContainer.addIntersection(40.4189, -86.8939);
// mapContainer.addVehicle(40.4190, -86.8940);
