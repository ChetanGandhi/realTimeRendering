#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include "vmath.h"

bool isFullscreen = false;
bool isCloseButtonClicked = false;
bool isEscapeKeyPressed = false;
bool isLightingEnabled = false;
bool isAnimationEnabled = false;

int windowWidth = 800;
int windowHeight = 600;

enum
{
    CG_ATTRIBUTE_VERTEX_POSITION = 0,
	CG_ATTRIBUTE_COLOR,
	CG_ATTRIBUTE_NORMAL,
	CG_ATTRIBUTE_TEXTURE0,
};

GLuint vertexShaderObject = 0;
GLuint fragmentShaderObject = 0;
GLuint shaderProgramObject = 0;

GLuint vaoPyramid = 0;
GLuint vboPyramidPosition = 0;
GLuint vboPyramidNormal = 0;
GLuint modelMatrixUniform = 0;
GLuint viewMatrixUniform = 0;
GLuint projectionMatrixUniform = 0;
GLuint laZeroUniform = 0;
GLuint ldZeroUniform = 0;
GLuint lsZeroUniform = 0;
GLuint laOneUniform = 0;
GLuint ldOneUniform = 0;
GLuint lsOneUniform = 0;
GLuint kaUniform = 0;
GLuint kdUniform = 0;
GLuint ksUniform = 0;
GLuint materialShininessUniform = 0;
GLuint lightZeroPositionUniform = 0;
GLuint lightOnePositionUniform = 0;
GLuint isLightingEnabledUniform = 0;

vmath::mat4 perspectiveProjectionMatrix;

GLfloat anglePyramid = 0.0f;
GLfloat speed = 0.01f;

GLfloat lightZeroAmbient[] = {0.0f, 0.0f, 0.0f, 0.0f};
GLfloat lightZeroDiffuse[] = {1.0f, 0.0f, 0.0f, 0.0f};
GLfloat lightZeroSpecular[] = {1.0f, 0.0f, 0.0f, 0.0f};
GLfloat lightZeroPosition[] = {2.0f, 1.0f, 1.0f, 0.0f};

GLfloat lightOneAmbient[] = {0.0f, 0.0f, 0.0f, 0.0f};
GLfloat lightOneDiffuse[] = {0.0f, 0.0f, 1.0f, 0.0f};
GLfloat lightOneSpecular[] = {0.0f, 0.0f, 1.0f, 0.0f};
GLfloat lightOnePosition[] = {-2.0f, 1.0f, 1.0f, 0.0f};

GLfloat materialAmbient[] = {0.0f, 0.0f, 0.0f, 0.0f};
GLfloat materialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialSpecular[] = {1.0f, 1.0f, 1.0f, 0.0f};
GLfloat materialShininess = 50.0f;

FILE *logFile = NULL;

Display *gDisplay = NULL;
XVisualInfo *visualInfo = NULL;
Colormap colormap;
Window window;
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *display, GLXFBConfig fbConfig, GLXContext sharedContext, Bool direct, const int* attributes);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
GLXFBConfig glxFBConfig;
GLXContext glxContext;

void createWindow(void);
void initialize(void);
void listExtensions(void);
void initializeVertexShader(void);
void initializeFragmentShader(void);
void initializeShaderProgram(void);
void initializePyramidBuffers(void);
void cleanUp(void);
void handleEvent(XEvent *event);
void update(void);
void display(void);
void drawPyramid(void);
void resize(int width, int height);
void toggleFullscreen(bool isFullscreen);

int main(void)
{
    logFile = fopen("debug.log", "w");
    if(logFile == NULL)
    {
        fprintf(logFile, "Error: Unable to create log file.");
        exit(EXIT_FAILURE);
    }

    fprintf(logFile, "---------- CG: OpenGL Debug Logs Start ----------\n");
    fflush(logFile);

    XEvent event;
    bool done = false;

    createWindow();
    initialize();

    while(!done)
    {
        while(XPending(gDisplay))
        {
            XNextEvent(gDisplay, &event);
            handleEvent(&event);
        }

        done = (isCloseButtonClicked || isEscapeKeyPressed);
        update();
        display();
    }

    cleanUp();

    fprintf(logFile, "---------- CG: OpenGL Debug Logs End ----------\n");
    fflush(logFile);
    fclose(logFile);

    return EXIT_SUCCESS;
}

