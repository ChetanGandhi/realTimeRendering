let surface = null;
let gl = null;
let isEscapeKeyPressed = false;
let isFullscreen = false;
let surfaceOriginalWidth = 1;
let surfaceOriginalHeight = 1;
let displayAnimationRequestId = 0;
let isLightingEnabled = false;
let isPerFragmentLightEnabled = false;

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

let vertexShaderObjectPerVertex = null;
let fragmentShaderObjectPerVertex = null;
let shaderProgramObjectPerVertex = null;

let modelMatrixUniformPerVertex = null;
let viewMatrixUniformPerVertex = null;
let projectionMatrixUniformPerVertex = null;
let laUniformPerVertex = null;
let ldUniformPerVertex = null;
let lsUniformPerVertex = null;
let kaUniformPerVertex = null;
let kdUniformPerVertex = null;
let ksUniformPerVertex = null;
let materialShininessUniformPerVertex = null;
let lightPositionUniformPerVertex = null;
let isLightingEnabledUniformPerVertex = null;

let vertexShaderObjectPerFragment = null;
let fragmentShaderObjectPerFragment = null;
let shaderProgramObjectPerFragment = null;

let modelMatrixUniformPerFragment = null;
let viewMatrixUniformPerFragment = null;
let projectionMatrixUniformPerFragment = null;
let laUniformPerFragment = null;
let ldUniformPerFragment = null;
let lsUniformPerFragment = null;
let kaUniformPerFragment = null;
let kdUniformPerFragment = null;
let ksUniformPerFragment = null;
let materialShininessUniformPerFragment = null;
let lightPositionUniformPerFragment = null;
let isLightingEnabledUniformPerFragment = null;

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
        case "F11":
            // This is to prevent browser's default action of fullscreen.
            event.preventDefault();
            toggleFullscreen();
            break;

        case "Escape":
            if (!isEscapeKeyPressed) {
                isEscapeKeyPressed = true;
                cleanUp();
                alert("Rendering is stopped.");
            }
            break;

        case "F":
        case "f":
            isPerFragmentLightEnabled = true;
            break;

        case "L":
        case "l":
            isLightingEnabled = !isLightingEnabled;
            break;

        case "V":
        case "v":
            isPerFragmentLightEnabled = false;
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

    initializeVertexShaderObjectPerVertex();
    initializeFragmentShaderObjectPerVertex();
    initializeShaderProgramObjectPerVertex();
    initializeVertexShaderObjectPerFragment();
    initializeFragmentShaderObjectPerFragment();
    initializeShaderProgramObjectPerFragment();
    initializeSphereBuffers();

    perspectiveProjectionMatrix = mat4.create();

    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    isLightingEnabled = false;
    isPerFragmentLightEnabled = false;

    resize(surfaceOriginalWidth, surfaceOriginalHeight);
}

function initializeVertexShaderObjectPerVertex() {
    let vertexShaderCode =
        "#version 300 es" +
        "\n" +
        "in vec4 vertexPosition;" +
        "in vec3 vertexNormal;" +
        "\n" +
        "out vec3 phongAdsColor;" +
        "\n" +
        "uniform mat4 modelMatrix;" +
        "uniform mat4 viewMatrix;" +
        "uniform mat4 projectionMatrix;" +
        "uniform int isLightingEnabled;" +
        "uniform vec3 la;" +
        "uniform vec3 ld;" +
        "uniform vec3 ls;" +
        "uniform vec3 ka;" +
        "uniform vec3 kd;" +
        "uniform vec3 ks;" +
        "uniform vec4 lightPosition;" +
        "uniform float materialShininess;" +
        "\n" +
        "void main(void)" +
        "{" +
        "   if(isLightingEnabled == 1)" +
        "   {" +
        "       vec4 eyeCoordinates = viewMatrix * modelMatrix * vertexPosition;" +
        "       vec3 tNormal = normalize(mat3(viewMatrix * modelMatrix) * vertexNormal);" +
        "       vec3 source = normalize(vec3(lightPosition) - eyeCoordinates.xyz);" +
        "       float tNormalDotLightDirection = max(dot(tNormal, source), 0.0);" +
        "       vec3 ambient = la * ka;" +
        "       vec3 diffuse = ld * kd * tNormalDotLightDirection;" +
        "       vec3 reflectionVector = reflect(-source, tNormal);" +
        "       vec3 viewVector = normalize(-eyeCoordinates.xyz);" +
        "       vec3 specular = ls * ks * pow(max(dot(reflectionVector, viewVector), 0.0), materialShininess);" +
        "       phongAdsColor = ambient + diffuse + specular;" +
        "   }" +
        "   else" +
        "   {" +
        "       phongAdsColor = vec3(1.0, 1.0, 1.0);" +
        "   }" +
        "\n" +
        "   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;" +
        "}";

    vertexShaderObjectPerVertex = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObjectPerVertex, vertexShaderCode);
    gl.compileShader(vertexShaderObjectPerVertex);

    if (!gl.getShaderParameter(vertexShaderObjectPerVertex, gl.COMPILE_STATUS)) {
        let error = gl.getShaderInfoLog(vertexShaderObjectPerVertex);

        if (error.length > 0) {
            alert(error);
            cleanUp();
        }
    }
}

