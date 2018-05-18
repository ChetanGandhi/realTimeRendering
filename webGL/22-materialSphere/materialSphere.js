const numberOfRows = 6;
const numberOfColumns = 4;

let viewportWidth = 1;
let viewportHeight = 1;
let viewportInitialX = 0;
let viewportInitialY = 0;

let surface = null;
let gl = null;
let isEscapeKeyPressed = false;
let isFullscreen = false;
let surfaceOriginalWidth = 1;
let surfaceOriginalHeight = 1;
let displayAnimationRequestId = 0;
let isLightingEnabled = false;
let isAnimationEnabled = false;
let rotateLightOnXAxis = true;
let rotateLightOnYAxis = false;
let rotateLightOnZAxis = false;

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

let angleLightRotation = 0.0;
let speed = 0.01;

let vertexShaderObject = null;
let fragmentShaderObject = null;
let shaderProgramObject = null;

let modelMatrixUniform = null;
let viewMatrixUniform = null;
let projectionMatrixUniform = null;
let laZeroUniform = null;
let ldZeroUniform = null;
let lsZeroUniform = null;
let kaUniform = null;
let kdUniform = null;
let ksUniform = null;
let materialShininessUniform = null;
let lightZeroPositionUniform = null;
let isLightingEnabledUniform = null;

let perspectiveProjectionMatrix = null;
let sphere = null;

let lightZeroAmbient = [0.5, 0.5, 0.5];
let lightZeroDiffuse = [1.0, 1.0, 1.0];
let lightZeroSpecular = [1.0, 1.0, 1.0];
let lightZeroPosition = [0.0, 0.0, 1.0, 0.0];

let materialAmbient = [
    [
        // Column 1
        [0.0215, 0.1745, 0.0215],
        [0.135, 0.2225, 0.1575],
        [0.05375, 0.05, 0.06625],
        [0.25, 0.20725, 0.20725],
        [0.1745, 0.01175, 0.01175],
        [0.1, 0.18725, 0.1745]
    ],
    [
        // Column 2
        [0.329412, 0.223529, 0.027451],
        [0.2125, 0.1275, 0.054],
        [0.25, 0.25, 0.25],
        [0.19125, 0.0735, 0.0225],
        [0.24725, 0.1995, 0.0745],
        [0.19225, 0.19225, 0.19225]
    ],
    [
        // Column 3
        [0.0, 0.0, 0.0],
        [0.0, 0.1, 0.06],
        [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0]
    ],
    [
        // Column 4
        [0.02, 0.02, 0.02],
        [0.0, 0.05, 0.05],
        [0.0, 0.05, 0.0],
        [0.05, 0.0, 0.0],
        [0.05, 0.05, 0.05],
        [0.05, 0.05, 0.0]
    ]
];

let materialDiffuse = [
    [
        // Column 1
        [0.07568, 0.61424, 0.07568],
        [0.54, 0.89, 0.63],
        [0.18275, 0.17, 0.22525],
        [1.0, 0.829, 0.829],
        [0.61424, 0.04136, 0.04136],
        [0.396, 0.74151, 0.69102]
    ],
    [
        // Column 2
        [0.780392, 0.568627, 0.113725],
        [0.714, 0.4284, 0.18144],
        [0.4, 0.4, 0.4],
        [0.7038, 0.27048, 0.0828],
        [0.75164, 0.60648, 0.22648],
        [0.50754, 0.50754, 0.50754]
    ],
    [
        // Column 3
        [0.01, 0.01, 0.01],
        [0.0, 0.50980392, 0.50980392],
        [0.1, 0.35, 0.1],
        [0.5, 0.0, 0.0],
        [0.55, 0.55, 0.55],
        [0.5, 0.5, 0.0]
    ],
    [
        // Column 4
        [0.01, 0.01, 0.01],
        [0.4, 0.5, 0.5],
        [0.4, 0.5, 0.4],
        [0.5, 0.4, 0.4],
        [0.5, 0.5, 0.5],
        [0.5, 0.5, 0.4]
    ]
];

let materialSpecular = [
    [
        // Column 1
        [0.633, 0.727811, 0.633],
        [0.316228, 0.316228, 0.316228],
        [0.332741, 0.328634, 0.346435],
        [0.296648, 0.296648, 0.296648],
        [0.727811, 0.626959, 0.626959],
        [0.297254, 0.30829, 0.306678]
    ],
    [
        // Column 2
        [0.992157, 0.941176, 0.807843],
        [0.393548, 0.271906, 0.166721],
        [0.774597, 0.774597, 0.774597],
        [0.256777, 0.137622, 0.086014],
        [0.628281, 0.555802, 0.366065],
        [0.508273, 0.508273, 0.508273]
    ],
    [
        // Column 3
        [0.5, 0.5, 0.5],
        [0.50196078, 0.50196078, 0.50196078],
        [0.45, 0.55, 0.45],
        [0.7, 0.6, 0.6],
        [0.7, 0.7, 0.7],
        [0.6, 0.6, 0.5]
    ],
    [
        // Column 4
        [0.4, 0.4, 0.4],
        [0.04, 0.7, 0.7],
        [0.04, 0.7, 0.04],
        [0.7, 0.04, 0.04],
        [0.7, 0.7, 0.7],
        [0.7, 0.7, 0.04]
    ]
];