void createWindow(void)
{
    XSetWindowAttributes windowAttributes;     // This is an array of all available FBConfig matching to frameBufferAttributes.
    GLXFBConfig *pGLXFBConfigs = NULL;
    XVisualInfo *pTempXVisualInfo = NULL;
    int numberOfFBConfigs = 0;
    int defaultScreen = 0;
    int styleMask = 0;
    int counter = 0;

    static int frameBufferAttributes[] = {
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        GLX_DOUBLEBUFFER, True,
        GLX_X_RENDERABLE, True,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_SAMPLE_BUFFERS, 1,
        GLX_SAMPLES, 4,
        None
    };

    gDisplay = XOpenDisplay(NULL);

    if(gDisplay == NULL)
    {
        fprintf(logFile, "Error: Unable to open XgDisplay.\n");
        cleanUp();
        exit(EXIT_FAILURE);
    }

    defaultScreen = XDefaultScreen(gDisplay);

    // Get the frame buffer configuration list as per our requirement specified in frameBufferAttributes array.
    pGLXFBConfigs = glXChooseFBConfig(gDisplay, defaultScreen, frameBufferAttributes, &numberOfFBConfigs);

    if(pGLXFBConfigs == NULL)
    {
        fprintf(logFile, "Error: Unable to get valid frame buffer configurations.\n");
        cleanUp();
        exit(EXIT_FAILURE);
    }

    fprintf(logFile, "Total frame buffer configurations: %d\n", numberOfFBConfigs);

    // Select that FBConfig/visual which have the maximum samples per pixel.
    int bestFrameBufferConfig = -1;
    int worstFrameBufferConfig = -1;
    int bestNumberOfSamples = -1;
    int worstNumberOfSamples = 999;

    for(counter = 0; counter < numberOfFBConfigs; ++counter)
    {
        pTempXVisualInfo = glXGetVisualFromFBConfig(gDisplay, pGLXFBConfigs[counter]);

        if(pTempXVisualInfo)
        {
            int sampleBuffer = 0;
            int samples = 0;

            glXGetFBConfigAttrib(gDisplay, pGLXFBConfigs[counter], GLX_SAMPLE_BUFFERS, &sampleBuffer);
            glXGetFBConfigAttrib(gDisplay, pGLXFBConfigs[counter], GLX_SAMPLES, &samples);

            fprintf(logFile, "----------------------------------------\n");
            fprintf(logFile, "Frame Buffer Config: %d\n", counter);
            fprintf(logFile, "Visual ID: 0x%lu\n", pTempXVisualInfo->visualid);
            fprintf(logFile, "Sample Buffers: %d\n", sampleBuffer);
            fprintf(logFile, "Samples: %d\n", samples);
            fprintf(logFile, "----------------------------------------\n");

            if(bestFrameBufferConfig < 0 || sampleBuffer && samples > bestNumberOfSamples)
            {
                bestFrameBufferConfig = counter;
                bestNumberOfSamples = samples;
            }

            if(worstFrameBufferConfig < 0 || !sampleBuffer || samples < worstNumberOfSamples)
            {
                worstFrameBufferConfig = counter;
                worstNumberOfSamples = samples;
            }
        }

        XFree(pTempXVisualInfo);
    }

    // Set the global glxFBConfig to best found.
    glxFBConfig = pGLXFBConfigs[bestFrameBufferConfig];

    // Free the frame buffer configuration list.
    XFree(pGLXFBConfigs);

    visualInfo = glXGetVisualFromFBConfig(gDisplay, glxFBConfig);

    if(visualInfo == NULL)
    {
        fprintf(logFile, "Error: Unable to choose visual.\n");
        cleanUp();
        exit(EXIT_FAILURE);
    }

    fprintf(logFile, "----------------------------------------\n");
    fprintf(logFile, "Selected Visual ID: 0x%lu\n", visualInfo->visualid);
    fprintf(logFile, "----------------------------------------\n");

    windowAttributes.border_pixel = 0;
    windowAttributes.border_pixmap = 0;
    windowAttributes.background_pixel = BlackPixel(gDisplay, defaultScreen);
    windowAttributes.background_pixmap = 0;
    windowAttributes.colormap = XCreateColormap(gDisplay, XRootWindow(gDisplay, visualInfo->screen), visualInfo->visual, AllocNone);
    windowAttributes.event_mask = ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;

    styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
    colormap = windowAttributes.colormap;

    window = XCreateWindow(gDisplay, XRootWindow(gDisplay, visualInfo->screen), 0, 0, windowWidth, windowHeight, 0, visualInfo->depth, InputOutput, visualInfo->visual, styleMask, &windowAttributes);

    if(!window)
    {
        fprintf(logFile, "Error: Failed to create main window.");
        cleanUp();
        exit(EXIT_FAILURE);
    }

    XStoreName(gDisplay, window, "CG - PP - Pyramid with Multiple Light");

    Atom windowManagerDelete = XInternAtom(gDisplay, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(gDisplay, window, &windowManagerDelete, 1);
    XMapWindow(gDisplay, window);
}

void handleEvent(XEvent *event)
{
    KeySym keySymbol;
    char asciiChars[32] = {0};

    switch(event->type)
    {
        case MapNotify: // Window created.
        break;

        case KeyPress: // Key events.
            keySymbol = XkbKeycodeToKeysym(gDisplay, event->xkey.keycode, 0, 0);

            switch(keySymbol)
            {
                case XK_Escape:
                    isEscapeKeyPressed = true;
                break;

                default:
                break;
            }

            XLookupString(&event->xkey, asciiChars, sizeof(asciiChars), NULL, NULL);
            switch(asciiChars[0])
            {
                case 'F':
                case 'f':
                    isFullscreen = !isFullscreen;
                    toggleFullscreen(isFullscreen);
                break;

                case 'A':
                case 'a':
                    isAnimationEnabled = !isAnimationEnabled;
                break;

                case 'L':
                case 'l':
                    isLightingEnabled = !isLightingEnabled;
                break;

                default:
                break;
            }

            if(keySymbol > XK_0 && keySymbol <= XK_9)
            {
                speed = 0.1f * (keySymbol - XK_0);
            }

        break;

        case ButtonPress: // Mouse event.
            switch(event->xbutton.button)
            {
                case 1: // Left button.
                break;

                case 2: // Middle button.
                break;

                case 3: // Right button.
                break;

                case 4: // Mouse wheel up.
                break;

                case 5: // Mouse wheel down.
                break;
            }

        break;

        case MotionNotify: // Mouse move event.
        break;

        case ConfigureNotify: // Window configuration change, like resize.
            windowWidth = event->xconfigure.width;
            windowHeight = event->xconfigure.height;
            resize(windowWidth, windowHeight);
        break;

        case Expose: // Paint window.
        break;

        case DestroyNotify: // Window destroyed, cleanUp resources.
        break;

        case 33: // Windows close button event.
            isCloseButtonClicked = true;
        break;

        default:
        break;
    }
}

void initialize(void)
{
    // Create new GL context 4.5 for rendering.
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte *)"glXCreateContextAttribsARB");

    if(glXCreateContextAttribsARB == NULL)
    {
        fprintf(logFile, "Error: Unable to get glXCreateContextAttribsARBProc address.");
        cleanUp();
        exit(EXIT_FAILURE);
    }

    GLint attributes[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
        GLX_CONTEXT_MINOR_VERSION_ARB, 5,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
        None
    };

    glxContext = glXCreateContextAttribsARB(gDisplay, glxFBConfig, 0, True, attributes);

    // Not able to get context for OpenGL version 4.5, fallback to old version.
    if(!glxContext)
    {
        fprintf(logFile, "Failed to get GLX Context for OpenGL version 4.5, using old version.\n");

        // When a context version below 3.0 is requested, implementation will return
        // the newest context version compatible with OpenGL version less than 3.0
        GLint attributes[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
            GLX_CONTEXT_MINOR_VERSION_ARB, 0,
            None
        };

        glxContext = glXCreateContextAttribsARB(gDisplay, glxFBConfig, 0, True, attributes);
    }
    else
    {
        fprintf(logFile, "GLX Context for OpenGL version 4.5 is created.\n");
    }

    // Check do we have a direct context so that we get hardware rendering.
    if(!glXIsDirect(gDisplay, glxContext))
    {
        fprintf(logFile, "Indirect GLX rendering context is used.\n");
    }
    else
    {
        fprintf(logFile, "Direct GLX rendering context is used, hardware rendering is available.\n");
    }

    glXMakeCurrent(gDisplay, window, glxContext);

    GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
        fprintf(logFile, "Cannot initialize GLEW, Error: %d", glewError);
        fflush(logFile);

        cleanUp();
        exit(EXIT_FAILURE);
	}

    listExtensions();

    // Initialize the shaders and shader program object.
    initializeVertexShader();
    initializeFragmentShader();
    initializeShaderProgram();
    initializePyramidBuffers();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);

    // Disable face culling to see back side of object when rotated.
    // glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);

    perspectiveProjectionMatrix = vmath::mat4::identity();

    isAnimationEnabled = false;
    isLightingEnabled = false;

    resize(windowWidth, windowHeight);
}

