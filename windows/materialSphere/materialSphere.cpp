#define _USE_MATH_DEFINES

#include <windows.h>
#include <cstdlib>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "resources/resource.h"

HWND hWnd = NULL;
HDC hdc = NULL;
HGLRC hrc = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
RECT windowRect = {0, 0, 800, 600};

bool isFullscreen = false;
bool isActive = false;
bool isEscapeKeyPressed = false;
bool isLightingEnabled = false;

GLboolean rotateLightOnXAxis = GL_TRUE;
GLboolean rotateLightOnYAxis = GL_FALSE;
GLboolean rotateLightOnZAxis = GL_FALSE;

GLfloat angleLight = 0.0f;
GLfloat speed = 0.1f;

GLfloat lightZeroAmbient[] = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat lightZeroDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightZeroSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightZeroPosition[] = {0.0f, 0.0f, 1.0f, 0.0f};

GLfloat lightModalAmbient[] = {0.2f, 0.2f, 0.2f, 0.0f};
GLfloat lightModalLocalViewer[] = {0.0f, 0.0f, 0.0f, 0.0f};

GLfloat materialAmbient[4][6][4] = {
    {   // Column 1
        {0.0215f, 0.1745f, 0.0215f, 1.0f },
        {0.135f, 0.2225f, 0.1575f, 1.0f },
        {0.05375f, 0.05f, 0.06625f, 1.0f },
        {0.25f, 0.20725f, 0.20725f, 1.0f },
        {0.1745f, 0.01175f, 0.01175f, 1.0f },
        {0.1f, 0.18725f, 0.1745f, 1.0f }
    },
    {   // Column 2
        {0.329412f, 0.223529f, 0.027451f, 1.0f },
        {0.2125f, 0.1275f, 0.054f, 1.0f },
        {0.25f, 0.25f, 0.25f, 1.0f },
        {0.19125f, 0.0735f, 0.0225f, 1.0f },
        {0.24725f, 0.1995f, 0.0745f, 1.0f },
        {0.19225f, 0.19225f, 0.19225f, 1.0f }
    },
    {   // Column 3
        {0.0f, 0.0f, 0.0f, 1.0f },
        {0.0f, 0.1f, 0.06f, 1.0f },
        {0.0f, 0.0f, 0.0f, 1.0f },
        {0.0f, 0.0f, 0.0f, 1.0f },
        {0.0f, 0.0f, 0.0f, 1.0f },
        {0.0f, 0.0f, 0.0f, 1.0f }
    },
    {   // Column 4
        {0.02f, 0.02f, 0.02f, 1.0f },
        {0.0f, 0.05f, 0.05f, 1.0f },
        {0.0f, 0.05f, 0.0f, 1.0f },
        {0.05f, 0.0f, 0.0f, 1.0f },
        {0.05f, 0.05f, 0.05f, 1.0f },
        {0.05f, 0.05f, 0.0f, 1.0f }
    }
};

GLfloat materialDiffuse[4][6][4] = {
    {   // Column 1
        {0.07568f, 0.61424f, 0.07568f, 1.0f},
        {0.54f, 0.89f, 0.63f, 1.0f},
        {0.18275f, 0.17f, 0.22525f, 1.0f},
        {1.0f, 0.829f, 0.829f, 1.0f},
        {0.61424f, 0.04136f, 0.04136f, 1.0f},
        {0.396f, 0.74151f, 0.69102f, 1.0f},
    },
    {   // Column 2
        {0.780392f, 0.568627f, 0.113725f, 1.0f},
        {0.714f, 0.4284f, 0.18144f, 1.0f},
        {0.4f, 0.4f, 0.4f, 1.0f},
        {0.7038f, 0.27048f, 0.0828f, 1.0f},
        {0.75164f, 0.60648f, 0.22648f, 1.0f},
        {0.50754f, 0.50754f, 0.50754f, 1.0f},
    },
    {   // Column 3
        {0.01f, 0.01f, 0.01f, 1.0f},
        {0.0f, 0.50980392f, 0.50980392f, 1.0f},
        {0.1f, 0.35f, 0.1f, 1.0f},
        {0.5f, 0.0f, 0.0f, 1.0f},
        {0.55f, 0.55f, 0.55f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f},
    },
    {   // Column 4
        {0.01f, 0.01f, 0.01f, 1.0f},
        {0.4f, 0.5f, 0.5f, 1.0f},
        {0.4f, 0.5f, 0.4f, 1.0f},
        {0.5f, 0.4f, 0.4f, 1.0f},
        {0.5f, 0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, 0.4f, 1.0f},
    },
};

