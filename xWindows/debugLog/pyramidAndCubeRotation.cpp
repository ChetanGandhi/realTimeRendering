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

bool isFullscreen = false;

int windowWidth = 800;
int windowHeight = 600;

FILE *logFile = NULL;

GLfloat angleCube = 0.0f;
GLfloat anglePyramid = 0.0f;
GLfloat speed = 0.1f;

Display *gDisplay = NULL;
XVisualInfo *visualInfo = NULL;
Colormap colormap;
Window window;
GLXContext glxContext;

void createWindow(void);
void initialize(void);
void cleanup(void);
void handleEvent(XEvent *event);
void update(void);
void display(void);
void drawPyramid(void);
void drawCube(void);
void resize(int width, int height);
void toggleFullscreen(bool isFullscreen);

int main(void)
{
    logFile = fopen("debug.log", "w");
    if(logFile == NULL)
    {
        printf("Error: Unable to create log file.");
        exit(EXIT_FAILURE);
    }

    fprintf(logFile, "---------- CG: OpenGL Debug Logs Start ----------\n");
    fprintf(logFile, "LOG: Entering main\n");
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

        update();
        display();
    }

    cleanup();

    fprintf(logFile, "LOG: Exiting main\n");
    fprintf(logFile, "---------- CG: OpenGL Debug Logs End ----------\n");
    fflush(logFile);
    fclose(logFile);

    return EXIT_SUCCESS;
}

void createWindow(void)
{
    fprintf(logFile, "LOG: Entering createWindow\n");
    fflush(logFile);

    XSetWindowAttributes windowAttributes;
    int defaultScreen;
    int styleMask;

    static int frameBufferAttributes[] = {
        GLX_RGBA,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DOUBLEBUFFER, True,
        GLX_DEPTH_SIZE, 24,
        // GLX_X_RENDERABLE, true, // did not work.
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        // GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT, // did not work.
        // GLX_RENDER_TYPE, GLX_RGBA_BIT, // did not work.
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
    visualInfo = glXChooseVisual(gDisplay, defaultScreen, frameBufferAttributes);

    if(visualInfo == NULL)
    {
        printf("Error: Unable to choose visual.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

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

    XStoreName(gDisplay, window, "CG - Pyramid And Cube Rotation");

    Atom windowManagerDelete = XInternAtom(gDisplay, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(gDisplay, window, &windowManagerDelete, 1);
    XMapWindow(gDisplay, window);

    fprintf(logFile, "LOG: Exiting createWindow\n");
    fflush(logFile);
}

void initialize(void)
{
    fprintf(logFile, "LOG: Entering initialize\n");
    fflush(logFile);

    glxContext = glXCreateContext(gDisplay, visualInfo, NULL, GL_TRUE);
    glXMakeCurrent(gDisplay, window, glxContext);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    resize(windowWidth, windowHeight);

    fprintf(logFile, "LOG: Exiting initialize\n");
    fflush(logFile);
}

void handleEvent(XEvent *event)
{
    fprintf(logFile, "LOG: Entering handleEvent\n");
    fflush(logFile);

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
            cleanup();
            exit(EXIT_SUCCESS);
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
        cleanup();
        exit(EXIT_SUCCESS);
        break;

        default:
        break;
    }

    fprintf(logFile, "LOG: Exiting handleEvent\n");
    fflush(logFile);
}

void update(void)
{
    fprintf(logFile, "LOG: Entering update\n");
    fflush(logFile);

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

    fprintf(logFile, "LOG: Exiting update\n");
    fflush(logFile);
}

void display(void)
{
    fprintf(logFile, "LOG: Entering display\n");
    fflush(logFile);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    glTranslatef(-1.5f, 0.0f, -7.0f);
    glRotatef(anglePyramid, 0.0f, 1.0f, 0.0f);
    drawPyramid();

    glLoadIdentity();
    glTranslatef(1.5f, 0.0f, -7.0f);
    glScalef(0.75f, 0.75f, 0.75f);
    glRotatef(angleCube, 1.0f, 1.0f, 1.0f);
    drawCube();

    glXSwapBuffers(gDisplay, window);

    fprintf(logFile, "LOG: Exiting display\n");
    fflush(logFile);
}

void drawPyramid(void)
{
    fprintf(logFile, "LOG: Entering drawPyramid\n");
    fflush(logFile);

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

    fprintf(logFile, "LOG: Exiting drawPyramid\n");
    fflush(logFile);
}

void drawCube(void)
{
    fprintf(logFile, "LOG: Entering drawCube\n");
    fflush(logFile);

    glBegin(GL_QUADS);

	//Top face
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	//Bottom face
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	//Front face
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	//Back face
	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	//right face
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	//left face
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

    glEnd();

    fprintf(logFile, "LOG: Exiting drawCube\n");
    fflush(logFile);
}

void resize(int width, int height)
{
    fprintf(logFile, "LOG: Entering resize\n");
    fflush(logFile);

    if(height == 0)
    {
        height = 1;
    }

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 1.0f, 100.0f);

    fprintf(logFile, "LOG: Exiting resize\n");
    fflush(logFile);
}

void toggleFullscreen(bool isFullscreen)
{
    fprintf(logFile, "LOG: Entering toggleFullscreen\n");
    fflush(logFile);

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

    fprintf(logFile, "LOG: Exiting toggleFullscreen\n");
    fflush(logFile);
}

void cleanup(void)
{
    fprintf(logFile, "LOG: Entering cleanup\n");
    fflush(logFile);

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

    fprintf(logFile, "LOG: Exiting cleanup\n");
    fflush(logFile);
}
