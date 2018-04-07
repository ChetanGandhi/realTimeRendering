let surface = null;
let context = null;

function main() {
    surface = document.getElementById("surface");

    if (!surface) {
        console.log("CG | Error | Not able to get surface.");
        return;
    }

    console.log("CG | Info | Surface found.");
    console.log("CG | Info | Surface width:", surface.width, "surface height:", surface.height);

    context = surface.getContext("2d");

    if (!context) {
        console.log("CG | Error | Not able to get context.");
        return;
    }

    console.log("CG | Info | 2D Context found.");

    // Fill the canvas with black color
    context.fillStyle = "#000000";
    context.fillRect(0, 0, surface.width, surface.height);

    // Draw the text
    drawText("Hello World!!!");

    // Register keyboard and mouse event handlers
    window.addEventListener("keydown", onKeyDown, false);
    window.addEventListener("click", onMouseDown, false);
}

function onKeyDown(event) {
    switch (event.key) {
        case "F":
        case "f":
            toggleFullscreen();
            drawText("Hello World!!!");
            break;
    }
}

function onMouseDown(event) {}

function drawText(text) {
    context.textAlign = "center";
    context.textBaseline = "middle";
    context.font = "48px sans-serif";
    context.fillStyle = "#ffffff"; // White color
    context.fillText(text, surface.width / 2, surface.height / 2);
}

function toggleFullscreen() {
    let fullscreenElement =
        document.fullscreenElement || document.webkitFullscreenElement || document.mozFullScreenElement || document.msFullscreenElement || null;

    if (!fullscreenElement) {
        if (surface.requestFullscreen) {
            surface.requestFullscreen();
        } else if (surface.mozRequestFullScreen) {
            surface.mozRequestFullScreen();
        } else if (surface.webkitRequestFullscreen) {
            surface.webkitRequestFullscreen();
        } else if (surface.msRequestFullscreen) {
            surface.msRequestFullscreen();
        }
    } else {
        if (document.exitFullscreen) {
            document.exitFullscreen();
        } else if (document.mozCancelFullScreen) {
            document.mozCancelFullScreen();
        } else if (document.webkitExitFullscreen) {
            document.webkitExitFullscreen();
        } else if (document.msExitFullscreen) {
            document.msExitFullscreen();
        }
    }
}