GLfloat materialSpecular[4][6][4] = {
    {   // Column 1
        {0.633f, 0.727811f, 0.633f, 1.0f},
        {0.316228f, 0.316228f, 0.316228f, 1.0f},
        {0.332741f, 0.328634f, 0.346435f, 1.0f},
        {0.296648f, 0.296648f, 0.296648f, 1.0f},
        {0.727811f, 0.626959f, 0.626959f, 1.0f},
        {0.297254f, 0.30829f, 0.306678f, 1.0f},
    },
    {   // Column 2
        {0.992157f, 0.941176f, 0.807843f, 1.0f},
        {0.393548f, 0.271906f, 0.166721f, 1.0f},
        {0.774597f, 0.774597f, 0.774597f, 1.0f},
        {0.256777f, 0.137622f, 0.086014f, 1.0f},
        {0.628281f, 0.555802f, 0.366065f, 1.0f},
        {0.508273f, 0.508273f, 0.508273f, 1.0f},
    },
    {   // Column 3
        {0.50f, 0.50f, 0.50f, 1.0f},
        {0.50196078f, 0.50196078f, 0.50196078f, 1.0f},
        {0.45f, 0.55f, 0.45f, 1.0f},
        {0.7f, 0.6f, 0.6f, 1.0f},
        {0.70f, 0.70f, 0.70f, 1.0f},
        {0.60f, 0.60f, 0.50f, 1.0f},
    },
    {   // Column 4
        {0.4f, 0.4f, 0.4f, 1.0f},
        {0.04f, 0.7f, 0.7f, 1.0f},
        {0.04f, 0.7f, 0.04f, 1.0f},
        {0.7f, 0.04f, 0.04f, 1.0f},
        {0.7f, 0.7f, 0.7f, 1.0f},
        {0.7f, 0.7f, 0.04f, 1.0f},
    }
};

