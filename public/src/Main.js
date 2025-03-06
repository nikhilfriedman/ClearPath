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
sideBar.addVehicle("Ambulance");
sideBar.addVehicle("Firetruck");

const mapContainer = new MapContainer(200, 40, 300);
console.log("Map container instance:", mapContainer);
console.log("Map instance inside MapContainer:", mapContainer.map);

// DO NOT CHANGE
traffic_lights = [
    [40.41912655975389, -86.89549348053828],
    [40.4182405143039, -86.89549348053828],
    [40.4191334282777, -86.8942755322012],
    [40.41823364568895, -86.89426199944191],
    [40.41913686253933, -86.89308013846299],
    [40.418223342765195, -86.89309818214204],
    [40.41916090236587, -86.89186670104569],
    [40.41824738291817, -86.89183963552709]
];

traffic_lights.forEach((element, index) => {
    console.log(index);
    mapContainer.addIntersection(element[0], element[1], index);
});

mapContainer.addVehicle(40.41732303838001, -86.89548276315818, "car");

// default traffic behavior
let time_int = 0;
let time_state = 0;

setInterval(() => {
    time_int++;

    if (time_int % 300 == 0) {
        time_state++;
        if (time_state > 3) {
            time_state = 0;
        }
    }

    let lights = mapContainer.intersections;

    lights.forEach(element => {
        // skip lights that are being modified by algo
        if (current_path.includes(element.id)) {
            // TODO : set to value based on traffic light
            let d_lat = mapContainer.vehicles[0].lat - element.lat;
            let d_lng = mapContainer.vehicles[0].lng - element.lng;

            let distance = Math.sqrt(d_lat ** 2 + d_lng ** 2);

            // NEAR ! change light
            if (distance < 0.00045) {
                
                if (d_lat < d_lng) {
                    // N S

                    element.setLight(0, "white");
                    element.setLight(1, "white");
                    element.setLight(2, "white");
                    element.setLight(3, "white");
    
                } else {
                    // E W

                    element.setLight(0, "black");
                    element.setLight(1, "black");
                    element.setLight(2, "black");
                    element.setLight(3, "black");
    
                }

                if (distance < 0.00005) {
                    current_path = current_path.filter(item => item !== element.id);
                    console.log("removed: ", element.id);
                    console.log("current path: ", current_path);

                    // TODO : MAKE THIS A QUEUE !!!!

                    mapContainer.map.removeLayer(mapContainer.currentPath);
                    mapContainer.drawPath(current_path);
                }

                return;

            } 

            // console.log(distance);

            // element.setLight(0, "black");
            // element.setLight(1, "black");
            // element.setLight(2, "black");
            // element.setLight(3, "black");
        }

        switch (time_state) {
            case 0:
                element.setLight(0, traffic_red);
                element.setLight(1, traffic_green);
                element.setLight(2, traffic_red);
                element.setLight(3, traffic_green);
                break;
            case 1:
                element.setLight(0, traffic_red);
                element.setLight(1, traffic_yellow);
                element.setLight(2, traffic_red);
                element.setLight(3, traffic_yellow);
                break;
            case 2:
                element.setLight(0, traffic_green);
                element.setLight(1, traffic_red);
                element.setLight(2, traffic_green);
                element.setLight(3, traffic_red);
                break;
            case 3:
                element.setLight(0, traffic_yellow);
                element.setLight(1, traffic_red);
                element.setLight(2, traffic_yellow);
                element.setLight(3, traffic_red);
                break;
            default:
                element.setLight(0, "black");
                element.setLight(1, "black");
                element.setLight(2, "black");
                element.setLight(3, "black");

                console.log("invalid time state: ", time_state);
                break;    
        }
    });
}, 10);