void listExtensions()
{
    GLint extensionCount = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);

    fprintf(logFile, "Number of extensions: %d\n", extensionCount);
    fflush(logFile);

    for(int counter = 0; counter < extensionCount; ++counter)
    {
        fprintf(logFile, "%d] Extension name: %s\n", counter + 1, (const char*)glGetStringi(GL_EXTENSIONS, counter));
        fflush(logFile);
    }
}

void initializeVertexShader()
{
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    const GLchar *vertexShaderCode = "#version 450 core" \
        "\n" \
        "in vec4 vertexPosition;" \
        "in vec3 vertexNormal;" \
        "\n" \
        "out vec3 tNormal;" \
		"out vec3 lightZeroDirection;" \
		"out vec3 lightOneDirection;" \
		"out vec3 viewVector;" \
		"\n" \
        "uniform mat4 modelMatrix;" \
        "uniform mat4 viewMatrix;" \
        "uniform mat4 projectionMatrix;" \
        "uniform vec4 lightZeroPosition;" \
        "uniform vec4 lightOnePosition;" \
        "uniform int isLightingEnabled;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   if(isLightingEnabled == 1)" \
        "   {" \
        "       vec4 eyeCoordinates = viewMatrix * modelMatrix * vertexPosition;" \
        "       tNormal = mat3(viewMatrix * modelMatrix) * vertexNormal;" \
        "       lightZeroDirection = vec3(lightZeroPosition) - eyeCoordinates.xyz;" \
        "       lightOneDirection = vec3(lightOnePosition) - eyeCoordinates.xyz;" \
        "       viewVector = -eyeCoordinates.xyz;" \
        "   }" \
        "\n" \
        "   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;" \
        "}";

    glShaderSource(vertexShaderObject, 1, (const char**)&vertexShaderCode, NULL);
    glCompileShader(vertexShaderObject);

    GLint infoLogLength = 0;
    GLint shaderCompileStatus = 0;
    char *infoLog = NULL;

    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, infoLog);
                fprintf(logFile, "CG: Vertex shader compilation log: %s\n", infoLog);
                free(infoLog);
                cleanUp();
                exit(EXIT_FAILURE);
            }
        }
    }
}

