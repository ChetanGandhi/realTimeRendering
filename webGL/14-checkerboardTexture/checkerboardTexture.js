let surface = null;
let gl = null;
let isEscapeKeyPressed = false;
let isFullscreen = false;
let surfaceOriginalWidth = 1;
let surfaceOriginalHeight = 1;
let displayAnimationRequestId = 0;
let requestAnimationFrame =
    window.requestAnimationFrame ||
    window.webkitRequestAnimationFrame ||
    window.mozRequestAnimationFrame ||
    window.oRequestAnimationFrame ||
    window.msRequestAnimationFrame;

let cancelAnimationFrame =
    window.cancelAnimationFrame ||
    window.webkitCancelRequestAnimationFrame ||
    window.webkitCancelAnimationFrame ||
    window.mozCancelRequestAnimationFrame ||
    window.mozCancelAnimationFrame ||
    window.oCancelRequestAnimationFrame ||
    window.oCancelAnimationFrame ||
    window.msCancelRequestAnimationFrame ||
    window.msCancelAnimationFrame;

const checkImageWidth = 64;
const checkImageHeight = 64;

const WebGLMacros = {
    CG_ATTRIBUTE_VERTEX: 0,
    CG_ATTRIBUTE_COLOR: 1,
    CG_ATTRIBUTE_NORMAL: 2,
    CG_ATTRIBUTE_TEXTURE0: 3
};

let vertexShaderObject = null;
let fragmentShaderObject = null;
let shaderProgramObject = null;

let vaoSquare = null;
let vboSquarePosition = null;
let vboSquareTexture = null;
let mvpUniform = null;
let textureSamplerUniform = null;
let textureCheckerboard = null;

let checkImage = null;

let perspectiveProjectionMatrix = null;

function main() {
    surface = document.getElementById("surface");

    if (!surface) {
        console.log("CG | Error | Not able to get surface.");
        return;
    }

    console.log("CG | Info | Surface found.");
    console.log("CG | Info | Surface width:", surface.width, "surface height:", surface.height);

    surfaceOriginalWidth = surface.width;
    surfaceOriginalHeight = surface.height;

    window.addEventListener("keydown", onKeyDown, false);
    window.addEventListener("click", onMouseDown, false);
    window.addEventListener("resize", onResize, false);

    initialize();
    display();
}

function onKeyDown(event) {
    if (event.repeat) {
        return;
    }

    switch (event.key) {
        case "F":
        case "f":
            toggleFullscreen();
            break;

        case "Escape":
            if (!isEscapeKeyPressed) {
                isEscapeKeyPressed = true;
                cleanUp();
                alert("Rendering is stopped.");
            }
            break;

        default:
            break;
    }
}

function onMouseDown(event) {}

function onResize(event) {
    if (isFullscreen) {
        resize(window.innerWidth, window.innerHeight);
    } else {
        resize(surfaceOriginalWidth, surfaceOriginalHeight);
    }
}

function initialize() {
    gl = surface.getContext("webgl2");

    if (!gl) {
        console.log("CG | Error | Not able to get WebGL-2 context.");
        return;
    }

    console.log("CG | Info | WebGL2 context found.");

    gl.viewportWidth = surface.width;
    gl.viewportHeight = surface.height;

    initializeVertexShaderObject();
    initializeFragmentShaderObject();
    initializeShaderProgramObject();
    initializeSquareBuffers();

    perspectiveProjectionMatrix = mat4.create();

    loadCheckerboardTexture();

    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    resize(surfaceOriginalWidth, surfaceOriginalHeight);
}

