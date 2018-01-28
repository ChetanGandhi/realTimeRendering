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

int windowWidth = 800;
int windowHeight = 600;

enum
{
    CG_ATTRIBUTE_VERTEX_POSITION = 0,
	CG_ATTRIBUTE_COLOR,
	CG_ATTRIBUTE_NORMAL,
	CG_ATTRIBUTE_TEXTURE,
};

GLuint vertexShaderObject = 0;
GLuint fragmentShaderObject = 0;
GLuint shaderProgramObject = 0;

GLuint vao = 0;
GLuint vbo = 0;
GLuint mvpUniform = 0;

vmath::mat4 orthographicProjectionMatrix;

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
void cleanUp(void);
void handleEvent(XEvent *event);
void display(void);
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
        // GLX_SAMPLE_BUFFERS, 1,
        // GLX_SAMPLES, 4,
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

    XStoreName(gDisplay, window, "CG - PP - Triangle Ortho");

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

                default:
                break;
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

    const GLfloat triangleVertices[] = {
        0.0f, 50.0f, 0.0f,
        -50.0f, -50.0f, 0.0f,
        50.0f, -50.0f, 0.0f
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_VERTEX_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    orthographicProjectionMatrix = vmath::mat4::identity();

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
        "uniform mat4 mvpMatrix;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   gl_Position = mvpMatrix * vertexPosition;" \
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
        "out vec4 fragmentColor;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   fragmentColor = vec4(1.0, 1.0, 1.0, 1.0);" \
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

    // After linking get the value of MVP uniform location from the shader program.
    mvpUniform = glGetUniformLocation(shaderProgramObject, "mvpMatrix");
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramObject);

    vmath::mat4 modelViewMatrix = vmath::mat4::identity();
    vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();

	// Multiply modelViewMatrix and orthographicProjectionMatrix to get modelViewProjectionMatrix
    // Oder of multiplication is very important projectionMatrix * modelMatrix * viewMatrix
    // As we have model and view matrix combined, we just have to multiply projectionMatrix and modelViewMatrix
    modelViewProjectionMatrix = orthographicProjectionMatrix * modelViewMatrix;

    // Pass modelViewProjectionMatrix to vertex shader in 'mvpMatrix' variable defined in shader.
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    // Now bind the VAO to which we want to use
    glBindVertexArray(vao);

    // Draw the triangle
    // 3 is number of vertices in the array i.e. element count in triangleVertices divide by 3 (x, y, z) component
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // unbind the vao
    glBindVertexArray(0);

    glUseProgram(0);
    glXSwapBuffers(gDisplay, window);
}

void resize(int width, int height)
{
    if(height == 0)
    {
        height = 1;
    }

    if(width == 0)
    {
        width = 1;
    }

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    GLfloat ratio = 1.0f;

    if(width <= height)
    {
        ratio = (GLfloat)height / (GLfloat)width;
        orthographicProjectionMatrix = vmath::ortho(-100.0f, 100.0f, -100.0f * ratio, 100.0f * ratio, -100.0f, 100.0f);
    }
    else
    {
        ratio = (GLfloat)width / (GLfloat)height;
        orthographicProjectionMatrix = vmath::ortho(-100.0f * ratio, 100.0f * ratio, -100.0f, 100.0f, -100.0f, 100.0f);
    }
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
    if(vao)
    {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    if(vbo)
    {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
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