let materialShininess = [
    [
        // Column 1
        0.6 * 128.0,
        0.1 * 128.0,
        0.3 * 128.0,
        0.088 * 128.0,
        0.6 * 128.0,
        0.1 * 128.0
    ],
    [
        // Column 2
        0.21794872 * 128.0,
        0.2 * 128.0,
        0.6 * 128.0,
        0.1 * 128.0,
        0.4 * 128.0,
        0.4 * 128.0
    ],
    [
        // Column 3
        0.25 * 128.0,
        0.25 * 128.0,
        0.25 * 128.0,
        0.25 * 128.0,
        0.25 * 128.0,
        0.25 * 128.0
    ],
    [
        // Column 4
        0.078125 * 128.0,
        0.078125 * 128.0,
        0.078125 * 128.0,
        0.078125 * 128.0,
        0.078125 * 128.0,
        0.078125 * 128.0
    ]
];

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

        case "A":
        case "a":
            isAnimationEnabled = !isAnimationEnabled;
            break;

        case "L":
        case "l":
            isLightingEnabled = !isLightingEnabled;
            break;

        case "X":
        case "x":
            rotateLightOnXAxis = true;
            rotateLightOnYAxis = false;
            rotateLightOnZAxis = false;
            break;

        case "Y":
        case "y":
            rotateLightOnXAxis = false;
            rotateLightOnYAxis = true;
            rotateLightOnZAxis = false;
            break;

        case "Z":
        case "z":
            rotateLightOnXAxis = false;
            rotateLightOnYAxis = false;
            rotateLightOnZAxis = true;
            break;

        case "1":
            speed = 0.01;
            break;

        case "2":
            speed = 0.01 * 2;
            break;

        case "3":
            speed = 0.01 * 3;
            break;

        case "4":
            speed = 0.01 * 4;
            break;

        case "5":
            speed = 0.01 * 5;
            break;

        case "6":
            speed = 0.01 * 6;
            break;

        case "7":
            speed = 0.01 * 7;
            break;

        case "8":
            speed = 0.01 * 8;
            break;

        case "9":
            speed = 0.01 * 9;
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

    gl.clearColor(0.25, 0.25, 0.25, 1.0);
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    isAnimationEnabled = false;
    isLightingEnabled = false;
    rotateLightOnXAxis = true;
    rotateLightOnYAxis = false;
    rotateLightOnZAxis = false;

    resize(surfaceOriginalWidth, surfaceOriginalHeight);
}

function initializeVertexShaderObject() {
    let vertexShaderCode =
        "#version 300 es" +
        "\n" +
        "in vec4 vertexPosition;" +
        "in vec3 vertexNormal;" +
        "\n" +
        "out vec3 tNormal;" +
        "out vec3 lightZeroDirection;" +
        "out vec4 eyeCoordinates;" +
        "out vec3 viewVector;" +
        "\n" +
        "uniform mat4 modelMatrix;" +
        "uniform mat4 viewMatrix;" +
        "uniform mat4 projectionMatrix;" +
        "uniform vec4 lightZeroPosition;" +
        "uniform mediump int isLightingEnabled;" +
        "\n" +
        "void main(void)" +
        "{" +
        "   if(isLightingEnabled == 1)" +
        "   {" +
        "       eyeCoordinates = viewMatrix * modelMatrix * vertexPosition;" +
        "       tNormal = mat3(viewMatrix * modelMatrix) * vertexNormal;" +
        "       lightZeroDirection = vec3(lightZeroPosition);" +
        "       viewVector = -eyeCoordinates.xyz;" +
        "   }" +
        "\n" +
        "   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;" +
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
        "in vec3 tNormal;" +
        "in vec3 lightZeroDirection;" +
        "in vec4 eyeCoordinates;" +
        "in vec3 viewVector;" +
        "\n" +
        "out vec4 fragmentColor;" +
        "\n" +
        "uniform int isLightingEnabled;" +
        "uniform vec3 laZero;" +
        "uniform vec3 ldZero;" +
        "uniform vec3 lsZero;" +
        "uniform vec3 ka;" +
        "uniform vec3 kd;" +
        "uniform vec3 ks;" +
        "uniform float materialShininess;" +
        "\n" +
        "void main(void)" +
        "{" +
        "   vec3 phongAdsColor = vec3(0.0, 0.0, 0.0);" +
        "   if(isLightingEnabled == 1)" +
        "   {" +
        "       vec3 normalizedTNormal = normalize(tNormal);" +
        "       vec3 normalizedViewVector = normalize(viewVector);" +
        "\n" +
        "       vec3 normalizedLightZeroDirection = normalize(lightZeroDirection - eyeCoordinates.xyz);" +
        "       float tNormalDotLightZeroDirection = max(dot(normalizedTNormal, normalizedLightZeroDirection), 0.0);" +
        "       vec3 ambientLightZero = laZero * ka;" +
        "       vec3 diffuseLightZero = ldZero * kd * tNormalDotLightZeroDirection;" +
        "       vec3 reflectionVectorLightZero = reflect(-normalizedLightZeroDirection, normalizedTNormal);" +
        "       vec3 specularLightZero = lsZero * ks * pow(max(dot(reflectionVectorLightZero, normalizedViewVector), 0.0), materialShininess);" +
        "\n" +
        "       phongAdsColor = ambientLightZero + diffuseLightZero + specularLightZero;" +
        "   }" +
        "   else" +
        "   {" +
        "       phongAdsColor = vec3(1.0, 1.0, 1.0);" +
        "   }" +
        "\n" +
        "   fragmentColor = vec4(phongAdsColor, 1.0);" +
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

    modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "modelMatrix");
    viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "viewMatrix");
    projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "projectionMatrix");
    isLightingEnabledUniform = gl.getUniformLocation(shaderProgramObject, "isLightingEnabled");
    laZeroUniform = gl.getUniformLocation(shaderProgramObject, "laZero");
    ldZeroUniform = gl.getUniformLocation(shaderProgramObject, "ldZero");
    lsZeroUniform = gl.getUniformLocation(shaderProgramObject, "lsZero");
    kaUniform = gl.getUniformLocation(shaderProgramObject, "ka");
    kdUniform = gl.getUniformLocation(shaderProgramObject, "kd");
    ksUniform = gl.getUniformLocation(shaderProgramObject, "ks");
    lightZeroPositionUniform = gl.getUniformLocation(shaderProgramObject, "lightZeroPosition");
    materialShininessUniform = gl.getUniformLocation(shaderProgramObject, "materialShininess");
}