void initializeFragmentShader()
{
    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar *fragmentShaderCode = "#version 450 core" \
        "\n" \
		"in vec3 tNormal;" \
		"in vec3 lightZeroDirection;" \
		"in vec3 lightOneDirection;" \
		"in vec3 viewVector;" \
        "\n" \
        "out vec4 fragmentColor;" \
        "\n" \
        "uniform int isLightingEnabled;" \
        "uniform vec3 laZero;" \
        "uniform vec3 ldZero;" \
        "uniform vec3 lsZero;" \
        "uniform vec3 laOne;" \
        "uniform vec3 ldOne;" \
        "uniform vec3 lsOne;" \
        "uniform vec3 ka;" \
        "uniform vec3 kd;" \
        "uniform vec3 ks;" \
        "uniform float materialShininess;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   vec3 phongAdsColor;" \
        "   if(isLightingEnabled == 1)" \
        "   {" \
        "       vec3 normalizedTNormal = normalize(tNormal);" \
        "       vec3 normalizedViewVector = normalize(viewVector);" \
        "\n" \
        "       vec3 normalizedLightZeroDirection = normalize(lightZeroDirection);" \
        "       float tNormalDotLightZeroDirection = max(dot(normalizedTNormal, normalizedLightZeroDirection), 0.0);" \
        "       vec3 ambientLightZero = laZero * ka;" \
        "       vec3 diffuseLightZero = ldZero * kd * tNormalDotLightZeroDirection;" \
        "       vec3 reflectionVectorLightZero = reflect(-normalizedLightZeroDirection, normalizedTNormal);" \
        "       vec3 specularLightZero = lsZero * ks * pow(max(dot(reflectionVectorLightZero, normalizedViewVector), 0.0), materialShininess);" \
        "\n" \
        "       vec3 normalizedLightOneDirection = normalize(lightOneDirection);" \
        "       float tNormalDotLightOneDirection = max(dot(normalizedTNormal, normalizedLightOneDirection), 0.0);" \
        "       vec3 ambientLightOne = laOne * ka;" \
        "       vec3 diffuseLightOne = ldOne * kd * tNormalDotLightOneDirection;" \
        "       vec3 reflectionVectorLightOne = reflect(-normalizedLightOneDirection, normalizedTNormal);" \
        "       vec3 specularLightOne = lsOne * ks * pow(max(dot(reflectionVectorLightOne, normalizedViewVector), 0.0), materialShininess);" \
        "\n" \
        "       phongAdsColor = ambientLightZero + diffuseLightZero + specularLightZero + ambientLightOne + diffuseLightOne + specularLightOne;" \
        "   }" \
        "   else" \
        "   {" \
        "       phongAdsColor = vec3(1.0, 1.0, 1.0);" \
        "   }" \
        "\n" \
        "   fragmentColor = vec4(phongAdsColor, 1.0);" \
        "}";

    glShaderSource(fragmentShaderObject, 1, (const char**)&fragmentShaderCode, NULL);
    glCompileShader(fragmentShaderObject);

    GLint infoLogLength = 0;
    GLint shaderCompileStatus = 0;
    char *infoLog = NULL;

    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, infoLog);
                fprintf(logFile, "CG: Fragment shader compilation log: %s\n", infoLog);
                free(infoLog);
                cleanUp();
                exit(EXIT_FAILURE);
            }
        }
    }
}

