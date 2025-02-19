
class SideBar {
    constructor(x, y, width) {
        this.x = x;
        this.y = y;
        this.width = width;

        this.intersections = [];
        this.vehicles = [];

        // Create sidebar container
        this.domElement = document.createElement('div');
        Object.assign(this.domElement.style, {
            position: 'fixed',
            left: `${this.x}px`,
            top: `${this.y}px`,
            height: `calc(100vh - ${this.y}px)`,
            width: `${this.width}px`,
            background: 'white',
            borderRight: '2px solid black',
            overflow: 'hidden',
            display: 'flex',
            flexDirection: 'column'
        });
        document.body.appendChild(this.domElement);

        // Create resizable handle
        this.resizer = document.createElement("div");
        Object.assign(this.resizer.style, {
            width: "5px",
            height: "100%",
            position: "absolute",
            right: "0",
            zIndex: "9999",
            top: "0",
            cursor: "ew-resize",
            background: "none"
        });
        this.domElement.appendChild(this.resizer);
        this.makeResizable();

        // Create intersections container
        this.intersectionsContainer = document.createElement('div');
        this.intersectionsContainer.style.flex = "1";
        this.intersectionsContainer.style.overflowY = "auto";
        this.intersectionsContainer.style.padding = "10px";
        this.intersectionsContainer.innerHTML = "<h3>Intersections</h3>";
        this.domElement.appendChild(this.intersectionsContainer);

        // Create vehicles container
        this.vehiclesContainer = document.createElement('div');
        this.vehiclesContainer.style.flex = "1";
        this.vehiclesContainer.style.overflowY = "auto";
        this.vehiclesContainer.style.padding = "10px";
        this.vehiclesContainer.innerHTML = "<h3>Vehicles</h3>";
        this.domElement.appendChild(this.vehiclesContainer);
    }

    // Method to update the displayed intersections and vehicles
    update() {
        // Clear existing lists
        this.intersectionsContainer.innerHTML = "<h3>Intersections</h3>";
        this.vehiclesContainer.innerHTML = "<h3>Vehicles</h3>";

        // Render intersections
        this.intersections.forEach((intersection, index) => {
            const item = document.createElement("div");
            item.textContent = `Intersection ${index + 1}: ${intersection}`;
            item.style.borderBottom = "1px solid gray";
            item.style.padding = "5px";
            this.intersectionsContainer.appendChild(item);
        });

        // Render vehicles
        this.vehicles.forEach((vehicle, index) => {
            const item = document.createElement("div");
            item.textContent = `Vehicle ${index + 1}: ${vehicle}`;
            item.style.borderBottom = "1px solid gray";
            item.style.padding = "5px";
            this.vehiclesContainer.appendChild(item);
        });
    }

    // Method to add an intersection
    addIntersection(intersection) {
        this.intersections.push(intersection);
        this.update();
    }

    // Method to add a vehicle
    addVehicle(vehicle) {
        this.vehicles.push(vehicle);
        this.update();
    }

    // Make the sidebar resizable
    makeResizable() {
        let isResizing = false;
        let startX, startWidth;

        this.resizer.addEventListener("mousedown", (event) => {
            isResizing = true;
            startX = event.clientX;
            startWidth = this.domElement.offsetWidth;
            event.preventDefault();
        });

        document.addEventListener("mousemove", (event) => {
            if (!isResizing) return;
            let newWidth = startWidth + (event.clientX - startX);
            if (newWidth < 200) newWidth = 200; // Minimum width
            if (newWidth > 500) newWidth = 500; // Maximum width

            this.width = newWidth;
            this.domElement.style.width = `${newWidth}px`;

            const mapDOM = document.getElementById("map");
            // Apply styles dynamically using Object.assign()

            Object.assign(mapDOM.style, {
                width: `calc(100vw - ${this.width}px)`,
                left: `${this.width}px`
            });
            // mapContainer.style.width = `calc(100vw - ${this.width})`;

            mapContainer.updateElements();
        });

        document.addEventListener("mouseup", () => {
            isResizing = false;
        });
    }
}