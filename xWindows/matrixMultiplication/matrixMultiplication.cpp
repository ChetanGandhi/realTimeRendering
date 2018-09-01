#define _USE_MATH_DEFINES

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <math.h>

bool isFullscreen = false;
bool isCloseButtonClicked = false;
bool isEscapeKeyPressed = false;

int windowWidth = 800;
int windowHeight = 600;

GLfloat angleCube = 0.0f;
GLfloat anglePyramid = 0.0f;
GLfloat speed = 0.1f;

GLfloat identityMatrix[16] = {0};
GLfloat translationMatrix[16] = {0};
GLfloat scaleMatrix[16] = {0};
GLfloat rotationXMatrix[16] = {0};
GLfloat rotationYMatrix[16] = {0};
GLfloat rotationZMatrix[16] = {0};

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
void initializeIdentityMatrix(void);
void initializeScaleMatrix(void);
void cleanup(void);
void handleEvent(XEvent *event);
void update(void);
void display(void);
void drawCube(void);
void drawPyramid(void);
void resize(int width, int height);
void toggleFullscreen(bool isFullscreen);

int main(void)
{
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

    cleanup();
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
        printf("Error: Unable to open XgDisplay.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    defaultScreen = XDefaultScreen(gDisplay);

    // Get the frame buffer configuration list as per our requirement specified in frameBufferAttributes array.
    pGLXFBConfigs = glXChooseFBConfig(gDisplay, defaultScreen, frameBufferAttributes, &numberOfFBConfigs);

    if(pGLXFBConfigs == NULL)
    {
        printf("Error: Unable to get valid frame buffer configurations.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    printf("Total frame buffer configurations: %d\n", numberOfFBConfigs);

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

            printf("----------------------------------------\n");
            printf("Frame Buffer Config: %d\n", counter);
            printf("Visual ID: 0x%lu\n", pTempXVisualInfo->visualid);
            printf("Sample Buffers: %d\n", sampleBuffer);
            printf("Samples: %d\n", samples);
            printf("----------------------------------------\n");

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
        printf("Error: Unable to choose visual.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    printf("----------------------------------------\n");
    printf("Selected Visual ID: 0x%lu\n", visualInfo->visualid);
    printf("----------------------------------------\n");

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
        printf("Error: Failed to create main window.");
        cleanup();
        exit(EXIT_FAILURE);
    }

    XStoreName(gDisplay, window, "CG - Rotation by Matrix Multiplication");

    Atom windowManagerDelete = XInternAtom(gDisplay, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(gDisplay, window, &windowManagerDelete, 1);
    XMapWindow(gDisplay, window);
}

void initialize(void)
{
    // Create new GL context 4.5 for rendering.
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte *)"glXCreateContextAttribsARB");

    if(glXCreateContextAttribsARB == NULL)
    {
        printf("Error: Unable to get glXCreateContextAttribsARBProc address.");
        cleanup();
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
        printf("Failed to get GLX Context for OpenGL version 4.5, using old version.\n");

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
        printf("GLX Context for OpenGL version 4.5 is created.\n");
    }

    // Check do we have a direct context so that we get hardware rendering.
    if(!glXIsDirect(gDisplay, glxContext))
    {
        printf("Indirect GLX rendering context is used.\n");
    }
    else
    {
        printf("Direct GLX rendering context is used, hardware rendering is available.\n");
    }

    glXMakeCurrent(gDisplay, window, glxContext);
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // Always cull back face for performance gain.
    // glEnable(GL_CULL_FACE);

    initializeIdentityMatrix();
    initializeScaleMatrix();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    resize(windowWidth, windowHeight);
}

void handleEvent(XEvent *event)
{
    KeySym keySymbol;

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

                case XK_F:
                case XK_f:
                    isFullscreen = !isFullscreen;
                    toggleFullscreen(isFullscreen);
                break;

                case XK_9:
                    speed = 0.9f;
                break;

                default:
                break;
            }

            if(keySymbol > XK_0 && keySymbol < XK_9)
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

        case DestroyNotify: // Window destroyed, cleanup resources.
        break;

        case 33: // Windows close button event.
            isCloseButtonClicked = true;
        break;

        default:
        break;
    }
}

void initializeIdentityMatrix(void)
{
    // x, y, z, w
    // 1.0f, 0.0f, 0.0f, 0.0f,
    // 0.0f, 1.0f, 0.0f, 0.0f,
    // 0.0f, 0.0f, 1.0f, 0.0f,
    // 0.0f, 0.0f, 0.0f, 1.0f;
    identityMatrix[0] = 1.0f; identityMatrix[4] = 0.0f; identityMatrix[8] = 0.0f; identityMatrix[12] = 0.0f;
    identityMatrix[1] = 0.0f; identityMatrix[5] = 1.0f; identityMatrix[9] = 0.0f; identityMatrix[13] = 0.0f;
    identityMatrix[2] = 0.0f; identityMatrix[6] = 0.0f; identityMatrix[10] = 1.0f; identityMatrix[14] = 0.0f;
    identityMatrix[3] = 0.0f; identityMatrix[7] = 0.0f; identityMatrix[11] = 0.0f; identityMatrix[15] = 1.0f;
}