function initializeFragmentShaderObjectPerVertex() {
    let fragmentShaderCode =
        "#version 300 es" +
        "\n" +
        "precision highp float;" +
        "\n" +
        "in vec3 phongAdsColor;" +
        "\n" +
        "out vec4 fragmentColor;" +
        "\n" +
        "void main(void)" +
        "{" +
        "   fragmentColor = vec4(phongAdsColor, 1.0);" +
        "}";

    fragmentShaderObjectPerVertex = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObjectPerVertex, fragmentShaderCode);
    gl.compileShader(fragmentShaderObjectPerVertex);

    if (!gl.getShaderParameter(fragmentShaderObjectPerVertex, gl.COMPILE_STATUS)) {
        let error = gl.getShaderInfoLog(fragmentShaderObjectPerVertex);

        if (error.length > 0) {
            alert(error);
            cleanUp();
        }
    }
}

function initializeShaderProgramObjectPerVertex() {
    shaderProgramObjectPerVertex = gl.createProgram();
    gl.attachShader(shaderProgramObjectPerVertex, vertexShaderObjectPerVertex);
    gl.attachShader(shaderProgramObjectPerVertex, fragmentShaderObjectPerVertex);
    gl.bindAttribLocation(shaderProgramObjectPerVertex, WebGLMacros.CG_ATTRIBUTE_VERTEX, "vertexPosition");
    gl.bindAttribLocation(shaderProgramObjectPerVertex, WebGLMacros.CG_ATTRIBUTE_NORMAL, "vertexNormal");
    gl.linkProgram(shaderProgramObjectPerVertex);

    if (!gl.getProgramParameter(shaderProgramObjectPerVertex, gl.LINK_STATUS)) {
        let error = gl.getProgramInfoLog(shaderProgramObjectPerVertex);

        if (error.length > 0) {
            alert(error);
            cleanUp();
        }
    }

    modelMatrixUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "modelMatrix");
    viewMatrixUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "viewMatrix");
    projectionMatrixUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "projectionMatrix");
    isLightingEnabledUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "isLightingEnabled");
    laUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "la");
    ldUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "ld");
    lsUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "ls");
    kaUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "ka");
    kdUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "kd");
    ksUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "ks");
    lightPositionUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "lightPosition");
    materialShininessUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "materialShininess");
}

function initializeVertexShaderObjectPerFragment() {
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

    vertexShaderObjectPerFragment = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObjectPerFragment, vertexShaderCode);
    gl.compileShader(vertexShaderObjectPerFragment);

    if (!gl.getShaderParameter(vertexShaderObjectPerFragment, gl.COMPILE_STATUS)) {
        let error = gl.getShaderInfoLog(vertexShaderObjectPerFragment);

        if (error.length > 0) {
            alert(error);
            cleanUp();
        }
    }
}

function initializeFragmentShaderObjectPerFragment() {
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

    fragmentShaderObjectPerFragment = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObjectPerFragment, fragmentShaderCode);
    gl.compileShader(fragmentShaderObjectPerFragment);

    if (!gl.getShaderParameter(fragmentShaderObjectPerFragment, gl.COMPILE_STATUS)) {
        let error = gl.getShaderInfoLog(fragmentShaderObjectPerFragment);

        if (error.length > 0) {
            alert(error);
            cleanUp();
        }
    }
}

function initializeShaderProgramObjectPerFragment() {
    shaderProgramObjectPerFragment = gl.createProgram();
    gl.attachShader(shaderProgramObjectPerFragment, vertexShaderObjectPerFragment);
    gl.attachShader(shaderProgramObjectPerFragment, fragmentShaderObjectPerFragment);
    gl.bindAttribLocation(shaderProgramObjectPerFragment, WebGLMacros.CG_ATTRIBUTE_VERTEX, "vertexPosition");
    gl.bindAttribLocation(shaderProgramObjectPerFragment, WebGLMacros.CG_ATTRIBUTE_NORMAL, "vertexNormal");
    gl.linkProgram(shaderProgramObjectPerFragment);

    if (!gl.getProgramParameter(shaderProgramObjectPerFragment, gl.LINK_STATUS)) {
        let error = gl.getProgramInfoLog(shaderProgramObjectPerFragment);

        if (error.length > 0) {
            alert(error);
            cleanUp();
        }
    }

    modelMatrixUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "modelMatrix");
    viewMatrixUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "viewMatrix");
    projectionMatrixUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "projectionMatrix");
    isLightingEnabledUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "isLightingEnabled");
    laUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "la");
    ldUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "ld");
    lsUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "ls");
    kaUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "ka");
    kdUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "kd");
    ksUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "ks");
    lightPositionUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "lightPosition");
    materialShininessUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "materialShininess");
}