GLfloat matrialShininess[4][6][4] = {
    {   // Column 1
        {0.6f * 128.0f, 0.6f * 128.0f, 0.6f * 128.0f, 0.6f * 128.0f},
        {0.1f * 128.0f, 0.1f * 128.0f, 0.1f * 128.0f, 0.1f * 128.0f},
        {0.3f * 128.0f, 0.3f * 128.0f, 0.3f * 128.0f, 0.3f * 128.0f},
        {0.088f * 128.0f, 0.088f * 128.0f, 0.088f * 128.0f, 0.088f * 128.0f},
        {0.6f * 128.0f, 0.6f * 128.0f, 0.6f * 128.0f, 0.6f * 128.0f},
        {0.1f * 128.0f, 0.1f * 128.0f, 0.1f * 128.0f, 0.1f * 128.0f},
    },
    {   // Column 2
        {0.21794872f * 128.0f, 0.21794872f * 128.0f, 0.21794872f * 128.0f, 0.21794872f * 128.0f},
        {0.2f * 128.0f, 0.2f * 128.0f, 0.2f * 128.0f, 0.2f * 128.0f},
        {0.6f * 128.0f, 0.6f * 128.0f, 0.6f * 128.0f, 0.6f * 128.0f},
        {0.1f * 128.0f, 0.1f * 128.0f, 0.1f * 128.0f, 0.1f * 128.0f},
        {0.4f * 128.0f, 0.4f * 128.0f, 0.4f * 128.0f, 0.4f * 128.0f},
        {0.4f * 128.0f, 0.4f * 128.0f, 0.4f * 128.0f, 0.4f * 128.0f},
    },
    {   // Column 3
        {0.25f * 128.0f, 0.25f * 128.0f, 0.25f * 128.0f, 0.25f * 128.0f},
        {0.25f * 128.0f, 0.25f * 128.0f, 0.25f * 128.0f, 0.25f * 128.0f},
        {0.25f * 128.0f, 0.25f * 128.0f, 0.25f * 128.0f, 0.25f * 128.0f},
        {0.25f * 128.0f, 0.25f * 128.0f, 0.25f * 128.0f, 0.25f * 128.0f},
        {0.25f * 128.0f, 0.25f * 128.0f, 0.25f * 128.0f, 0.25f * 128.0f},
        {0.25f * 128.0f, 0.25f * 128.0f, 0.25f * 128.0f, 0.25f * 128.0f},
    },
    {   // Column 4
        {0.078125f * 128.0f, 0.078125f * 128.0f, 0.078125f * 128.0f, 0.078125f * 128.0f},
        {0.078125f * 128.0f, 0.078125f * 128.0f, 0.078125f * 128.0f, 0.078125f * 128.0f},
        {0.078125f * 128.0f, 0.078125f * 128.0f, 0.078125f * 128.0f, 0.078125f * 128.0f},
        {0.078125f * 128.0f, 0.078125f * 128.0f, 0.078125f * 128.0f, 0.078125f * 128.0f},
        {0.078125f * 128.0f, 0.078125f * 128.0f, 0.078125f * 128.0f, 0.078125f * 128.0f},
        {0.078125f * 128.0f, 0.078125f * 128.0f, 0.078125f * 128.0f, 0.078125f * 128.0f},
    }
};

GLUquadric *guadric = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

