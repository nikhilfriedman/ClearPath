function createTriangle(x, y, w, h, color, direction = "up") {
    const triangle = document.createElement("div");
    Object.assign(triangle.style, {
        position: "absolute",
        zIndex: "9999",
        width: "0",
        height: "0",
        left: `${x}px`,
        top: `${y}px`,
        pointerEvents: "none",
    });

    switch (direction) {
        case "up":
            Object.assign(triangle.style, {
                borderLeft: `${w / 2}px solid transparent`,
                borderRight: `${w / 2}px solid transparent`,
                borderBottom: `${h}px solid ${color}`,
            });
            break;

        case "down":
            Object.assign(triangle.style, {
                borderLeft: `${w / 2}px solid transparent`,
                borderRight: `${w / 2}px solid transparent`,
                borderTop: `${h}px solid ${color}`,
            });
            break;

        case "left":
            Object.assign(triangle.style, {
                borderTop: `${h / 2}px solid transparent`,
                borderBottom: `${h / 2}px solid transparent`,
                borderRight: `${w}px solid ${color}`,
            });
            break;

        case "right":
            Object.assign(triangle.style, {
                borderTop: `${h / 2}px solid transparent`,
                borderBottom: `${h / 2}px solid transparent`,
                borderLeft: `${w}px solid ${color}`,
            });
            break;

        default:
            console.warn("Invalid direction. Use 'up', 'down', 'left', or 'right'.");
            return null;
    }

    document.body.appendChild(triangle);
    return triangle;
}

function setTriangleColor(triangle, width, color, direction) {
    switch (direction) {
        case "up":
            triangle.style.borderBottom = `${width}px solid ${color}`;
            break;
        case "down":
            triangle.style.borderTop = `${width}px solid ${color}`;
            break;
        case "left":
            triangle.style.borderRight = `${width}px solid ${color}`;
            break;
        case "right":
            triangle.style.borderLeft = `${width}px solid ${color}`;
            break;
        default:
            console.warn("Invalid direction. Use 'up', 'down', 'left', or 'right'.");
    }
}

class TrafficLight {
    constructor(lat, lng, id) {
        this.id = id;
        this.lat = lat;
        this.lng = lng;
        this.state = [traffic_green, traffic_green, traffic_green, traffic_green]; // One light per direction (N, E, S, W)

        console.log(this.id);

        this.elements = [];
        for (let i = 0; i < 4; i++) {
            const element = document.createElement("div");
            element.className = `traffic-light-${id}`;
            element.innerText = "";
            document.body.appendChild(element);
            Object.assign(element.style, {
                zIndex: "9999",
                position: "absolute",
                width: "20px",
                height: "20px",
                backgroundColor: this.state[i],
                borderRadius: "50%",
                pointerEvents: "none",
            });
            this.elements.push(element);
        }

        this.triangles = [];
        
        this.triangles.push(createTriangle(100, 100, 20, 10, "black", "down"));
        this.triangles.push(createTriangle(100, 200, 10, 20, "black", "left"));
        this.triangles.push(createTriangle(100, 300, 20, 10, "black", "up"));
        this.triangles.push(createTriangle(100, 400, 10, 20, "black", "right"));

        this.background = document.createElement("div");
        this.background.innerText = this.id; // Display the ID
        document.body.appendChild(this.background);
        Object.assign(this.background.style, {
            zIndex: "99999",
            position: "absolute",
            width: "40px",
            height: "40px",
            backgroundColor: "black",
            borderRadius: "50%",
            pointerEvents: "none",
            color: "white", // Ensure text is visible
            display: "flex",
            alignItems: "center",
            justifyContent: "center",
            fontSize: "14px",
            fontWeight: "bold",        
        });

        this.setLight(0, traffic_green);
        this.setLight(1, traffic_red);
        this.setLight(2, traffic_green);
        this.setLight(3, traffic_red);
    }

    updatePosition(x, y, zoom) {
        // Position lights in a square around the intersection
        const offsets = [
            { dx: 0, dy: 10 }, // North
            { dx: 10, dy: 0 },  // East
            { dx: 0, dy: -10 },   // South
            { dx: -10, dy: 0 },  // West
        ];
        this.elements.forEach((el, i) => {
            el.style.left = `${x + offsets[i].dx}px`;
            el.style.top = `${y + offsets[i].dy}px`;
        });

        const tri_offsets = [
            { dx: 0, dy: 0 }, // North
            { dx: 10 , dy: 0 },  // East
            { dx: 0, dy: 10  },   // South
            { dx: 0, dy: 0 },  // West
        ];
        this.triangles.forEach((tri, i) => {
            tri.style.left = `${x + tri_offsets[i].dx}px`;
            tri.style.top = `${y + tri_offsets[i].dy}px`;
        });

        this.background.style.left = `${x - 10}px`;
        this.background.style.top = `${y - 10}px`;

    }

    setLight(direction, color) {
        // direction: 0 (N), 1 (E), 2 (S), 3 (W)
        this.state[direction] = color;
        this.elements[direction].style.backgroundColor = color;

        let tri_direction;
        switch (direction) {
            case 0:
                tri_direction = "down";
                break;
            case 1:
                tri_direction = "left";
                break;
            case 2:
                tri_direction = "up";
                break;
            case 3:
                tri_direction = "right";
                break;
        }
        setTriangleColor(this.triangles[direction], 10, color, tri_direction);
    }
}