void initializeShaderProgram()
{
    shaderProgramObject = glCreateProgram();
    glAttachShader(shaderProgramObject, vertexShaderObject);
    glAttachShader(shaderProgramObject, fragmentShaderObject);

    // Bind the position attribute location before linking.
    glBindAttribLocation(shaderProgramObject, CG_ATTRIBUTE_VERTEX_POSITION, "vertexPosition");

    // Bind the normal attribute location before linking.
    glBindAttribLocation(shaderProgramObject, CG_ATTRIBUTE_NORMAL, "vertexNormal");

    // Now link and check for error.
    glLinkProgram(shaderProgramObject);

    GLint infoLogLength = 0;
    GLint shaderProgramLinkStatus = 0;
    char *infoLog = NULL;

    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &shaderProgramLinkStatus);

    if(shaderProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetProgramInfoLog(shaderProgramObject, infoLogLength, &written, infoLog);
                fprintf(logFile, "CG: Shader program link log: %s\n", infoLog);
                free(infoLog);
                cleanUp();
                exit(EXIT_FAILURE);
            }
        }
    }

    // After linking get the value of modelView and projection uniform location from the shader program.
    modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "modelMatrix");
    viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "viewMatrix");
    projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "projectionMatrix");

    isLightingEnabledUniform = glGetUniformLocation(shaderProgramObject, "isLightingEnabled");

    laZeroUniform = glGetUniformLocation(shaderProgramObject, "laZero");
    ldZeroUniform = glGetUniformLocation(shaderProgramObject, "ldZero");
    lsZeroUniform = glGetUniformLocation(shaderProgramObject, "lsZero");
    laOneUniform = glGetUniformLocation(shaderProgramObject, "laOne");
    ldOneUniform = glGetUniformLocation(shaderProgramObject, "ldOne");
    lsOneUniform = glGetUniformLocation(shaderProgramObject, "lsOne");
    kaUniform = glGetUniformLocation(shaderProgramObject, "ka");
    kdUniform = glGetUniformLocation(shaderProgramObject, "kd");
    ksUniform = glGetUniformLocation(shaderProgramObject, "ks");
    lightZeroPositionUniform = glGetUniformLocation(shaderProgramObject, "lightZeroPosition");
    lightOnePositionUniform = glGetUniformLocation(shaderProgramObject, "lightOnePosition");
    materialShininessUniform = glGetUniformLocation(shaderProgramObject, "materialShininess");
}

