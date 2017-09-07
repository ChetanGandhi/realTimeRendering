#define _USE_MATH_DEFINES

#include <windows.h>
#include <Mmsystem.h>
#include <cstdlib>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <time.h>
#include <stdio.h>

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
bool stopTimer = false;
bool doneIOne = false;
bool doneN = false;
bool doneD = false;
bool doneITwo = false;
bool doneA = false;
bool doneFlag = false;

double lastSeconds = 0.0;

const GLfloat LINE_WIDTH = 5.0f;
const GLfloat Z_TRANSLATION = -6.0f;

GLfloat translationFactorIOne = 0.0f;
GLfloat translationFactorN = 0.0f;
GLfloat translationFactorD = 0.0f;
GLfloat translationFactorITwo = 0.0f;
GLfloat translationFactorA = 0.0f;
GLfloat translationFactorFlag = 0.0f;
GLfloat colorSaffron[3] = {255.0f / 255.0f, 153.0f / 255.0f, 51.0f / 255.0f};
GLfloat colorIndiaGreen[3] = {19.0f / 255.0f, 136.0f / 255.0f, 8.0f / 255.0f};

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

void initialize(void);
void cleanUp(void);
void update(void);
void display(void);
void drawIndia(void);
bool drawIOne(void);
bool drawITwo(void);
bool drawN(void);
bool drawD(void);
bool drawA(void);
bool drawFlag(void);
void resize(int width, int height);
void toggleFullscreen(HWND hWnd, bool isFullscreen);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsatnce, LPSTR lpszCmdLine, int nCmdShow)
{
    WNDCLASSEX wndClassEx;
    MSG message;
    TCHAR szApplicationTitle[] = TEXT("CG - India");
    TCHAR szApplicationClassName[] = TEXT("RTR_OPENGL_INDIA");
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

    PlaySound(L"resources/audio/satyamevJayate.wav", NULL, SND_ASYNC | SND_NODEFAULT | SND_FILENAME);

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

    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // This is required for DirectX
    resize(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
}

void update()
{
    double difference = 0.0f;

    if(!stopTimer)
    {
        static time_t start = time(NULL);
        time_t now = time(NULL);
        difference = difftime(now, start);

        if(difference >= 11.0f)
        {
            if(!doneIOne)
            {
                translationFactorIOne += difference / 100000.0;
            }

            if(doneIOne && !doneN)
            {
                translationFactorN += difference / 100000.0;
            }

            if(doneN && !doneD)
            {
                translationFactorD += difference / 100000.0;
            }

            if(doneD && !doneITwo)
            {
                translationFactorITwo += difference / 100000.0;
            }

            if(doneITwo && !doneA)
            {
                translationFactorA += difference / 100000.0;
            }

            if(doneA && !doneFlag)
            {
                translationFactorFlag += difference / 100000.0;
            }
        }
    }

    // const size_t len = 256;
    // wchar_t buffer[len] = {};
    // _snwprintf(buffer, len - 1, L"%lf | %lf | %lf | %lf | %lf | %lf | %lf", difference, translationFactorIOne, translationFactorN, translationFactorD, translationFactorITwo, translationFactorA, translationFactorFlag);
    // SetWindowText(hWnd, buffer);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawIndia();
    SwapBuffers(hdc);
}

void drawIndia(void)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    doneIOne = drawIOne();
    doneN = drawN();
    doneD = drawD();
    doneITwo = drawITwo();
    doneA = drawA();
    doneFlag = drawFlag();
    stopTimer = doneIOne && doneN && doneD && doneITwo && doneA && doneFlag;
 }

bool drawIOne()
{
    GLfloat position = -4.5 + translationFactorIOne;
    bool done = false;

    if(position > -2.75f)
    {
        position = -2.75f;
        done = true;
    }

    glLoadIdentity();
    glTranslatef(position, 0.0f, Z_TRANSLATION);

    glLineWidth(LINE_WIDTH);
    glBegin(GL_LINES);

    glColor3f(colorSaffron[0], colorSaffron[1], colorSaffron[2]);
    glVertex3f(0.0f, 1.0f, 0.0f);

    glColor3f(colorIndiaGreen[0], colorIndiaGreen[1], colorIndiaGreen[2]);
    glVertex3f(0.0f, -1.0f, 0.0f);

    glEnd();

    return done;
}

bool drawITwo()
{
    GLfloat position = -3.5f + translationFactorITwo;
    bool done = false;

    if(position > 0.0f)
    {
        position = 0.0f;
        done = true;
    }

    glLoadIdentity();
    glTranslatef(1.5f, position, Z_TRANSLATION);

    glLineWidth(LINE_WIDTH);
    glBegin(GL_LINES);

    glColor3f(colorSaffron[0], colorSaffron[1], colorSaffron[2]);
    glVertex3f(0.0f, 1.0f, 0.0f);

    glColor3f(colorIndiaGreen[0], colorIndiaGreen[1], colorIndiaGreen[2]);
    glVertex3f(0.0f, -1.0f, 0.0f);

    glEnd();

    return done;
}