function initializeSphereBuffers() {
    sphere = new Mesh();
    makeSphere(sphere, 2.0, 30, 30);
}

function display() {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    if (isPerFragmentLightEnabled) {
        usePerFragmentLight();
    } else {
        usePerVertexLight();
    }

    displayAnimationRequestId = requestAnimationFrame(display);
}

function usePerVertexLight() {
    gl.useProgram(shaderProgramObjectPerVertex);

    if (isLightingEnabled) {
        gl.uniform1i(isLightingEnabledUniformPerVertex, 1);
        gl.uniform3fv(laUniformPerVertex, lightAmbient);
        gl.uniform3fv(ldUniformPerVertex, lightDiffuse);
        gl.uniform3fv(lsUniformPerVertex, lightSpecular);
        gl.uniform4fv(lightPositionUniformPerVertex, lightPosition);

        gl.uniform3fv(kaUniformPerVertex, materialAmbient);
        gl.uniform3fv(kdUniformPerVertex, materialDiffuse);
        gl.uniform3fv(ksUniformPerVertex, materialSpecular);
        gl.uniform1f(materialShininessUniformPerVertex, materialShininess);
    } else {
        gl.uniform1i(isLightingEnabledUniformPerVertex, 0);
    }

    drawSpherePerVertex();
    gl.useProgram(null);
}

function usePerFragmentLight() {
    gl.useProgram(shaderProgramObjectPerFragment);

    if (isLightingEnabled) {
        gl.uniform1i(isLightingEnabledUniformPerFragment, 1);
        gl.uniform3fv(laUniformPerFragment, lightAmbient);
        gl.uniform3fv(ldUniformPerFragment, lightDiffuse);
        gl.uniform3fv(lsUniformPerFragment, lightSpecular);
        gl.uniform4fv(lightPositionUniformPerFragment, lightPosition);

        gl.uniform3fv(kaUniformPerFragment, materialAmbient);
        gl.uniform3fv(kdUniformPerFragment, materialDiffuse);
        gl.uniform3fv(ksUniformPerFragment, materialSpecular);
        gl.uniform1f(materialShininessUniformPerFragment, materialShininess);
    } else {
        gl.uniform1i(isLightingEnabledUniformPerFragment, 0);
    }

    drawSpherePerFragment();
    gl.useProgram(null);
}

function drawSpherePerVertex() {
    let modelMatrix = mat4.create();
    let viewMatrix = mat4.create();

    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);

    gl.uniformMatrix4fv(modelMatrixUniformPerVertex, false, modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniformPerVertex, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniformPerVertex, false, perspectiveProjectionMatrix);

    sphere.draw();
}

function drawSpherePerFragment() {
    let modelMatrix = mat4.create();
    let viewMatrix = mat4.create();

    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);

    gl.uniformMatrix4fv(modelMatrixUniformPerFragment, false, modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniformPerFragment, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniformPerFragment, false, perspectiveProjectionMatrix);

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

    if (shaderProgramObjectPerVertex) {
        if (vertexShaderObjectPerVertex) {
            gl.detachShader(shaderProgramObjectPerVertex, vertexShaderObjectPerVertex);
        }

        if (fragmentShaderObjectPerVertex) {
            gl.detachShader(shaderProgramObjectPerVertex, fragmentShaderObjectPerVertex);
        }
    }

    if (vertexShaderObjectPerVertex) {
        gl.deleteShader(vertexShaderObjectPerVertex);
        vertexShaderObjectPerVertex = null;
    }

    if (fragmentShaderObjectPerVertex) {
        gl.deleteShader(fragmentShaderObjectPerVertex);
        fragmentShaderObjectPerVertex = null;
    }

    if (shaderProgramObjectPerVertex) {
        gl.deleteProgram(shaderProgramObjectPerVertex);
        shaderProgramObjectPerVertex = null;
    }

    if (shaderProgramObjectPerFragment) {
        if (vertexShaderObjectPerFragment) {
            gl.detachShader(shaderProgramObjectPerFragment, vertexShaderObjectPerFragment);
        }

        if (fragmentShaderObjectPerFragment) {
            gl.detachShader(shaderProgramObjectPerFragment, fragmentShaderObjectPerFragment);
        }
    }

    if (vertexShaderObjectPerFragment) {
        gl.deleteShader(vertexShaderObjectPerFragment);
        vertexShaderObjectPerFragment = null;
    }

    if (fragmentShaderObjectPerFragment) {
        gl.deleteShader(fragmentShaderObjectPerFragment);
        fragmentShaderObjectPerFragment = null;
    }

    if (shaderProgramObjectPerFragment) {
        gl.deleteProgram(shaderProgramObjectPerFragment);
        shaderProgramObjectPerFragment = null;
    }

    gl.useProgram(null);
}

function degreeToRadian(degree) {
    return degree * Math.PI / 180.0;
}
