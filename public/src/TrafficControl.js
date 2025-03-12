// default traffic behavior
let time_int = 0;
let time_state = 0;

let next_to_delete = -1;

// this really needs to be broken up lmao
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
            let d_lat = Math.abs(mapContainer.vehicles[0].lat - element.lat);
            let d_lng = Math.abs(mapContainer.vehicles[0].lng - element.lng);

            let distance = Math.sqrt(d_lat ** 2 + d_lng ** 2);

            // NEAR ! change light
            if (distance < 0.00065) {
                if (d_lat > d_lng) {
                    if (element.state[0] == traffic_green || element.state[0] == traffic_yellow) {
                        // do nothing. we are already in desired state
                    } else {
                        // make yellow
                        element.setLight(1, traffic_yellow);
                        element.setLight(3, traffic_yellow);        
                    }
                } else {
                    if (element.state[0] == traffic_green || element.state[0] == traffic_yellow) {
                        // make yellow
                        element.setLight(0, traffic_yellow);
                        element.setLight(2, traffic_yellow);        
                    } else {
                        // do nothing. we are already in desired state
                        // TODO : clean up this logic
                    }    
                }

                // finally turn green when close enough
                if (distance < 0.00045) {
                    if (d_lat > d_lng) {
                        // N S
                        console.log("N S");
                        element.setLight(0, traffic_green);
                        element.setLight(1, traffic_red);
                        element.setLight(2, traffic_green);
                        element.setLight(3, traffic_red);
            
                    } else {
                        // E W
                        console.log("E W");
                        element.setLight(0, traffic_red);
                        element.setLight(1, traffic_green);
                        element.setLight(2, traffic_red);
                        element.setLight(3, traffic_green);
                    }
    
                }

                // remove if we got close enough
                if (distance < 0.00005) {
                    if (current_path.length > 1) {
                        if (next_to_delete != -1 && next_to_delete != element.id) {
                            current_path = current_path.filter(item => item !== next_to_delete);
                            console.log("removed: ", element.id);
                            console.log("current path: ", current_path);
                        }
    
                        next_to_delete = element.id;    
                    } else {
                        current_path = current_path.filter(item => item !== element.id);
                        console.log("removed: ", element.id);
                        console.log("current path: ", current_path);
                    }

                    // if we finished the path...
                    if (current_path.length == 0) {
                        if (mapContainer.transactions.length != 0) {
                            let next_path = mapContainer.transactions.pop();
                            sideBar.scheduled_paths.pop();
                            sideBar.setCurrentPath(next_path);
                            sideBar.update();

                            mapContainer.startNode = next_path[0];
                            mapContainer.endNode = next_path[1];
                            mapContainer.calculateAndDrawPath();
                        } else {
                            mapContainer.startNode = null;
                            mapContainer.endNode = null;
                            mapContainer.calculateAndDrawPath();

                            sideBar.setCurrentPath([]);
                            sideBar.update();
                        }
                    }

                    mapContainer.map.removeLayer(mapContainer.currentPath);
                    mapContainer.drawPath(current_path);
                }

                return;
            } 
        }

        // otherwise...
        // default traffic light behavior
        setDefaultTraffic(element);
    });
}, 10);

function setDefaultTraffic(element) {
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
}