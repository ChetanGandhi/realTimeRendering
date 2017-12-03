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

GLfloat angleRedLight = 0.0f;
GLfloat angleGreenLight = 0.0f;
GLfloat angleBlueLight = 0.0f;
GLfloat speed = 0.1f;

GLfloat lightZeroAmbient[] = {0.0f, 0.0f, 0.0f, 0.0f};
GLfloat lightZeroDiffuse[] = {1.0f, 0.0f, 0.0f, 0.0f};
GLfloat lightZeroSpecular[] = {1.0f, 0.0f, 0.0f, 0.0f};
GLfloat lightZeroPosition[] = {0.0f, 0.0f, 0.0f, 0.0f};

GLfloat lightOneAmbient[] = {0.0f, 0.0f, 0.0f, 0.0f};
GLfloat lightOneDiffuse[] = {0.0f, 1.0f, 0.0f, 0.0f};
GLfloat lightOneSpecular[] = {0.0f, 0.0f, 1.0f, 0.0f};
GLfloat lightOnePosition[] = {0.0f, 0.0f, 0.0f, 0.0f};

GLfloat lightTwoAmbient[] = {0.0f, 0.0f, 0.0f, 0.0f};
GLfloat lightTwoDiffuse[] = {0.0f, 0.0f, 1.0f, 0.0f};
GLfloat lightTwoSpecular[] = {0.0f, 0.0f, 1.0f, 0.0f};
GLfloat lightTwoPosition[] = {0.0f, 0.0f, 0.0f, 0.0f};

GLfloat materialAmbient[] = {0.0f, 0.0f, 0.0f, 0.0f};
GLfloat materialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialSpecular[] = {1.0f, 1.0f, 1.0f, 0.0f};
GLfloat matrialShininess[] = {50.0f, 50.0f, 50.0f, 50.0f};

GLUquadric *guadric = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

void initialize(void);
void cleanUp(void);
void update(void);
void display(void);
void drawSphere(void);
void resize(int width, int height);
void toggleFullscreen(HWND hWnd, bool isFullscreen);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsatnce, LPSTR lpszCmdLine, int nCmdShow)
{
    WNDCLASSEX wndClassEx;
    MSG message;
    TCHAR szApplicationTitle[] = TEXT("CG - Rotating Multiple Light on Sphere");
    TCHAR szApplicationClassName[] = TEXT("RTR_OPENGL_MULTIPLE_LIGHT_SPHERE");
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
                        glEnable(GL_LIGHT1);
                        glEnable(GL_LIGHT2);
                    }
                    else {
                        isLightingEnabled = false;
                        glDisable(GL_LIGHTING);
                        glDisable(GL_LIGHT0);
                        glDisable(GL_LIGHT1);
                        glDisable(GL_LIGHT2);
                    }
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

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightZeroAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightZeroSpecular);

    glLightfv(GL_LIGHT1, GL_AMBIENT, lightOneAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightOneDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightOneSpecular);

    glLightfv(GL_LIGHT2, GL_AMBIENT, lightTwoAmbient);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lightTwoDiffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, lightTwoSpecular);

    glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matrialShininess);

    glEnable(GL_DEPTH_TEST);
    // Lighting and light 0, 1 and 2 is enabled in WM_KEYDOWN when 'L' or 'l' key is pressed.
    // glEnable(GL_LIGHT0);
    // glEnable(GL_LIGHT1);
    // glEnable(GL_LIGHT2);

    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // This is required for DirectX
    resize(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
}

void update(void)
{
    angleRedLight += speed;
    angleGreenLight += speed;
    angleBlueLight += speed;

    if(angleRedLight >= 360.0f)
    {
        angleRedLight = 0.0f;
    }

    if(angleGreenLight >= 360.0f)
    {
        angleGreenLight = 0.0f;
    }

    if(angleBlueLight >= 360.0f)
    {
        angleBlueLight = 0.0f;
    }
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawSphere();

    SwapBuffers(hdc);
}

void drawSphere(void)
{
    // Push initial matrix.
    glPushMatrix();
    gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    // Light 0 push
    glPushMatrix();

    glRotatef(angleRedLight, 1.0f, 0.0f, 0.0f);
    lightZeroPosition[0] = 0;
    lightZeroPosition[1] = angleRedLight;
    lightZeroPosition[2] = 0;
    glLightfv(GL_LIGHT0, GL_POSITION, lightZeroPosition);

    // Light 0 pop
    glPopMatrix();

    // Light 1 push
    glPushMatrix();

    glRotatef(angleGreenLight, 0.0f, 1.0f, 0.0f);
    lightOnePosition[0] = angleRedLight;
    lightOnePosition[1] = 0;
    lightOnePosition[2] = 0;
    glLightfv(GL_LIGHT1, GL_POSITION, lightOnePosition);

    // Light 1 pop
    glPopMatrix();

    // Light 2 push
    glPushMatrix();

    glRotatef(angleBlueLight, 0.0f, 0.0f, 1.0f);
    lightTwoPosition[0] = angleRedLight;
    lightTwoPosition[1] = 0;
    lightTwoPosition[2] = 0;
    glLightfv(GL_LIGHT2, GL_POSITION, lightTwoPosition);

    // Light 2 pop
    glPopMatrix();

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
