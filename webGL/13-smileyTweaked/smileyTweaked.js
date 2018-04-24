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

let vaoSmiley = null;
let vboSmileyPosition = null;
let vboSmileyTexture = null;
let mvpUniform = null;
let textureSamplerUniform = null;
let textureSmiley = null;
let textureWhiteColor = null;
let textureMode = 0;

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
            textureMode = 1;
            break;

        case "2":
            textureMode = 2;
            break;

        case "3":
            textureMode = 3;
            break;

        case "4":
            textureMode = 4;
            break;

        default:
            textureMode = 0;
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
    initializeSmileyBuffers();

    perspectiveProjectionMatrix = mat4.create();

    textureSmiley = loadGLTextures("resources/smiley.png");
    makeWhiteColorTexture();

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

function initializeSmileyBuffers() {
    let smileyVertices = new Float32Array([1.0, 1.0, 1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0]);
    let smileyTextureCoordinates = new Float32Array([1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0]);

    vaoSmiley = gl.createVertexArray();
    gl.bindVertexArray(vaoSmiley);

    vboSmileyPosition = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboSmileyPosition);
    gl.bufferData(gl.ARRAY_BUFFER, smileyVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.CG_ATTRIBUTE_VERTEX, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.CG_ATTRIBUTE_VERTEX);

    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    vboSmileyTexture = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboSmileyTexture);
    gl.bufferData(gl.ARRAY_BUFFER, smileyTextureCoordinates, gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.CG_ATTRIBUTE_TEXTURE0, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.CG_ATTRIBUTE_TEXTURE0);

    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);
}

function display() {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.useProgram(shaderProgramObject);

    drawSmiley();

    gl.useProgram(null);

    displayAnimationRequestId = requestAnimationFrame(display);
}

function drawSmiley() {
    let modelViewMatrix = mat4.create();
    let modelViewProjectionMatrix = mat4.create();

    mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -6.0]);

    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
    gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

    gl.bindVertexArray(vaoSmiley);

    // As we are using dynamic draw, pass the coordinates here as per texture mode.
    let smileyTextureCoordinates = null;
    let currentTexture = textureSmiley;

    if (textureMode == 1) {
        smileyTextureCoordinates = new Float32Array([0.5, 0.5, 0.0, 0.5, 0.0, 1.0, 0.5, 1.0]);
    } else if (textureMode == 2) {
        smileyTextureCoordinates = new Float32Array([1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0]);
    } else if (textureMode == 3) {
        smileyTextureCoordinates = new Float32Array([2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.0, 2.0]);
    } else if (textureMode == 4) {
        smileyTextureCoordinates = new Float32Array([0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5]);
    } else {
        currentTexture = textureWhiteColor;
        smileyTextureCoordinates = new Float32Array([1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0]);
    }

    gl.bindBuffer(gl.ARRAY_BUFFER, vboSmileyTexture);
    gl.bufferData(gl.ARRAY_BUFFER, smileyTextureCoordinates, gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, currentTexture);
    gl.uniform1i(textureSamplerUniform, 0);
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

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

function makeWhiteColorTexture() {
    textureWhiteColor = gl.createTexture();

    let whitePixel = new Uint8Array([1.0, 1.0, 1.0, 1.0]);

    gl.bindTexture(gl.TEXTURE_2D, textureWhiteColor);
    gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, whitePixel);
    gl.generateMipmap(gl.TEXTURE_2D);
    gl.bindTexture(gl.TEXTURE_2D, null);
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

    if (vaoSmiley) {
        gl.deleteVertexArray(vaoSmiley);
        vaoSmiley = null;
    }

    if (vboSmileyPosition) {
        gl.deleteBuffer(vboSmileyPosition);
        vboSmileyPosition = null;
    }

    if (vboSmileyTexture) {
        gl.deleteBuffer(vboSmileyTexture);
        vboSmileyTexture = null;
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

    if (textureSmiley) {
        gl.deleteTexture(textureSmiley);
        textureSmiley = null;
    }

    if (textureWhiteColor) {
        gl.deleteTexture(textureWhiteColor);
        textureWhiteColor = null;
    }
}
