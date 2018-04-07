function main() {
    let surface = document.getElementById("surface");

    if (!surface) {
        console.log("CG | Error | Not able to get surface.");
        return;
    }

    console.log("CG | Info | Surface found.");
    console.log("CG | Info | Surface width:", surface.width, "surface height:", surface.height);

    let context = surface.getContext("2d");

    if (!context) {
        console.log("CG | Error | Not able to get context.");
        return;
    }

    console.log("CG | Info | 2D Context found.");

    // Fill the canvas with black color;
    context.fillStyle = "#000000";
    context.fillRect(0, 0, surface.width, surface.height);

    // Draw the text
    context.textAlign = "center";
    context.textBaseline = "middle";
    context.font = "48px sans-serif";
    context.fillStyle = "#ffffff"; // White color
    context.fillText("Hello World!!!", surface.width / 2, surface.height / 2);
}