bool drawN()
{
    GLfloat position = 3.5f - translationFactorN;
    bool done = false;

    if(position < 0.0f)
    {
        position = 0.0f;
        done = true;
    }

    glLoadIdentity();
    glTranslatef(-1.5, position, Z_TRANSLATION);

    glLineWidth(LINE_WIDTH);
    glBegin(GL_LINES);

    glColor3f(colorSaffron[0], colorSaffron[1], colorSaffron[2]);
    glVertex3f(0.25f, 1.0f, 0.0f);
    glColor3f(colorIndiaGreen[0], colorIndiaGreen[1], colorIndiaGreen[2]);
    glVertex3f(0.25f, -1.0f, 0.0f);

    glColor3f(colorIndiaGreen[0], colorIndiaGreen[1], colorIndiaGreen[2]);
    glVertex3f(0.25f, -1.0f, 0.0f);
    glColor3f(colorSaffron[0], colorSaffron[1], colorSaffron[2]);
    glVertex3f(-0.25f, 1.0f, 0.0f);

    glColor3f(colorSaffron[0], colorSaffron[1], colorSaffron[2]);
    glVertex3f(-0.25f, 1.0f, 0.0f);
    glColor3f(colorIndiaGreen[0], colorIndiaGreen[1], colorIndiaGreen[2]);
    glVertex3f(-0.25f, -1.0f, 0.0f);

    glEnd();

    return done;
}

bool drawD()
{
    GLfloat colorFactor = 0.0f + translationFactorD;
    bool done = false;

    if(colorFactor > 1.0f)
    {
        colorFactor = 1.0f;
        done = true;
    }

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, Z_TRANSLATION);

    glLineWidth(LINE_WIDTH);
    glBegin(GL_LINES);

    glColor3f(colorSaffron[0] * colorFactor, colorSaffron[1] * colorFactor, colorSaffron[2] * colorFactor);
    glVertex3f(0.25f, 1.0f, 0.0f);
    glColor3f(colorIndiaGreen[0] * colorFactor, colorIndiaGreen[1] * colorFactor, colorIndiaGreen[2] * colorFactor);
    glVertex3f(0.25f, -1.0f, 0.0f);

    glColor3f(colorSaffron[0] * colorFactor, colorSaffron[1] * colorFactor, colorSaffron[2] * colorFactor);
    glVertex3f(0.25f + (LINE_WIDTH * 0.5f * 0.01f), 1.0f, 0.0f);
    glVertex3f(-0.3f, 1.0f, 0.0f);

    glColor3f(colorSaffron[0] * colorFactor, colorSaffron[1] * colorFactor, colorSaffron[2] * colorFactor);
    glVertex3f(-0.25f, 1.0f, 0.0f);
    glColor3f(colorIndiaGreen[0] * colorFactor, colorIndiaGreen[1] * colorFactor, colorIndiaGreen[2] * colorFactor);
    glVertex3f(-0.25f, -1.0f, 0.0f);

    glColor3f(colorIndiaGreen[0] * colorFactor, colorIndiaGreen[1] * colorFactor, colorIndiaGreen[2] * colorFactor);
    glVertex3f(0.25f + (LINE_WIDTH * 0.5f * 0.01f), -1.0f, 0.0f);
    glVertex3f(-0.3f, -1.0f, 0.0f);

    glEnd();

    return done;
}

bool drawA()
{
    GLfloat position = 5.5f - translationFactorA;
    bool done = false;

    if(position < 2.75f)
    {
        position = 2.75f;
        done = true;
    }

    glLoadIdentity();
    glTranslatef(position, 0.0f, Z_TRANSLATION);

    glLineWidth(LINE_WIDTH);
    glBegin(GL_LINES);

    glColor3f(colorSaffron[0], colorSaffron[1], colorSaffron[2]);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glColor3f(colorIndiaGreen[0], colorIndiaGreen[1], colorIndiaGreen[2]);
    glVertex3f(-0.25f, -1.0f, 0.0f);

    glColor3f(colorIndiaGreen[0], colorIndiaGreen[1], colorIndiaGreen[2]);
    glVertex3f(0.25f, -1.0f, 0.0f);
    glColor3f(colorSaffron[0], colorSaffron[1], colorSaffron[2]);
    glVertex3f(0.0f, 1.0f, 0.0f);

    glEnd();

    return done;
}

bool drawFlag()
{
    GLfloat start = -13.5f + translationFactorFlag * 3.0f;
    GLfloat end = -6.5f + translationFactorFlag * 3.0f;
    bool done = false;

    if(end > 2.75f)
    {
        end = 2.75f;
    }

    if(start > 2.75f)
    {
        start = 2.75f;
        done = true;
    }

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, Z_TRANSLATION);

    glLineWidth(LINE_WIDTH);
    glBegin(GL_LINES);

    glColor3f(colorSaffron[0], colorSaffron[1], colorSaffron[2]);
    glVertex3f(start - 0.13f, 0.025f, 0.0f);
    glVertex3f(end + 0.13f, 0.025f, 0.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(start - 0.14f, 0.0f, 0.0f);
    glVertex3f(end + 0.14f, 0.0f, 0.0f);

    glColor3f(colorIndiaGreen[0], colorIndiaGreen[1], colorIndiaGreen[2]);
    glVertex3f(start - 0.15f, -0.025, 0.0f);
    glVertex3f(end + 0.15f, -0.025, 0.0f);

    glEnd();

    return done;
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

    wglMakeCurrent(NULL, NULL);

    wglDeleteContext(hrc);
    hrc = NULL;

    ReleaseDC(hWnd, hdc);
    hdc = NULL;

    DestroyWindow(hWnd);
    hWnd = NULL;
}
