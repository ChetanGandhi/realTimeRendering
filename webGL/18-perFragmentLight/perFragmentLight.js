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

let modelMatrixUniform = null;
let viewMatrixUniform = null;
let projectionMatrixUniform = null;
let laUniform = null;
let ldUniform = null;
let lsUniform = null;
let kaUniform = null;
let kdUniform = null;
let ksUniform = null;
let materialShininessUniform = null;
let lightPositionUniform = null;
let isLightingEnabledUniform = null;

let perspectiveProjectionMatrix = null;
let sphere = null;

let lightAmbient = [0.0, 0.0, 0.0];
let lightDiffuse = [1.0, 1.0, 1.0];
let lightSpecular = [1.0, 1.0, 1.0];
let lightPosition = [100.0, 100.0, 100.0, 1.0];

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
        "out vec3 tNormal;" +
        "out vec3 source;" +
        "out vec3 viewVector;" +
        "\n" +
        "uniform mat4 modelMatrix;" +
        "uniform mat4 viewMatrix;" +
        "uniform mat4 projectionMatrix;" +
        "uniform vec4 lightPosition;" +
        "uniform mediump int isLightingEnabled;" +
        "\n" +
        "void main(void)" +
        "{" +
        "   if(isLightingEnabled == 1)" +
        "   {" +
        "       vec4 eyeCoordinates = viewMatrix * modelMatrix * vertexPosition;" +
        "       tNormal = mat3(viewMatrix * modelMatrix) * vertexNormal;" +
        "       source = vec3(lightPosition) - eyeCoordinates.xyz;" +
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
        "in vec3 source;" +
        "in vec3 viewVector;" +
        "\n" +
        "out vec4 fragmentColor;" +
        "\n" +
        "uniform int isLightingEnabled;" +
        "uniform vec3 la;" +
        "uniform vec3 ld;" +
        "uniform vec3 ls;" +
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
        "       vec3 normalizedSource = normalize(source);" +
        "       vec3 normalizedViewVector = normalize(viewVector);" +
        "       float tNormalDotLightDirection = max(dot(normalizedTNormal, normalizedSource), 0.0);" +
        "       vec3 ambient = la * ka;" +
        "       vec3 diffuse = ld * kd * tNormalDotLightDirection;" +
        "       vec3 reflectionVector = reflect(-normalizedSource, normalizedTNormal);" +
        "       vec3 specular = ls * ks * pow(max(dot(reflectionVector, normalizedViewVector), 0.0), materialShininess);" +
        "       phongAdsColor = ambient + diffuse + specular;" +
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
    laUniform = gl.getUniformLocation(shaderProgramObject, "la");
    ldUniform = gl.getUniformLocation(shaderProgramObject, "ld");
    lsUniform = gl.getUniformLocation(shaderProgramObject, "ls");
    kaUniform = gl.getUniformLocation(shaderProgramObject, "ka");
    kdUniform = gl.getUniformLocation(shaderProgramObject, "kd");
    ksUniform = gl.getUniformLocation(shaderProgramObject, "ks");
    lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "lightPosition");
    materialShininessUniform = gl.getUniformLocation(shaderProgramObject, "materialShininess");
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
        gl.uniform3fv(laUniform, lightAmbient);
        gl.uniform3fv(ldUniform, lightDiffuse);
        gl.uniform3fv(lsUniform, lightSpecular);
        gl.uniform4fv(lightPositionUniform, lightPosition);

        gl.uniform3fv(kaUniform, materialAmbient);
        gl.uniform3fv(kdUniform, materialDiffuse);
        gl.uniform3fv(ksUniform, materialSpecular);
        gl.uniform1f(materialShininessUniform, materialShininess);
    } else {
        gl.uniform1i(isLightingEnabledUniform, 0);
    }

    drawSphere();

    gl.useProgram(null);

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
