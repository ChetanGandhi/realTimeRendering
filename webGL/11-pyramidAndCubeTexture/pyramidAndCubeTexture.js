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

let angleCube = 0.0;
let anglePyramid = 0.0;
let speed = 0.3;

let vertexShaderObject = null;
let fragmentShaderObject = null;
let shaderProgramObject = null;

let vaoPyramid = null;
let vboPyramidPosition = null;
let vboPyramidTexture = null;
let vaoCube = null;
let vboCubePosition = null;
let vboCubeTexture = null;
let mvpUniform = null;
let textureSamplerUniform = null;
let textureKundali = null;
let textureStone = null;

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

    console.log("CG | Info | WebGL2 context found.");

    gl.viewportWidth = surface.width;
    gl.viewportHeight = surface.height;

    initializeVertexShaderObject();
    initializeFragmentShaderObject();
    initializeShaderProgramObject();
    initializePyramidBuffers();
    initializeCubeBuffers();

    perspectiveProjectionMatrix = mat4.create();

    textureKundali = loadGLTextures("resources/vijay_kundali.png");
    textureStone = loadGLTextures("resources/stone.png");

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

function initializePyramidBuffers() {
    let pyramidVertices = new Float32Array([
        // Front face
        0.0,
        1.0,
        0.0,
        -1.0,
        -1.0,
        1.0,
        1.0,
        -1.0,
        1.0,

        // Right face
        0.0,
        1.0,
        0.0,
        1.0,
        -1.0,
        1.0,
        1.0,
        -1.0,
        -1.0,

        // Back face
        0.0,
        1.0,
        0.0,
        1.0,
        -1.0,
        -1.0,
        -1.0,
        -1.0,
        -1.0,

        // Left face
        0.0,
        1.0,
        0.0,
        -1.0,
        -1.0,
        -1.0,
        -1.0,
        -1.0,
        1.0
    ]);

    let pyramidTextureCoordinates = new Float32Array([
        // Front face
        0.5,
        1.0,
        0.0,
        0.0,
        1.0,
        0.0,

        // Right face
        0.5,
        1.0,
        1.0,
        0.0,
        0.0,
        0.0,

        // Back face
        0.5,
        1.0,
        1.0,
        0.0,
        0.0,
        0.0,

        // Left face
        0.5,
        1.0,
        0.0,
        0.0,
        1.0,
        0.0
    ]);

    vaoPyramid = gl.createVertexArray();
    gl.bindVertexArray(vaoPyramid);

    vboPyramidPosition = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboPyramidPosition);
    gl.bufferData(gl.ARRAY_BUFFER, pyramidVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.CG_ATTRIBUTE_VERTEX, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.CG_ATTRIBUTE_VERTEX);

    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    vboPyramidTexture = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboPyramidTexture);
    gl.bufferData(gl.ARRAY_BUFFER, pyramidTextureCoordinates, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.CG_ATTRIBUTE_TEXTURE0, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.CG_ATTRIBUTE_TEXTURE0);

    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);
}

function initializeCubeBuffers() {
    let cubeVertices = new Float32Array([
        // Top face
        1.0,
        1.0,
        -1.0,
        -1.0,
        1.0,
        -1.0,
        -1.0,
        1.0,
        1.0,
        1.0,
        1.0,
        1.0,

        // Bottom face
        1.0,
        -1.0,
        -1.0,
        -1.0,
        -1.0,
        -1.0,
        -1.0,
        -1.0,
        1.0,
        1.0,
        -1.0,
        1.0,

        // Front face
        1.0,
        1.0,
        1.0,
        -1.0,
        1.0,
        1.0,
        -1.0,
        -1.0,
        1.0,
        1.0,
        -1.0,
        1.0,

        // Back face
        1.0,
        1.0,
        -1.0,
        -1.0,
        1.0,
        -1.0,
        -1.0,
        -1.0,
        -1.0,
        1.0,
        -1.0,
        -1.0,

        // Right face
        1.0,
        1.0,
        -1.0,
        1.0,
        1.0,
        1.0,
        1.0,
        -1.0,
        1.0,
        1.0,
        -1.0,
        -1.0,

        // Left face
        -1.0,
        1.0,
        1.0,
        -1.0,
        1.0,
        -1.0,
        -1.0,
        -1.0,
        -1.0,
        -1.0,
        -1.0,
        1.0
    ]);

    let cubeTextureCoordinates = new Float32Array([
        // Top face
        0.0,
        1.0,
        0.0,
        0.0,
        1.0,
        0.0,
        1.0,
        1.0,

        // Bottom face
        1.0,
        1.0,
        0.0,
        1.0,
        0.0,
        0.0,
        1.0,
        0.0,

        // Front face
        0.0,
        0.0,
        1.0,
        0.0,
        1.0,
        1.0,
        0.0,
        1.0,

        // Back face
        1.0,
        0.0,
        1.0,
        1.0,
        0.0,
        1.0,
        0.0,
        0.0,

        // right face
        1.0,
        0.0,
        1.0,
        1.0,
        0.0,
        1.0,
        0.0,
        0.0,

        // left face
        0.0,
        0.0,
        1.0,
        0.0,
        1.0,
        1.0,
        0.0,
        1.0
    ]);

    vaoCube = gl.createVertexArray();
    gl.bindVertexArray(vaoCube);

    vboCubePosition = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboCubePosition);
    gl.bufferData(gl.ARRAY_BUFFER, cubeVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.CG_ATTRIBUTE_VERTEX, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.CG_ATTRIBUTE_VERTEX);

    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    vboCubeTexture = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboCubeTexture);
    gl.bufferData(gl.ARRAY_BUFFER, cubeTextureCoordinates, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.CG_ATTRIBUTE_TEXTURE0, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.CG_ATTRIBUTE_TEXTURE0);

    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);
}