function initializeSphereBuffers() {
    sphere = new Mesh();
    makeSphere(sphere, 2.0, 30, 30);
}

function update() {
    if (!isAnimationEnabled) {
        return;
    }

    angleLightRotation += speed;

    if (angleLightRotation >= 360.0) {
        angleLightRotation = 0.0;
    }
}

function display() {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    for (let columnCounter = 0; columnCounter < numberOfColumns; ++columnCounter) {
        for (let rowCounter = 0; rowCounter < numberOfRows; ++rowCounter) {
            gl.viewport(viewportInitialX + columnCounter * viewportWidth, viewportInitialY + rowCounter * viewportHeight, viewportWidth, viewportHeight);

            gl.useProgram(shaderProgramObject);

            if (isLightingEnabled) {
                gl.uniform1i(isLightingEnabledUniform, 1);

                gl.uniform3fv(laZeroUniform, lightZeroAmbient);
                gl.uniform3fv(ldZeroUniform, lightZeroDiffuse);
                gl.uniform3fv(lsZeroUniform, lightZeroSpecular);

                if (rotateLightOnXAxis) {
                    lightZeroPosition[0] = 0.0;
                    lightZeroPosition[1] = Math.sin(angleLightRotation) * 100.0 - 6.0;
                    lightZeroPosition[2] = Math.cos(angleLightRotation) * 100.0 - 6.0;
                } else if (rotateLightOnYAxis) {
                    lightZeroPosition[0] = Math.sin(angleLightRotation) * 100.0 - 6.0;
                    lightZeroPosition[1] = 0.0;
                    lightZeroPosition[2] = Math.cos(angleLightRotation) * 100.0 - 6.0;
                } else if (rotateLightOnZAxis) {
                    lightZeroPosition[0] = Math.sin(angleLightRotation) * 100.0 - 6.0;
                    lightZeroPosition[1] = Math.cos(angleLightRotation) * 100.0 - 6.0;
                    lightZeroPosition[2] = 0.0;
                }

                gl.uniform4fv(lightZeroPositionUniform, lightZeroPosition);
                gl.uniform3fv(kaUniform, materialAmbient[columnCounter][rowCounter]);
                gl.uniform3fv(kdUniform, materialDiffuse[columnCounter][rowCounter]);
                gl.uniform3fv(ksUniform, materialSpecular[columnCounter][rowCounter]);
                gl.uniform1f(materialShininessUniform, materialShininess[columnCounter][rowCounter]);
            } else {
                gl.uniform1i(isLightingEnabledUniform, 0);
            }

            drawSphere();

            gl.useProgram(null);
        }
    }

    update();
    displayAnimationRequestId = requestAnimationFrame(display);
}

function drawSphere() {
    let modelMatrix = mat4.create();
    let viewMatrix = mat4.create();

    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
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

    viewportWidth = width / numberOfRows;
    viewportHeight = height / numberOfRows;
    viewportInitialX = (width - viewportWidth * numberOfColumns) / 2;
    viewportInitialY = (height - viewportHeight * numberOfRows) / 2;
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