void initializeScaleMatrix(void)
{
    // x, y, z, w
    // sx, 0.0f, 0.0f, 0.0f,
    // 0.0f, sy, 0.0f, 0.0f,
    // 0.0f, 0.0f, sz, 0.0f,
    // 0.0f, 0.0f, 0.0f, 1.0f;
    scaleMatrix[0] = 0.75f; scaleMatrix[4] = 0.0f; scaleMatrix[8] = 0.0f; scaleMatrix[12] = 0.0f;
    scaleMatrix[1] = 0.0f; scaleMatrix[5] = 0.75f; scaleMatrix[9] = 0.0f; scaleMatrix[13] = 0.0f;
    scaleMatrix[2] = 0.0f; scaleMatrix[6] = 0.0f; scaleMatrix[10] = 0.75f; scaleMatrix[14] = 0.0f;
    scaleMatrix[3] = 0.0f; scaleMatrix[7] = 0.0f; scaleMatrix[11] = 0.0f; scaleMatrix[15] = 1.0f;
}

void update(void)
{
    angleCube -= speed;
    anglePyramid += speed;

    if(angleCube <= -360.0f)
    {
        angleCube = 0.0f;
    }

    if(anglePyramid >= 360.0f)
    {
        anglePyramid = 0.0f;
    }
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(identityMatrix);

    // x, y, z, w
    // 1.0f, 0.0f, 0.0f, tx,
    // 0.0f, 1.0f, 0.0f, ty,
    // 0.0f, 0.0f, 1.0f, tz,
    // 0.0f, 0.0f, 0.0f, 1.0f
    translationMatrix[0] = 1.0f; translationMatrix[4] = 0.0f; translationMatrix[8] = 0.0f; translationMatrix[12] = 1.5f;
    translationMatrix[1] = 0.0f; translationMatrix[5] = 1.0f; translationMatrix[9] = 0.0f; translationMatrix[13] = 0.0f;
    translationMatrix[2] = 0.0f; translationMatrix[6] = 0.0f; translationMatrix[10] = 1.0f; translationMatrix[14] = -6.0f;
    translationMatrix[3] = 0.0f; translationMatrix[7] = 0.0f; translationMatrix[11] = 0.0f; translationMatrix[15] = 1.0f;

    glMultMatrixf(translationMatrix);
    glMultMatrixf(scaleMatrix);

    GLfloat angleCubeRadians = angleCube * M_PI / 180.0f;

    // Rotation on x-axis
    // x, y, z, w
    // 1.0f, 0.0f, 0.0f, 0.0f
    // 0.0f, cosT, -sinT, 0.0f
    // 0.0f, sinT, cosT, 0.0f
    // 0.0f, 0.0f, 0.0f, 0.0f
    rotationXMatrix[0] = 1.0f; rotationXMatrix[4] = 0.0f; rotationXMatrix[8] = 0.0f; rotationXMatrix[12] = 0.0f;
    rotationXMatrix[1] = 0.0f; rotationXMatrix[5] = cosf(angleCubeRadians); rotationXMatrix[9] = -sinf(angleCubeRadians); rotationXMatrix[13] = 0.0f;
    rotationXMatrix[2] = 0.0f; rotationXMatrix[6] = sinf(angleCubeRadians); rotationXMatrix[10] = cosf(angleCubeRadians); rotationXMatrix[14] = 0.0f;
    rotationXMatrix[3] = 0.0f; rotationXMatrix[7] = 0.0f; rotationXMatrix[11] = 0.0f; rotationXMatrix[15] = 1.0f;

    // Rotation on y-axis
    // x, y, z, w
    // cosT, 0.0f, sinT, 0.0f
    // 0.0f, 1.0f, 0.0f, 0.0f
    // -sinT, 0.0f, cosT, 0.0f
    // 0.0f, 0.0f, 0.0f, 0.0f
    rotationYMatrix[0] = cosf(angleCubeRadians); rotationYMatrix[4] = 0.0f; rotationYMatrix[8] = sin(angleCubeRadians); rotationYMatrix[12] = 0.0f;
    rotationYMatrix[1] = 0.0f; rotationYMatrix[5] = 1.0f; rotationYMatrix[9] = 0.0f; rotationYMatrix[13] = 0.0f;
    rotationYMatrix[2] = -sinf(angleCubeRadians); rotationYMatrix[6] = 0.0f; rotationYMatrix[10] = cosf(angleCubeRadians); rotationYMatrix[14] = 0.0f;
    rotationYMatrix[3] = 0.0f; rotationYMatrix[7] = 0.0f; rotationYMatrix[11] = 0.0f; rotationYMatrix[15] = 1.0f;

    // Rotation on z-axis
    // x, y, z, w
    // cosT, -sinT, 0.0f, 0.0f
    // sinT, cosT, 0.0f, 0.0f
    // 0.0f, 0.0f, 1.0f, 0.0f
    // 0.0f, 0.0f, 0.0f, 0.0f
    rotationZMatrix[0] = cosf(angleCubeRadians); rotationZMatrix[4] = -sinf(angleCubeRadians); rotationZMatrix[8] = 0.0f; rotationZMatrix[12] = 0.0f;
    rotationZMatrix[1] = sinf(angleCubeRadians); rotationZMatrix[5] = cosf(angleCubeRadians); rotationZMatrix[9] = 0.0f; rotationZMatrix[13] = 0.0f;
    rotationZMatrix[2] = 0.0f; rotationZMatrix[6] = 0.0f; rotationZMatrix[10] = 1.0f; rotationZMatrix[14] = 0.0f;
    rotationZMatrix[3] = 0.0f; rotationZMatrix[7] = 0.0f; rotationZMatrix[11] = 0.0f; rotationZMatrix[15] = 1.0f;

    glMultMatrixf(rotationXMatrix);
    glMultMatrixf(rotationYMatrix);
    glMultMatrixf(rotationZMatrix);

    drawCube();

    glLoadMatrixf(identityMatrix);

    // x, y, z, w
    // 1.0f, 0.0f, 0.0f, tx,
    // 0.0f, 1.0f, 0.0f, ty,
    // 0.0f, 0.0f, 1.0f, tz,
    // 0.0f, 0.0f, 0.0f, 1.0f
    translationMatrix[0] = 1.0f; translationMatrix[4] = 0.0f; translationMatrix[8] = 0.0f; translationMatrix[12] = -1.5f;
    translationMatrix[1] = 0.0f; translationMatrix[5] = 1.0f; translationMatrix[9] = 0.0f; translationMatrix[13] = 0.0f;
    translationMatrix[2] = 0.0f; translationMatrix[6] = 0.0f; translationMatrix[10] = 1.0f; translationMatrix[14] = -6.0f;
    translationMatrix[3] = 0.0f; translationMatrix[7] = 0.0f; translationMatrix[11] = 0.0f; translationMatrix[15] = 1.0f;

    glMultMatrixf(scaleMatrix);
    glMultMatrixf(translationMatrix);

    GLfloat anglePyramidRadians = anglePyramid * M_PI / 180.0f;

    // Rotation on y-axis
    // x, y, z, w
    // cosT, 0.0f, sinT, 0.0f
    // 0.0f, 1.0f, 0.0f, 0.0f
    // -sinT, 0.0f, cosT, 0.0f
    // 0.0f, 0.0f, 0.0f, 0.0f
    rotationYMatrix[0] = cosf(anglePyramidRadians); rotationYMatrix[4] = 0.0f; rotationYMatrix[8] = sin(anglePyramidRadians); rotationYMatrix[12] = 0.0f;
    rotationYMatrix[1] = 0.0f; rotationYMatrix[5] = 1.0f; rotationYMatrix[9] = 0.0f; rotationYMatrix[13] = 0.0f;
    rotationYMatrix[2] = -sinf(anglePyramidRadians); rotationYMatrix[6] = 0.0f; rotationYMatrix[10] = cosf(anglePyramidRadians); rotationYMatrix[14] = 0.0f;
    rotationYMatrix[3] = 0.0f; rotationYMatrix[7] = 0.0f; rotationYMatrix[11] = 0.0f; rotationYMatrix[15] = 1.0f;

    glMultMatrixf(rotationYMatrix);
    drawPyramid();

    glXSwapBuffers(gDisplay, window);
}

void drawCube(void)
{
    glBegin(GL_QUADS);

	// Top face
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	// Bottom face
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	// Front face
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	// Back face
	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	// Right face
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	// Left face
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

    glEnd();
}

void drawPyramid(void)
{
    glBegin(GL_TRIANGLES);

    // Front face
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);

    // Right face
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);

    // Back face
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);

    // Left face
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);

    glEnd();
}

void resize(int width, int height)
{
    if(height == 0)
    {
        height = 1;
    }

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 1.0f, 100.0f);
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

void cleanup(void)
{
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