void initialize(void);
void cleanUp(void);
void update(void);
void display(void);
void rotateLight(void);
void loadMaterial(int row, int column);
void drawSphere(void);
void resize(int width, int height);
void toggleFullscreen(HWND hWnd, bool isFullscreen);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsatnce, LPSTR lpszCmdLine, int nCmdShow)
{
    WNDCLASSEX wndClassEx;
    MSG message;
    TCHAR szApplicationTitle[] = TEXT("CG - Material on Sphere");
    TCHAR szApplicationClassName[] = TEXT("RTR_OPENGL_MATERIAL_SPHERE");
    bool done = false;

    wndClassEx.cbSize = sizeof(WNDCLASSEX);
    wndClassEx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndClassEx.cbClsExtra = 0;
    wndClassEx.cbWndExtra = 0;
    wndClassEx.lpfnWndProc = WndProc;
    wndClassEx.hInstance = hInstance;
    wndClassEx.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(CP_ICON));
    wndClassEx.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(CP_ICON_SMALL));
    wndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClassEx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClassEx.lpszClassName = szApplicationClassName;
    wndClassEx.lpszMenuName = NULL;

    if(!RegisterClassEx(&wndClassEx))
    {
        MessageBox(NULL, TEXT("Cannot register class."), TEXT("Error"), MB_OK | MB_ICONERROR);
        exit(EXIT_FAILURE);
    }

    DWORD styleExtra = WS_EX_APPWINDOW;
    dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE;

    hWnd = CreateWindowEx(styleExtra,
        szApplicationClassName,
        szApplicationTitle,
        dwStyle,
        windowRect.left,
        windowRect.top,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        NULL,
        NULL,
        hInstance,
        NULL);

    if(!hWnd)
    {
        MessageBox(NULL, TEXT("Cannot create windows."), TEXT("Error"), MB_OK | MB_ICONERROR);
        exit(EXIT_FAILURE);
    }

    initialize();

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);

    while(!done)
    {
        if(PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            if(message.message == WM_QUIT)
            {
                done = true;
            }
            else
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }
        else
        {
            if(isActive)
            {
                if(isEscapeKeyPressed)
                {
                    done = true;
                }
                else
                {
                    update();
                    display();
                }
            }
        }
    }

    cleanUp();

    return (int)message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    RECT rect;

    switch(iMessage)
    {
        case WM_ACTIVATE:
            isActive = (HIWORD(wParam) == 0);
        break;

        case WM_SIZE:
            resize(LOWORD(lParam), HIWORD(lParam));
        break;

        case WM_KEYDOWN:
            switch(wParam)
            {
                case VK_ESCAPE:
                    isEscapeKeyPressed = true;
                break;

                // 0x46 is hex value for key 'F' or 'f'
                case 0x46:
                    isFullscreen = !isFullscreen;
                    toggleFullscreen(hWnd, isFullscreen);
                break;

                default:
                break;
            }

            if(wParam > 0x30 && wParam < 0x40)
            {
                speed = 0.1f * (wParam - 0x30);
            }
            else if(wParam > 0x60 && wParam < 0x70)
            {
                speed = 0.1f * (wParam - 0x60);
            }

        break;

        case WM_CHAR:
            switch(wParam)
            {
                case 'L':
                case 'l':
                    if(!isLightingEnabled) {
                        isLightingEnabled = true;
                        glEnable(GL_LIGHTING);
                        glEnable(GL_LIGHT0);
                    }
                    else {
                        isLightingEnabled = false;
                        glDisable(GL_LIGHTING);
                        glDisable(GL_LIGHT0);
                    }
                break;

                case 'X':
                case 'x':
                    rotateLightOnXAxis = GL_TRUE;
                    rotateLightOnYAxis = GL_FALSE;
                    rotateLightOnZAxis = GL_FALSE;
                break;

                case 'Y':
                case 'y':
                    rotateLightOnXAxis = GL_FALSE;
                    rotateLightOnYAxis = GL_TRUE;
                    rotateLightOnZAxis = GL_FALSE;
                break;

                case 'Z':
                case 'z':
                    rotateLightOnXAxis = GL_FALSE;
                    rotateLightOnYAxis = GL_FALSE;
                    rotateLightOnZAxis = GL_TRUE;
                break;

            }
        break;

        case WM_LBUTTONDOWN:
        break;

        case WM_DESTROY:
            PostQuitMessage(0);
        break;

        default:
        break;
    }

    return DefWindowProc(hWnd, iMessage, wParam, lParam);
}

void initialize(void)
{
    PIXELFORMATDESCRIPTOR pfd;
    int pixelFormatIndex = 0;

    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cRedBits = 8;
    pfd.cGreenBits = 8;
    pfd.cBlueBits = 8;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 32;

    hdc = GetDC(hWnd);

    pixelFormatIndex = ChoosePixelFormat(hdc, &pfd);

    if(pixelFormatIndex == 0)
    {
        ReleaseDC(hWnd, hdc);
        hdc = NULL;
    }

    if(!SetPixelFormat(hdc, pixelFormatIndex, &pfd))
    {
        ReleaseDC(hWnd, hdc);
        hdc = NULL;
    }

    hrc = wglCreateContext(hdc);
    if(hrc == NULL)
    {
        ReleaseDC(hWnd, hdc);
        hdc = NULL;
    }

    if(!wglMakeCurrent(hdc, hrc))
    {
        wglDeleteContext(hrc);
        hrc = NULL;

        ReleaseDC(hWnd, hdc);
        hdc = NULL;
    }

    glClearColor(0.25f, 0.25f, 0.25f, 0.25f);
    glClearDepth(1.0f);

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightZeroAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightZeroSpecular);

    glEnable(GL_DEPTH_TEST);
    // Lighting and light 0 is enabled in WM_KEYDOWN when 'L' or 'l' key is pressed.
    // glEnable(GL_LIGHT0);

    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightModalAmbient);
    glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, lightModalLocalViewer);

    // This is required for DirectX
    resize(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
}