function initializeVertexShaderObject() {
    let vertexShaderCode =
        "#version 300 es" +
        "\n" +
        "in vec4 vertexPosition;" +
        "in vec2 vertexTextureCoordinate0;" +
        "\n" +
        "out vec2 outVertexTextureCoordinate0;" +
        "\n" +
        "uniform mat4 mvpMatrix;" +
        "\n" +
        "void main(void)" +
        "{" +
        "   gl_Position = mvpMatrix * vertexPosition;" +
        "   outVertexTextureCoordinate0 = vertexTextureCoordinate0;" +
        "}";

    vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject, vertexShaderCode);
    gl.compileShader(vertexShaderObject);

    if (!gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS)) {
        let error = gl.getShaderInfoLog(vertexShaderObject);

        if (error.length > 0) {
            alert("Vertex Shader Error: " + error);
            cleanUp();
        }
    }
}

function initializeFragmentShaderObject() {
    let fragmentShaderCode =
        "#version 300 es" +
        "\n" +
        "precision highp float;" +
        "\n" +
        "in vec2 outVertexTextureCoordinate0;" +
        "\n" +
        "out vec4 fragmentColor;" +
        "\n" +
        "uniform sampler2D textureSampler0;" +
        "\n" +
        "void main(void)" +
        "{" +
        "   fragmentColor = texture(textureSampler0, outVertexTextureCoordinate0);" +
        "}";

    fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject, fragmentShaderCode);
    gl.compileShader(fragmentShaderObject);

    if (!gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS)) {
        let error = gl.getShaderInfoLog(fragmentShaderObject);

        if (error.length > 0) {
            alert("Fragment Shader Error:" + error);
            cleanUp();
        }
    }
}

function initializeShaderProgramObject() {
    shaderProgramObject = gl.createProgram();
    gl.attachShader(shaderProgramObject, vertexShaderObject);
    gl.attachShader(shaderProgramObject, fragmentShaderObject);
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.CG_ATTRIBUTE_VERTEX, "vertexPosition");
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.CG_ATTRIBUTE_TEXTURE0, "vertexTextureCoordinate0");
    gl.linkProgram(shaderProgramObject);

    if (!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)) {
        let error = gl.getProgramInfoLog(shaderProgramObject);

        if (error.length > 0) {
            alert(error);
            cleanUp();
        }
    }

    mvpUniform = gl.getUniformLocation(shaderProgramObject, "mvpMatrix");
    textureSamplerUniform = gl.getUniformLocation(shaderProgramObject, "textureSampler0");
}

function initializeSquareBuffers() {
    // prettier-ignore
    let squareTextureCoordinates = new Float32Array([
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0
    ]);

    vaoSquare = gl.createVertexArray();
    gl.bindVertexArray(vaoSquare);

    vboSquarePosition = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboSquarePosition);
    gl.bufferData(gl.ARRAY_BUFFER, [], gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.CG_ATTRIBUTE_VERTEX, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.CG_ATTRIBUTE_VERTEX);

    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    vboSquareTexture = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboSquareTexture);
    gl.bufferData(gl.ARRAY_BUFFER, squareTextureCoordinates, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.CG_ATTRIBUTE_TEXTURE0, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.CG_ATTRIBUTE_TEXTURE0);

    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);
}

function display() {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.useProgram(shaderProgramObject);

    // Translation is same for both the shape, hence we are doing it in display.
    let modelViewMatrix = mat4.create();
    let modelViewProjectionMatrix = mat4.create();

    mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.6]);

    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
    gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

    drawStraightSquare();
    drawTiltedSquare();

    gl.useProgram(null);

    displayAnimationRequestId = requestAnimationFrame(display);
}

function drawStraightSquare() {
    gl.bindVertexArray(vaoSquare);

    // As we are using dynamic draw, pass the coordinates here as per texture mode.
    // prettier-ignore
    let squareVertices = new Float32Array([
        0.0, 1.0, 0.0,
        -2.0, 1.0, 0.0,
        -2.0, -1.0, 0.0,
        0.0, -1.0, 0.0
    ]);

    gl.bindBuffer(gl.ARRAY_BUFFER, vboSquarePosition);
    gl.bufferData(gl.ARRAY_BUFFER, squareVertices, gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, textureCheckerboard);
    gl.uniform1i(textureSamplerUniform, 0);
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

    gl.bindVertexArray(null);
}

