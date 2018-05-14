let surface = null;
let gl = null;
let isEscapeKeyPressed = false;
let isFullscreen = false;
let surfaceOriginalWidth = 1;
let surfaceOriginalHeight = 1;
let displayAnimationRequestId = 0;
let isLightingEnabled = false;
let isAnimationEnabled = false;

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

let anglePyramid = 0.0;
let speed = 0.1;

let vertexShaderObject = null;
let fragmentShaderObject = null;
let shaderProgramObject = null;

let vaoPyramid = null;
let vboPyramidPosition = null;
let vboPyramidNormal = null;
let modelMatrixUniform = null;
let viewMatrixUniform = null;
let projectionMatrixUniform = null;
let laZeroUniform = null;
let ldZeroUniform = null;
let lsZeroUniform = null;
let laOneUniform = null;
let ldOneUniform = null;
let lsOneUniform = null;
let kaUniform = null;
let kdUniform = null;
let ksUniform = null;
let materialShininessUniform = null;
let lightZeroPositionUniform = null;
let lightOnePositionUniform = null;
let isLightingEnabledUniform = null;

let perspectiveProjectionMatrix = null;

let lightZeroAmbient = [0.0, 0.0, 0.0];
let lightZeroDiffuse = [1.0, 0.0, 0.0];
let lightZeroSpecular = [1.0, 0.0, 0.0];
let lightZeroPosition = [2.0, 1.0, 1.0, 0.0];

let lightOneAmbient = [0.0, 0.0, 0.0];
let lightOneDiffuse = [0.0, 0.0, 1.0];
let lightOneSpecular = [0.0, 0.0, 1.0];
let lightOnePosition = [-2.0, 1.0, 1.0, 0.0];

let materialAmbient = [0.0, 0.0, 0.0];
let materialDiffuse = [1.0, 1.0, 1.0];
let materialSpecular = [1.0, 1.0, 1.0];
let materialShininess = 50.0;

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

        case "1":
            speed = 0.1;
            break;

        case "2":
            speed = 0.1 * 2;
            break;

        case "3":
            speed = 0.1 * 3;
            break;

        case "4":
            speed = 0.1 * 4;
            break;

        case "5":
            speed = 0.1 * 5;
            break;

        case "6":
            speed = 0.1 * 6;
            break;

        case "7":
            speed = 0.1 * 7;
            break;

        case "8":
            speed = 0.1 * 8;
            break;

        case "9":
            speed = 0.1 * 9;
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
    initializePyramidBuffers();

    perspectiveProjectionMatrix = mat4.create();

    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    isAnimationEnabled = false;
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
        "out vec3 tNormal;" +
        "out vec3 lightZeroDirection;" +
        "out vec3 lightOneDirection;" +
        "out vec3 viewVector;" +
        "\n" +
        "uniform mat4 modelMatrix;" +
        "uniform mat4 viewMatrix;" +
        "uniform mat4 projectionMatrix;" +
        "uniform vec4 lightZeroPosition;" +
        "uniform vec4 lightOnePosition;" +
        "uniform mediump int isLightingEnabled;" +
        "\n" +
        "void main(void)" +
        "{" +
        "   if(isLightingEnabled == 1)" +
        "   {" +
        "       vec4 eyeCoordinates = viewMatrix *  modelMatrix * vertexPosition;" +
        "       tNormal = mat3(viewMatrix * modelMatrix) * vertexNormal;" +
        "       lightZeroDirection = vec3(lightZeroPosition) - eyeCoordinates.xyz;" +
        "       lightOneDirection = vec3(lightOnePosition) - eyeCoordinates.xyz;" +
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
        "in vec3 lightOneDirection;" +
        "in vec3 viewVector;" +
        "\n" +
        "out vec4 fragmentColor;" +
        "\n" +
        "uniform int isLightingEnabled;" +
        "uniform vec3 laZero;" +
        "uniform vec3 ldZero;" +
        "uniform vec3 lsZero;" +
        "uniform vec3 laOne;" +
        "uniform vec3 ldOne;" +
        "uniform vec3 lsOne;" +
        "uniform vec3 ka;" +
        "uniform vec3 kd;" +
        "uniform vec3 ks;" +
        "uniform float materialShininess;" +
        "\n" +
        "void main(void)" +
        "{" +
        "   vec3 phongAdsColor;" +
        "   if(isLightingEnabled == 1)" +
        "   {" +
        "       vec3 normalizedTNormal = normalize(tNormal);" +
        "       vec3 normalizedViewVector = normalize(viewVector);" +
        "\n" +
        "       vec3 normalizedLightZeroDirection = normalize(lightZeroDirection);" +
        "       float tNormalDotLightZeroDirection = max(dot(normalizedTNormal, normalizedLightZeroDirection), 0.0);" +
        "       vec3 ambientLightZero = laZero * ka;" +
        "       vec3 diffuseLightZero = ldZero * kd * tNormalDotLightZeroDirection;" +
        "       vec3 reflectionVectorLightZero = reflect(-normalizedLightZeroDirection, normalizedTNormal);" +
        "       vec3 specularLightZero = lsZero * ks * pow(max(dot(reflectionVectorLightZero, normalizedViewVector), 0.0), materialShininess);" +
        "\n" +
        "       vec3 normalizedLightOneDirection = normalize(lightOneDirection);" +
        "       float tNormalDotLightOneDirection = max(dot(normalizedTNormal, normalizedLightOneDirection), 0.0);" +
        "       vec3 ambientLightOne = laOne * ka;" +
        "       vec3 diffuseLightOne = ldOne * kd * tNormalDotLightOneDirection;" +
        "       vec3 reflectionVectorLightOne = reflect(-normalizedLightOneDirection, normalizedTNormal);" +
        "       vec3 specularLightOne = lsOne * ks * pow(max(dot(reflectionVectorLightOne, normalizedViewVector), 0.0), materialShininess);" +
        "\n" +
        "       phongAdsColor = ambientLightZero + diffuseLightZero + specularLightZero + ambientLightOne + diffuseLightOne + specularLightOne;" +
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
    laZeroUniform = gl.getUniformLocation(shaderProgramObject, "lzZero");
    ldZeroUniform = gl.getUniformLocation(shaderProgramObject, "ldZero");
    lsZeroUniform = gl.getUniformLocation(shaderProgramObject, "lsZero");
    laOneUniform = gl.getUniformLocation(shaderProgramObject, "lzOne");
    ldOneUniform = gl.getUniformLocation(shaderProgramObject, "ldOne");
    lsOneUniform = gl.getUniformLocation(shaderProgramObject, "lsOne");
    kaUniform = gl.getUniformLocation(shaderProgramObject, "ka");
    kdUniform = gl.getUniformLocation(shaderProgramObject, "kd");
    ksUniform = gl.getUniformLocation(shaderProgramObject, "ks");
    lightZeroPositionUniform = gl.getUniformLocation(shaderProgramObject, "lightPosZeroitionUniform");
    lightOnePositionUniform = gl.getUniformLocation(shaderProgramObject, "lightOnePositionUniform");
    materialShininessUniform = gl.getUniformLocation(shaderProgramObject, "materialShininess");
}