void update(void)
{
    angleLight += speed;

    if(angleLight >= 360.0f)
    {
        angleLight = 0.0f;
    }
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    // Column 1
    glLoadIdentity();
    glTranslatef(-6.0f, -5.0f, -20.0f);
    rotateLight();
    loadMaterial(0, 0);
    drawSphere();

    glLoadIdentity();
    glTranslatef(-6.0f, -3.0f, -20.0f);
    rotateLight();
    loadMaterial(0, 1);
    drawSphere();

    glLoadIdentity();
    glTranslatef(-6.0f, -1.0f, -20.0f);
    rotateLight();
    loadMaterial(0, 2);
    drawSphere();

    glLoadIdentity();
    glTranslatef(-6.0f, 1.0f, -20.0f);
    rotateLight();
    loadMaterial(0, 3);
    drawSphere();

    glLoadIdentity();
    glTranslatef(-6.0f, 3.0f, -20.0f);
    rotateLight();
    loadMaterial(0, 4);
    drawSphere();

    glLoadIdentity();
    glTranslatef(-6.0f, 5.0f, -20.0f);
    rotateLight();
    loadMaterial(0, 5);
    drawSphere();

    // Column 2
    glLoadIdentity();
    glTranslatef(-2.0f, -5.0f, -20.0f);
    rotateLight();
    loadMaterial(1, 0);
    drawSphere();

    glLoadIdentity();
    glTranslatef(-2.0f, -3.0f, -20.0f);
    rotateLight();
    loadMaterial(1, 1);
    drawSphere();

    glLoadIdentity();
    glTranslatef(-2.0f, -1.0f, -20.0f);
    rotateLight();
    loadMaterial(1, 2);
    drawSphere();

    glLoadIdentity();
    glTranslatef(-2.0f, 1.0f, -20.0f);
    rotateLight();
    loadMaterial(1, 3);
    drawSphere();

    glLoadIdentity();
    glTranslatef(-2.0f, 3.0f, -20.0f);
    rotateLight();
    loadMaterial(1, 4);
    drawSphere();

    glLoadIdentity();
    glTranslatef(-2.0f, 5.0f, -20.0f);
    rotateLight();
    loadMaterial(1, 5);
    drawSphere();

    // Column 3
    glLoadIdentity();
    glTranslatef(2.0f, -5.0f, -20.0f);
    rotateLight();
    loadMaterial(2, 0);
    drawSphere();

    glLoadIdentity();
    glTranslatef(2.0f, -3.0f, -20.0f);
    rotateLight();
    loadMaterial(2, 1);
    drawSphere();

    glLoadIdentity();
    glTranslatef(2.0f, -1.0f, -20.0f);
    rotateLight();
    loadMaterial(2, 2);
    drawSphere();

    glLoadIdentity();
    glTranslatef(2.0f, 1.0f, -20.0f);
    rotateLight();
    loadMaterial(2, 3);
    drawSphere();

    glLoadIdentity();
    glTranslatef(2.0f, 3.0f, -20.0f);
    rotateLight();
    loadMaterial(2, 4);
    drawSphere();

    glLoadIdentity();
    glTranslatef(2.0f, 5.0f, -20.0f);
    rotateLight();
    loadMaterial(2, 5);
    drawSphere();

    // Column 4
    glLoadIdentity();
    glTranslatef(6.0f, -5.0f, -20.0f);
    rotateLight();
    loadMaterial(3, 0);
    drawSphere();

    glLoadIdentity();
    glTranslatef(6.0f, -3.0f, -20.0f);
    rotateLight();
    loadMaterial(3, 1);
    drawSphere();

    glLoadIdentity();
    glTranslatef(6.0f, -1.0f, -20.0f);
    rotateLight();
    loadMaterial(3, 2);
    drawSphere();

    glLoadIdentity();
    glTranslatef(6.0f, 1.0f, -20.0f);
    rotateLight();
    loadMaterial(3, 3);
    drawSphere();

    glLoadIdentity();
    glTranslatef(6.0f, 3.0f, -20.0f);
    rotateLight();
    loadMaterial(3, 4);
    drawSphere();

    glLoadIdentity();
    glTranslatef(6.0f, 5.0f, -20.0f);
    rotateLight();
    loadMaterial(3, 5);
    drawSphere();

    SwapBuffers(hdc);
}

