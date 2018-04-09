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

const WebGLMacros = {
    CG_ATTRIBUTE_VERTEX: 0,
    CG_ATTRIBUTE_COLOR: 1,
    CG_ATTRIBUTE_NORMAL: 2,
    CG_ATTRIBUTE_TEXTURE0: 3
};

let vertexShaderObject = null;
let fragmentShaderObject = null;
let shaderProgramObject = null;

let vaoTriangle = null;
let vboTriangle = null;
let mvpUniform = null;

let orthographicProjectionMatrix = null;

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
    switch (event.key) {
        case "F":
        case "f":
            if (!event.repeat) {
                toggleFullscreen();
            }
            break;

        case "Escape":
            if (!event.repeat && !isEscapeKeyPressed) {
                isEscapeKeyPressed = true;
                cleanUp();
                alert("Rendering is stopped.");
            }
            break;

        default:
            console.log("Keydown not handled:", event.key);
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

    console.log("CG | Info | 2D gl found.");

    gl.viewportWidth = surface.width;
    gl.viewportHeight = surface.height;

    initializeVertexShaderObject();
    initializeFragmentShaderObject();
    initializeShaderProgramObject();
    initializeTriangleBuffers();

    orthographicProjectionMatrix = mat4.create();

    gl.clearColor(0.0, 0.0, 1.0, 1.0);

    resize(surfaceOriginalWidth, surfaceOriginalHeight);
}

function initializeVertexShaderObject() {
    let vertexShaderCode =
        "#version 300 es" +
        "\n" +
        "in vec4 vertexPosition;" +
        "uniform mat4 mvpMatrix;" +
        "\n" +
        "void main(void)" +
        "{" +
        "   gl_Position = mvpMatrix * vertexPosition;" +
        "}";

    vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject, vertexShaderCode);
    gl.compileShader(vertexShaderObject);

    if (!gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS)) {
        let error = gl.getShaderInfoLog(vertexShaderObject);

        if (error.length > 0) {
            alert(error);
            cleanUp();
        }
    }
}

function initializeFragmentShaderObject() {
    let fragmentShaderCode =
        "#version 300 es" +
        "\n" +
        "precision highp float;" +
        "out vec4 fragmentColor;" +
        "\n" +
        "void main(void)" +
        "{" +
        "   fragmentColor = vec4(1.0, 1.0, 1.0, 1.0);" +
        "}";

    fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject, fragmentShaderCode);
    gl.compileShader(fragmentShaderObject);

    if (!gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS)) {
        let error = gl.getShaderInfoLog(fragmentShaderObject);

        if (error.length > 0) {
            alert(error);
            cleanUp();
        }
    }
}

function initializeShaderProgramObject() {
    shaderProgramObject = gl.createProgram();
    gl.attachShader(shaderProgramObject, vertexShaderObject);
    gl.attachShader(shaderProgramObject, fragmentShaderObject);
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.CG_ATTRIBUTE_VERTEX, "vertexPosition");
    gl.linkProgram(shaderProgramObject);

    if (!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)) {
        let error = gl.getProgramInfoLog(shaderProgramObject);

        if (error.length > 0) {
            alert(error);
            cleanUp();
        }
    }

    mvpUniform = gl.getUniformLocation(shaderProgramObject, "mvpMatrix");
}

function initializeTriangleBuffers() {
    let triangleVertices = new Float32Array([0.0, 50.0, 0.0, -50.0, -50.0, 0.0, 50.0, -50.0, 0.0]);

    vaoTriangle = gl.createVertexArray();
    gl.bindVertexArray(vaoTriangle);

    vboTriangle = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboTriangle);
    gl.bufferData(gl.ARRAY_BUFFER, triangleVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.CG_ATTRIBUTE_VERTEX, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.CG_ATTRIBUTE_VERTEX);

    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.bindVertexArray(null);
}

function display() {
    gl.clear(gl.COLOR_BUFFER_BIT);
    gl.useProgram(shaderProgramObject);

    let modelViewMatrix = mat4.create();
    let modelViewProjectionMatrix = mat4.create();

    mat4.multiply(modelViewProjectionMatrix, orthographicProjectionMatrix, modelViewMatrix);
    gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

    gl.bindVertexArray(vaoTriangle);

    gl.drawArrays(gl.TRIANGLES, 0, 3);

    gl.bindVertexArray(null);
    gl.useProgram(null);

    displayAnimationRequestId = requestAnimationFrame(display);
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

    if (width == 0) {
        width = 1;
    }

    surface.width = width;
    surface.height = height;

    gl.viewport(0, 0, width, height);

    let ratio = 1.0;

    if (width <= height) {
        ratio = height / width;
        mat4.ortho(orthographicProjectionMatrix, -100.0, 100.0, -100.0 * ratio, 100.0 * ratio, -100.0, 100.0);
    } else {
        ratio = width / height;
        mat4.ortho(orthographicProjectionMatrix, -100.0 * ratio, 100.0 * ratio, -100.0, 100.0, -100.0, 100.0);
    }
}

function cleanUp() {
    cancelAnimationFrame(displayAnimationRequestId);

    if (vaoTriangle) {
        gl.deleteVertexArray(vaoTriangle);
        vaoTriangle = null;
    }

    if (vboTriangle) {
        gl.deleteBuffer(vboTriangle);
        vboTriangle = null;
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
}