void initializePyramidBuffers()
{
    const GLfloat pyramidVertices[] = {
        // Front face
        0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,

        // Right face
        0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,

        // Back face
        0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,

        // Left face
        0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f
    };

    const GLfloat pyramidNormals[] = {
        // Front face
        0.0f, 0.447214f, 0.894427f,
        0.0f, 0.447214f, 0.894427f,
        0.0f, 0.447214f, 0.894427f,

        // Right face
        0.894427f, 0.447214f, 0.0f,
        0.894427f, 0.447214f, 0.0f,
        0.894427f, 0.447214f, 0.0f,

        // Back face
        0.0f, 0.447214f, -0.894427f,
        0.0f, 0.447214f, -0.894427f,
        0.0f, 0.447214f, -0.894427f,

        // Left face
        -0.894427f, 0.447214f, 0.0f,
        -0.894427f, 0.447214f, 0.0f,
        -0.894427f, 0.447214f, 0.0f
    };

    glGenVertexArrays(1, &vaoPyramid);
    glBindVertexArray(vaoPyramid);

    glGenBuffers(1, &vboPyramidPosition);
    glBindBuffer(GL_ARRAY_BUFFER, vboPyramidPosition);

    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_VERTEX_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboPyramidNormal);
    glBindBuffer(GL_ARRAY_BUFFER, vboPyramidNormal);

    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_NORMAL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void update(void)
{
    if(isAnimationEnabled)
    {
        anglePyramid += speed;

        if(anglePyramid >= 360.0f)
        {
            anglePyramid = 0.0f;
        }
    }
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glUseProgram(shaderProgramObject);

    if(isLightingEnabled)
    {
        glUniform1i(isLightingEnabledUniform, 1);

        glUniform3fv(laZeroUniform, 1, lightZeroAmbient);
        glUniform3fv(ldZeroUniform, 1, lightZeroDiffuse);
        glUniform3fv(lsZeroUniform, 1, lightZeroSpecular);

        glUniform3fv(laOneUniform, 1, lightOneAmbient);
        glUniform3fv(ldOneUniform, 1, lightOneDiffuse);
        glUniform3fv(lsOneUniform, 1, lightOneSpecular);

        glUniform4fv(lightZeroPositionUniform, 1, lightZeroPosition);
        glUniform4fv(lightOnePositionUniform, 1, lightOnePosition);

        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDiffuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        glUniform1f(materialShininessUniform, materialShininess);
    }
    else
    {
        glUniform1i(isLightingEnabledUniform, 0);
    }

    drawPyramid();

    glUseProgram(0);
    glXSwapBuffers(gDisplay, window);
}

