// contains logic for top bar of website

class TopBar {
    constructor(x, y) {
        this.x = x;
        this.y = y;

        // main top bar
        this.domElement = document.createElement('div');
        Object.assign(this.domElement.style, {
            position: 'fixed',
            left: `${this.x}px`,
            top: `${this.y}px`,
            height: '40px',
            width: '100%',
            background: 'white',
            borderBottom: '2px solid black',
            display: 'flex',
            alignItems: 'center', // Vertically center
            justifyContent: 'center' // Horizontally center        
        });
        document.body.appendChild(this.domElement);

        // ClearPath logo
        this.logo = document.createElement('img');
        this.logo.src = 'resources/logo.svg';
        this.logo.id = 'top-logo';
        this.domElement.appendChild(this.logo);
    }
}