function initializePyramidBuffers() {
    // prettier-ignore
    let pyramidVertices = new Float32Array([
        // Front face
        0.0, 1.0, 0.0,
        -1.0, -1.0, 1.0,
        1.0, -1.0, 1.0,

        // Right face
        0.0, 1.0, 0.0,
        1.0, -1.0, 1.0,
        1.0, -1.0, -1.0,

        // Back face
        0.0, 1.0, 0.0,
        1.0, -1.0, -1.0,
        -1.0, -1.0, -1.0,

        // Left face
        0.0, 1.0, 0.0,
        -1.0, -1.0, -1.0,
        -1.0, -1.0, 1.0
    ]);

    // prettier-ignore
    let pyramidNormals = new Float32Array([
        // Front face
        0.0, 0.447214, 0.894427,
        0.0, 0.447214, 0.894427,
        0.0, 0.447214, 0.894427,

        // Right face
        0.894427, 0.447214, 0.0,
        0.894427, 0.447214, 0.0,
        0.894427, 0.447214, 0.0,

        // Back face
        0.0, 0.447214, -0.894427,
        0.0, 0.447214, -0.894427,
        0.0, 0.447214, -0.894427,

        // Left face
        -0.894427, 0.447214, 0.0,
        -0.894427, 0.447214, 0.0,
        -0.894427, 0.447214, 0.0
    ]);

    vaoPyramid = gl.createVertexArray();
    gl.bindVertexArray(vaoPyramid);

    vboPyramidPosition = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboPyramidPosition);
    gl.bufferData(gl.ARRAY_BUFFER, pyramidVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.CG_ATTRIBUTE_VERTEX, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.CG_ATTRIBUTE_VERTEX);

    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    vboPyramidNormal = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboPyramidNormal);
    gl.bufferData(gl.ARRAY_BUFFER, pyramidNormals, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.CG_ATTRIBUTE_NORMAL, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.CG_ATTRIBUTE_NORMAL);

    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);
}

function update() {
    if (isAnimationEnabled) {
        anglePyramid += speed;

        if (anglePyramid >= 360.0) {
            anglePyramid = 0.0;
        }
    }
}

function display() {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.useProgram(shaderProgramObject);

    if (isLightingEnabled) {
        gl.uniform1i(isLightingEnabledUniform, 1);
        gl.uniform3fv(laZeroUniform, lightZeroAmbient);
        gl.uniform3fv(ldZeroUniform, lightZeroDiffuse);
        gl.uniform3fv(lsZeroUniform, lightZeroSpecular);

        gl.uniform3fv(laOneUniform, lightOneAmbient);
        gl.uniform3fv(ldOneUniform, lightOneDiffuse);
        gl.uniform3fv(lsOneUniform, lightOneSpecular);

        gl.uniform4fv(lightZeroPositionUniform, lightZeroPosition);
        gl.uniform4fv(lightOnePositionUniform, lightOnePosition);

        gl.uniform3fv(kaUniform, materialAmbient);
        gl.uniform3fv(kdUniform, materialDiffuse);
        gl.uniform3fv(ksUniform, materialSpecular);
        gl.uniform1f(materialShininessUniform, materialShininess);
    } else {
        gl.uniform1i(isLightingEnabledUniform, 0);
    }

    drawPyramid();

    gl.useProgram(null);

    update();
    displayAnimationRequestId = requestAnimationFrame(display);
}

function drawPyramid() {
    let modelMatrix = mat4.create();
    let viewMatrix = mat4.create();
    let rotationMatrix = mat4.create();

    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
    mat4.rotateY(rotationMatrix, rotationMatrix, degreeToRadian(anglePyramid));

    mat4.multiply(modelMatrix, modelMatrix, rotationMatrix);

    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    gl.bindVertexArray(vaoPyramid);

    gl.drawArrays(gl.TRIANGLES, 0, 12);

    gl.bindVertexArray(null);
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

    if (vaoPyramid) {
        gl.deleteVertexArray(vaoPyramid);
        vaoPyramid = null;
    }

    if (vboPyramidPosition) {
        gl.deleteBuffer(vboPyramidPosition);
        vboPyramidPosition = null;
    }

    if (vboPyramidNormal) {
        gl.deleteBuffer(vboPyramidNormal);
        vboPyramidNormal = null;
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