void rotateLight(void)
{
    if(rotateLightOnXAxis == GL_TRUE)
    {
        glRotatef(angleLight, 1.0f, 0.0f, 0.0f);
        lightZeroPosition[0] = 0;
        lightZeroPosition[1] = angleLight;
        lightZeroPosition[2] = 0;
        glLightfv(GL_LIGHT0, GL_POSITION, lightZeroPosition);
    }
    else if(rotateLightOnYAxis == GL_TRUE)
    {
        glRotatef(angleLight, 0.0f, 1.0f, 0.0f);
        lightZeroPosition[0] = angleLight;
        lightZeroPosition[1] = 0;
        lightZeroPosition[2] = 0;
        glLightfv(GL_LIGHT0, GL_POSITION, lightZeroPosition);
    }
    else if(rotateLightOnZAxis == GL_TRUE)
    {
        glRotatef(angleLight, 0.0f, 0.0f, 1.0f);
        lightZeroPosition[0] = 0;
        lightZeroPosition[1] = angleLight;
        lightZeroPosition[2] = 0;
        glLightfv(GL_LIGHT0, GL_POSITION, lightZeroPosition);
    }
}

void loadMaterial(int row, int column)
{
    glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient[row][column]);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse[row][column]);
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular[row][column]);
    glMaterialfv(GL_FRONT, GL_SHININESS, matrialShininess[row][column]);
}

void drawSphere(void)
{
    // Push initial matrix.
    glPushMatrix();

    // Rotate the sphere by 90 degree on x-axis
    // because sphere created by GLUT has it rotated in x-axis
    // The north-south poles are on x-axis instead of y-axis.
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Draw the sun
    guadric = gluNewQuadric();

    // 3rd parameter is for slices (like longitudes)
    // 4th parameter is for stacks (like latitudes)
    // Higher the value of 3rd and 4th parameters, i.e. more the sub-divisions,
    // more circular the sphere will look.
    gluSphere(guadric, 0.75, 30, 30);

    // Pop back to initial state.
    glPopMatrix();
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
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);
}

void toggleFullscreen(HWND hWnd, bool isFullscreen)
{
    MONITORINFO monitorInfo;
    dwStyle = GetWindowLong(hWnd, GWL_STYLE);

    if(isFullscreen)
    {
        if(dwStyle & WS_OVERLAPPEDWINDOW)
        {
            monitorInfo = { sizeof(MONITORINFO) };

            if(GetWindowPlacement(hWnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(hWnd, MONITORINFOF_PRIMARY), &monitorInfo))
            {
                SetWindowLong(hWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
                SetWindowPos(hWnd, HWND_TOP, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top, monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }

        ShowCursor(FALSE);
    }
    else
    {
        SetWindowLong(hWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(hWnd, &wpPrev);
        SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
        ShowCursor(TRUE);
    }
}

void cleanUp(void)
{
    if(isFullscreen)
    {
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        SetWindowLong(hWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(hWnd, &wpPrev);
        SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
        ShowCursor(TRUE);
    }

    if(guadric)
    {
        gluDeleteQuadric(guadric);
        guadric = nullptr;
    }

    wglMakeCurrent(NULL, NULL);

    wglDeleteContext(hrc);
    hrc = NULL;

    ReleaseDC(hWnd, hdc);
    hdc = NULL;

    DestroyWindow(hWnd);
    hWnd = NULL;
}
