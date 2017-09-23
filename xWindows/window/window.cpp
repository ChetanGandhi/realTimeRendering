#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

bool isFullscreen = false;

int windowWidth = 800;
int windowHeight = 600;

Display *display = NULL;
XVisualInfo *visualInfo = NULL;
Colormap colormap;
Window window;

void createWindow(void);
void cleanup();
void handleEvent(XEvent *event);
void toggleFullscreen(bool isFullscreen);

int main(void)
{
    XEvent event;

    createWindow();

    while(1)
    {
        XNextEvent(display, &event);
        handleEvent(&event);
    }

    cleanup();
    return EXIT_SUCCESS;
}

void createWindow(void)
{
    XSetWindowAttributes windowAttributes;
    int defaultScreen;
    int defaultDepth;
    int styleMask;

    display = XOpenDisplay(NULL);

    if(display == NULL)
    {
        printf("Error: Unable to open XDisplay.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    defaultScreen = XDefaultScreen(display);
    defaultDepth = XDefaultDepth(display, defaultScreen);
    visualInfo = (XVisualInfo *)malloc(sizeof(XVisualInfo));

    if(visualInfo == NULL)
    {
        printf("Error: Unable to allocate memory for visual info.");
        cleanup();
        exit(EXIT_FAILURE);
    }

    int result = XMatchVisualInfo(display, defaultScreen, defaultDepth, TrueColor, visualInfo);

    if(result == 0)
    {
        printf("Error: Unable to get visual.");
        cleanup();
        exit(EXIT_FAILURE);
    }

    windowAttributes.border_pixel = 0;
    windowAttributes.border_pixmap = 0;
    windowAttributes.background_pixel = BlackPixel(display, defaultScreen);
    windowAttributes.background_pixmap = 0;
    windowAttributes.colormap = XCreateColormap(display, XRootWindow(display, visualInfo->screen), visualInfo->visual, AllocNone);
    windowAttributes.event_mask = ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;

    styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
    colormap = windowAttributes.colormap;

    window = XCreateWindow(display, XRootWindow(display, visualInfo->screen), 0, 0, windowWidth, windowHeight, 0, visualInfo->depth, InputOutput, visualInfo->visual, styleMask, &windowAttributes);

    if(!window)
    {
        printf("Error: Failed to create main window.");
        cleanup();
        exit(EXIT_FAILURE);
    }

    XStoreName(display, window, "CG - Empty Window");

    Atom windowManagerDelete = XInternAtom(display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(display, window, &windowManagerDelete, 1);
    XMapWindow(display, window);
}

void handleEvent(XEvent *event)
{
    KeySym keySymbol;

    switch(event->type)
    {
        case MapNotify: // Window created.
        break;

        case KeyPress: // key events.
            keySymbol = XkbKeycodeToKeysym(display, event->xkey.keycode, 0, 0);

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
        break;

        case Expose: // Pant window.
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
}

void toggleFullscreen(bool isFullscreen)
{
    Atom wmStateNormal;
    Atom wmStateFullscreen;
    XEvent event = {0};

    wmStateNormal = XInternAtom(display, "_NET_WM_STATE", False);
    wmStateFullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

    memset(&event, 0, sizeof(event));
    event.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = wmStateNormal;
    event.xclient.format = 32;
    event.xclient.data.l[0] = isFullscreen ? 1 : 0;
    event.xclient.data.l[1] = wmStateFullscreen;

    XSendEvent(display, XRootWindow(display, visualInfo->screen), False, StructureNotifyMask, &event);
}

void cleanup(void)
{
    if(window)
    {
        XDestroyWindow(display, window);
    }

    if(colormap)
    {
        XFreeColormap(display, colormap);
    }

    if(visualInfo)
    {
        free(visualInfo);
        visualInfo = NULL;
    }

    if(display)
    {
        XCloseDisplay(display);
        display = NULL;
    }
}