void drawPyramid()
{
    vmath::mat4 modelMatrix = vmath::mat4::identity();
    vmath::mat4 rotationMatrix = vmath::mat4::identity();
    vmath::mat4 viewMatrix = vmath::mat4::identity();

    // Translate the modal matrix.
    modelMatrix = vmath::translate(0.0f, 0.0f, -6.0f);

    // Rotate the modal matrix
    rotationMatrix = vmath::rotate(anglePyramid, 0.0f, 1.0f, 0.0f);

    modelMatrix = modelMatrix * rotationMatrix;

    // Pass modelMatrix to vertex shader in 'modelMatrix' variable defined in shader.
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

    // Pass viewMatrix to vertex shader in 'viewMatrix' variable defined in shader.
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);

    // Pass perspectiveProjectionMatrix to vertex shader in 'projectionMatrix' variable defined in shader.
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

    // Now bind the VAO to which we want to use
    glBindVertexArray(vaoPyramid);

    // Draw the pyramid
    // 12 is number of vertices in the array i.e. element count in pyramidVertices divide by 3 (x, y, z) component
    glDrawArrays(GL_TRIANGLES, 0, 12);

    // unbind the vao
    glBindVertexArray(0);
}

void resize(int width, int height)
{
    if(height == 0)
    {
        height = 1;
    }

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);
}

void toggleFullscreen(bool isFullscreen)
{
    Atom wmStateNormal;
    Atom wmStateFullscreen;
    XEvent event = {0};

    wmStateNormal = XInternAtom(gDisplay, "_NET_WM_STATE", False);
    wmStateFullscreen = XInternAtom(gDisplay, "_NET_WM_STATE_FULLSCREEN", False);

    memset(&event, 0, sizeof(event));
    event.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = wmStateNormal;
    event.xclient.format = 32;
    event.xclient.data.l[0] = isFullscreen ? 1 : 0;
    event.xclient.data.l[1] = wmStateFullscreen;

    XSendEvent(gDisplay, XRootWindow(gDisplay, visualInfo->screen), False, StructureNotifyMask, &event);
}

void cleanUp(void)
{
    if(vaoPyramid)
    {
        glDeleteVertexArrays(1, &vaoPyramid);
        vaoPyramid = 0;
    }

    if(vboPyramidPosition)
    {
        glDeleteBuffers(1, &vboPyramidPosition);
        vboPyramidPosition = 0;
    }

    if(vboPyramidNormal)
    {
        glDeleteBuffers(1, &vboPyramidNormal);
        vboPyramidNormal = 0;
    }

    if(shaderProgramObject)
    {
        if(vertexShaderObject)
        {
            glDetachShader(shaderProgramObject, vertexShaderObject);
        }

        if(fragmentShaderObject)
        {
            glDetachShader(shaderProgramObject, fragmentShaderObject);
        }
    }

    if(vertexShaderObject)
    {
        glDeleteShader(vertexShaderObject);
        vertexShaderObject = 0;
    }

    if(fragmentShaderObject)
    {
        glDeleteShader(fragmentShaderObject);
        fragmentShaderObject = 0;
    }

    if(shaderProgramObject)
    {
        glDeleteProgram(shaderProgramObject);
        shaderProgramObject = 0;
    }

    glUseProgram(0);

    GLXContext currentContext = glXGetCurrentContext();

    if(currentContext != NULL && currentContext == glxContext)
    {
        glXMakeCurrent(gDisplay, 0, 0);
    }

    if(glxContext)
    {
        glXDestroyContext(gDisplay, glxContext);
    }

    if(window)
    {
        XDestroyWindow(gDisplay, window);
    }

    if(colormap)
    {
        XFreeColormap(gDisplay, colormap);
    }

    if(visualInfo)
    {
        free(visualInfo);
        visualInfo = NULL;
    }

    if(gDisplay)
    {
        XCloseDisplay(gDisplay);
        gDisplay = NULL;
    }
}