function update() {
    angleCube -= speed;
    anglePyramid += speed;

    if (angleCube <= -360.0) {
        angleCube = 0.0;
    }

    if (anglePyramid >= 360.0) {
        anglePyramid = 0.0;
    }
}

function display() {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.useProgram(shaderProgramObject);

    drawPyramid();
    drawCube();

    gl.useProgram(null);

    update();
    displayAnimationRequestId = requestAnimationFrame(display);
}

function drawPyramid() {
    let modelViewMatrix = mat4.create();
    let rotationMatrix = mat4.create();
    let modelViewProjectionMatrix = mat4.create();

    mat4.translate(modelViewMatrix, modelViewMatrix, [-1.5, 0.0, -6.0]);
    mat4.rotateY(rotationMatrix, rotationMatrix, degreeToRadian(anglePyramid));

    mat4.multiply(modelViewMatrix, modelViewMatrix, rotationMatrix);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
    gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

    gl.bindVertexArray(vaoPyramid);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, textureStone);
    gl.uniform1i(textureSamplerUniform, 0);
    gl.drawArrays(gl.TRIANGLES, 0, 12);

    gl.bindVertexArray(null);
}

function drawCube() {
    let modelViewMatrix = mat4.create();
    let rotationMatrix = mat4.create();
    let scaleMatrix = mat4.create();
    let modelViewProjectionMatrix = mat4.create();

    mat4.scale(scaleMatrix, scaleMatrix, [0.75, 0.75, 0.75]);
    mat4.translate(modelViewMatrix, modelViewMatrix, [1.5, 0.0, -6.0]);
    mat4.rotateX(rotationMatrix, rotationMatrix, degreeToRadian(angleCube));
    mat4.rotateY(rotationMatrix, rotationMatrix, degreeToRadian(angleCube));
    mat4.rotateZ(rotationMatrix, rotationMatrix, degreeToRadian(angleCube));

    mat4.multiply(modelViewMatrix, modelViewMatrix, scaleMatrix);
    mat4.multiply(modelViewMatrix, modelViewMatrix, rotationMatrix);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
    gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

    gl.bindVertexArray(vaoCube);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, textureKundali);
    gl.uniform1i(textureSamplerUniform, 0);
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);

    gl.bindVertexArray(null);
}

function loadGLTextures(resourcePath) {
    let texture = gl.createTexture();

    // Because images have to be download over the internet, they might take a moment until they are ready.
    // Until then put a single pixel in the texture so we can use it immediately.
    // When the image has finished downloading we'll update the texture with the contents of the image.
    let pixel = new Uint8Array([0, 0, 0, 0]);
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, pixel);

    let image = new Image();
    image.src = resourcePath;
    image.onload = function() {
        gl.bindTexture(gl.TEXTURE_2D, texture);
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR);
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB, image.width, image.height, 0, gl.RGB, gl.UNSIGNED_BYTE, image);
        gl.generateMipmap(gl.TEXTURE_2D);
        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    gl.bindTexture(gl.TEXTURE_2D, null);
    return texture;
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

    if (vboPyramidTexture) {
        gl.deleteBuffer(vboPyramidTexture);
        vboPyramidTexture = null;
    }

    if (vaoCube) {
        gl.deleteVertexArray(vaoCube);
        vaoCube = null;
    }

    if (vboCubePosition) {
        gl.deleteBuffer(vboCubePosition);
        vboCubePosition = null;
    }

    if (vboCubeTexture) {
        gl.deleteBuffer(vboCubeTexture);
        vboCubeTexture = null;
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

    if (textureStone) {
        gl.deleteTexture(textureStone);
        textureStone = null;
    }

    if (textureKundali) {
        gl.deleteTexture(textureKundali);
        textureKundali = null;
    }
}

function degreeToRadian(degree) {
    return degree * Math.PI / 180.0;
}

function isPowerOf2(value) {
    return (value & (value - 1)) == 0;
}
