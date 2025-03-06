
class Vehicle {
    constructor(lat, lng, id) {
        this.id = id;
        this.lat = lat;
        this.lng = lng;

        this.background = document.createElement("div");
        this.background.innerText = this.id; // Display the ID
        document.body.appendChild(this.background);
        Object.assign(this.background.style, {
            zIndex: "9998",
            position: "absolute",
            width: "40px",
            height: "40px",
            backgroundColor: "blue",
            borderRadius: "50%",
            pointerEvents: "none",
            color: "white", // Ensure text is visible
            display: "flex",
            alignItems: "center",
            justifyContent: "center",
            fontSize: "14px",
            fontWeight: "bold",        
        });
    }

    updateLatLng(d_lat, d_lng) {
        this.lat += d_lat;
        this.lng += d_lng;
    }

    updatePosition(x, y, zoom) {
        // Position lights in a square around the intersection

        this.background.style.left = `${x - 10}px`;
        this.background.style.top = `${y - 10}px`;
    }
}
