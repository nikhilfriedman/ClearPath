
class SideBar {
    constructor(x, y, width) {
        this.x = x;
        this.y = y;
        this.width = width;

        this.current_path = [];
        this.scheduled_paths = [];

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

        // Create current_path container
        this.currentPathContainer = document.createElement('div');
        this.currentPathContainer.style.flex = "1";
        this.currentPathContainer.style.overflowY = "auto";
        this.currentPathContainer.style.padding = "10px";
        this.currentPathContainer.innerHTML = "<h3>Current Path</h3>";
        this.domElement.appendChild(this.currentPathContainer);

        // Create vehicles container
        this.scheduledPathsContainer = document.createElement('div');
        this.scheduledPathsContainer.style.flex = "1";
        this.scheduledPathsContainer.style.overflowY = "auto";
        this.scheduledPathsContainer.style.padding = "10px";
        this.scheduledPathsContainer.innerHTML = "<h3>Scheduled Paths</h3>";
        this.domElement.appendChild(this.scheduledPathsContainer);
    }

    // Method to update the displayed current_path and vehicles
    update() {
        // Clear existing lists
        this.currentPathContainer.innerHTML = "<h3>Current Path</h3>";
        this.scheduledPathsContainer.innerHTML = "<h3>Scheduled Paths</h3>";

        // Render current_path
        // this.current_path.forEach((intersection, index) => {
        //     const item = document.createElement("div");
        //     item.textContent = `Intersection ${index + 1}: ${intersection}`;
        //     item.style.borderBottom = "1px solid gray";
        //     item.style.padding = "5px";
        //     this.currentPathContainer.appendChild(item);
        // });

        const path_item = document.createElement("div");
        path_item.textContent = `${this.current_path[0]} -> ${this.current_path[1]}`;
        path_item.style.borderBottom = "1px solid gray";
        path_item.style.padding = "5px";
        this.currentPathContainer.appendChild(path_item);

        // Render vehicles
        this.scheduled_paths.forEach(path => {
            const item = document.createElement("div");
            item.textContent = `${path[0]} -> ${path[1]}`;
            item.style.borderBottom = "1px solid gray";
            item.style.padding = "5px";
            this.scheduledPathsContainer.appendChild(item);
        });
    }

    // Method to add an intersection
    setCurrentPath(path) {
        this.current_path = path;
        this.update();
    }

    // Method to add a vehicle
    addPath(path) {
        this.scheduled_paths.push(path);
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