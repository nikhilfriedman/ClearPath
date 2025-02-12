// contains logic for side bar

class SideBar {
    constructor(x, y, width) {
        this.x = x;
        this.y = y;
        this.width = width;

        this.intersections = [];
        this.vehicles = [];

        this.domElement = document.createElement('div');
        Object.assign(this.domElement.style, {
            position: 'fixed',
            left: `${this.x}px`,
            top: `${this.y}px`,
            height: `calc(100vh - ${this.y}px)`,
            width: `${this.width}px`,
            background: 'white',
            borderRight: '2px solid black',
        });
        document.body.appendChild(this.domElement);

        this.intersectionsContainer = document.createElement('div');
        Object.assign(this.intersectionsContainer.style, {
            position: 'fixed',

            // TODO : add UI for intersections

        });

    }
}