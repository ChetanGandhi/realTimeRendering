let surface = null;
let gl = null;
let isEscapeKeyPressed = false;
let isFullscreen = false;
let surfaceOriginalWidth = 1;
let surfaceOriginalHeight = 1;
let displayAnimationRequestId = 0;
let isLightingEnabled = false;

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

let modelViewMatrixUniform = null;
let projectionMatrixUniform = null;
let ldUniform = null;
let kdUniform = null;
let lightPositionUniform = null;
let isLightingEnabledUniform = null;

let perspectiveProjectionMatrix = null;
let sphere = null;

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

        case "L":
        case "l":
            isLightingEnabled = !isLightingEnabled;
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
    initializeSphereBuffers();

    perspectiveProjectionMatrix = mat4.create();

    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    isLightingEnabled = false;

    resize(surfaceOriginalWidth, surfaceOriginalHeight);
}

function initializeVertexShaderObject() {
    let vertexShaderCode =
        "#version 300 es" +
        "\n" +
        "in vec4 vertexPosition;" +
        "in vec3 vertexNormal;" +
        "\n" +
        "out vec3 diffuseLight;" +
        "\n" +
        "uniform mat4 modelViewMatrix;" +
        "uniform mat4 projectionMatrix;" +
        "uniform mediump int isLightingEnabled;" +
        "uniform vec3 ld;" +
        "uniform vec3 kd;" +
        "uniform vec4 lightPosition;" +
        "\n" +
        "void main(void)" +
        "{" +
        "   if(isLightingEnabled == 1);" +
        "   {" +
        "       vec4 eyeCoordinates = modelViewMatrix * vertexPosition;" +
        "       vec3 tNormal = normalize(mat3(modelViewMatrix) * vertexNormal);" +
        "       vec3 source = normalize(vec3(lightPosition - eyeCoordinates));" +
        "       diffuseLight = ld * kd * max(dot(source, tNormal), 0.0);" +
        "   }" +
        "\n" +
        "   gl_Position = projectionMatrix * modelViewMatrix * vertexPosition;" +
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
        "\n" +
        "in vec3 diffuseLight;" +
        "\n" +
        "out vec4 fragmentColor;" +
        "\n" +
        "uniform int isLightingEnabled;" +
        "\n" +
        "void main(void)" +
        "{" +
        "   if(isLightingEnabled == 1)" +
        "   {" +
        "       fragmentColor = vec4(diffuseLight, 1.0);" +
        "   }" +
        "   else" +
        "   {" +
        "       fragmentColor = vec4(1.0, 1.0, 1.0, 1.0);" +
        "   }" +
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
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.CG_ATTRIBUTE_NORMAL, "vertexNormal");
    gl.linkProgram(shaderProgramObject);

    if (!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)) {
        let error = gl.getProgramInfoLog(shaderProgramObject);

        if (error.length > 0) {
            alert(error);
            cleanUp();
        }
    }

    modelViewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "modelViewMatrix");
    projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "projectionMatrix");
    isLightingEnabledUniform = gl.getUniformLocation(shaderProgramObject, "isLightingEnabled");
    ldUniform = gl.getUniformLocation(shaderProgramObject, "ld");
    kdUniform = gl.getUniformLocation(shaderProgramObject, "kd");
    lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "lightPositionUniform");
}

function initializeSphereBuffers() {
    sphere = new Mesh();
    makeSphere(sphere, 2.0, 30, 30);
}

function display() {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.useProgram(shaderProgramObject);

    if (isLightingEnabled) {
        gl.uniform1i(isLightingEnabledUniform, 1);
        gl.uniform3f(ldUniform, 1.0, 1.0, 1.0);
        gl.uniform3f(kdUniform, 0.5, 0.5, 0.5);

        let lightPosition = [0.0, 0.0, 2.0, 1.0];
        gl.uniform4fv(lightPositionUniform, lightPosition);
    }

    drawSphere();

    gl.useProgram(null);

    displayAnimationRequestId = requestAnimationFrame(display);
}

function drawSphere() {
    let modelMatrix = mat4.create();
    let modelViewMatrix = mat4.create();

    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
    mat4.multiply(modelViewMatrix, modelViewMatrix, modelMatrix);
    gl.uniformMatrix4fv(modelViewMatrixUniform, false, modelViewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    sphere.draw();
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
    mat4.perspective(perspectiveProjectionMatrix, 45.0, width / height, 1.0, 100.0);
}

function cleanUp() {
    cancelAnimationFrame(displayAnimationRequestId);

    if (sphere) {
        sphere.deallocate();
        sphere = null;
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

function degreeToRadian(degree) {
    return degree * Math.PI / 180.0;
}