function drawTiltedSquare() {
    gl.bindVertexArray(vaoSquare);

    // As we are using dynamic draw, pass the coordinates here as per texture mode.
    // prettier-ignore
    let tiltedSquareVertices = new Float32Array([
        2.41421, 1.0, -1.41421,
        1.0, 1.0, 0.0,
        1.0, -1.0, 0.0,
        2.41421, -1.0, -1.41421
    ]);

    gl.bindBuffer(gl.ARRAY_BUFFER, vboSquarePosition);
    gl.bufferData(gl.ARRAY_BUFFER, tiltedSquareVertices, gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, textureCheckerboard);
    gl.uniform1i(textureSamplerUniform, 0);
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

    gl.bindVertexArray(null);
}

function loadCheckerboardTexture() {
    makeCheckImage();

    let imageData = new ImageData(checkImage, checkImageWidth, checkImageHeight);

    textureCheckerboard = gl.createTexture();

    gl.bindTexture(gl.TEXTURE_2D, textureCheckerboard);
    gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, checkImageWidth, checkImageHeight, 0, gl.RGBA, gl.UNSIGNED_BYTE, imageData);
    gl.generateMipmap(gl.TEXTURE_2D);
    gl.bindTexture(gl.TEXTURE_2D, null);
}

function makeCheckImage() {
    // Multiple by 4 as we need to specify RGBA component value for single pixel
    let colorData = new Array(checkImageWidth * checkImageHeight * 4);

    for (let heightCounter = 0; heightCounter < checkImageHeight; ++heightCounter) {
        for (let widthCounter = 0; widthCounter < checkImageWidth; ++widthCounter) {
            for (let componentCounter = 0; componentCounter < 4; ++componentCounter) {
                let useWhiteColor = ((heightCounter & 0x8) ^ (widthCounter & 0x8)) === 0 || componentCounter === 3;
                colorData[4 * heightCounter * checkImageHeight + 4 * widthCounter + componentCounter] = 255 * useWhiteColor;
            }
        }
    }

    checkImage = new Uint8ClampedArray(colorData);
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

        isFullscreen = true;
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

        isFullscreen = false;
    }
}

function resize(width, height) {
    if (height == 0) {
        height = 1;
    }

    surface.width = width;
    surface.height = height;

    gl.viewport(0, 0, width, height);
    mat4.perspective(perspectiveProjectionMatrix, degreeToRadian(60), width / height, 1.0, 30.0);
}

function cleanUp() {
    cancelAnimationFrame(displayAnimationRequestId);

    if (vaoSquare) {
        gl.deleteVertexArray(vaoSquare);
        vaoSquare = null;
    }

    if (vboSquarePosition) {
        gl.deleteBuffer(vboSquarePosition);
        vboSquarePosition = null;
    }

    if (vboSquareTexture) {
        gl.deleteBuffer(vboSquareTexture);
        vboSquareTexture = null;
    }

    if (shaderProgramObject) {
        if (vertexShaderObject) {
            gl.detachShader(shaderProgramObject, vertexShaderObject);
        }

        if (fragmentShaderObject) {
            gl.detachShader(shaderProgramObject, fragmentShaderObject);
        }
    }

    if (vertexShaderObject) {
        gl.deleteShader(vertexShaderObject);
        vertexShaderObject = null;
    }

    if (fragmentShaderObject) {
        gl.deleteShader(fragmentShaderObject);
        fragmentShaderObject = null;
    }

    if (shaderProgramObject) {
        gl.deleteProgram(shaderProgramObject);
        shaderProgramObject = null;
    }

    gl.useProgram(null);

    if (textureCheckerboard) {
        gl.deleteTexture(textureCheckerboard);
        textureCheckerboard = null;
    }
}

function degreeToRadian(degree) {
    return degree * Math.PI / 180.0;
}
