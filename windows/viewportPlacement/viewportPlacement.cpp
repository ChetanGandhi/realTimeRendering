#include <windows.h>
#include <cstdlib>
#include <gl/gl.h>
#include <gl/glu.h>

#include "resources/resource.h"

HWND hWnd = NULL;
HDC hdc = NULL;
HGLRC hrc = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
RECT windowRect = {0, 0, 800, 600};
RECT viewportRect = {0, 0, 800, 600};

bool isFullscreen = false;
bool isActive = false;
bool isEscapeKeyPressed = false;
bool changeViportPlacement = false;

int viewportPlacement = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

void initialize(void);
void cleanUp(void);
void display(void);
void drawTriangle(void);
void resize(int x, int y, int width, int height);
void updateViewportPosition(int position, int clientWidth, int clientHeight);
void toggleFullscreen(HWND hWnd, bool isFullscreen);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsatnce, LPSTR lpszCmdLine, int nCmdShow)
{
    WNDCLASSEX wndClassEx;
    MSG message;
    TCHAR szApplicationTitle[] = TEXT("CG - Viewport Placement");
    TCHAR szApplicationClassName[] = TEXT("RTR_OPENGL_VIEWPORT_PLACEMENT");
    bool done = false;

    wndClassEx.cbSize = sizeof(WNDCLASSEX);
    wndClassEx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndClassEx.cbClsExtra = 0;
    wndClassEx.cbWndExtra = 0;
    wndClassEx.lpfnWndProc = WndProc;
    wndClassEx.hInstance = hInstance;
    wndClassEx.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(CP_ICON));
    wndClassEx.hIconSm = LoadIcon(hInstance,MAKEINTRESOURCE(CP_ICON_SMALL));
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
            isActive  = (HIWORD(wParam) == 0);
        break;

        case WM_SIZE:
            GetClientRect(hWnd, &windowRect);
            updateViewportPosition(viewportPlacement, LOWORD(lParam), HIWORD(lParam));
            resize(
                viewportRect.left,
                viewportRect.top,
                viewportRect.right - viewportRect.left,
                viewportRect.bottom - viewportRect.top
            );
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

            if(wParam >= 0x30 && wParam <= 0x39)
            {
                viewportPlacement = wParam % 0x30;
                changeViportPlacement = true;
            }
            else if(wParam >= 0x60 && wParam <= 0x69)
            {
                viewportPlacement = wParam % 0x60;
                changeViportPlacement = true;
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
    int pixelFormatIndex;

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

    // This is required for DirectX
    resize(viewportRect.left, viewportRect.top, viewportRect.right - viewportRect.left, viewportRect.bottom - viewportRect.top);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    if(changeViportPlacement)
    {
        changeViportPlacement = false;
        updateViewportPosition(
            viewportPlacement,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top
        );
        glViewport(
            (GLint)viewportRect.left,
            (GLint)viewportRect.top,
            (GLsizei)(viewportRect.right - viewportRect.left),
            (GLsizei)(viewportRect.bottom - viewportRect.top)
        );
    }

    drawTriangle();
    SwapBuffers(hdc);
}

void drawTriangle(void)
{
    glLoadIdentity();
    glBegin(GL_TRIANGLES);

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);

    glEnd();
}

void resize(int x, int y, int width, int height)
{
    if(height == 0)
    {
        height = 1;
    }

    glViewport((GLint)x, (GLint)y, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(0.0f, (GLfloat)width/(GLsizei)height, -1.0f, 1.0f);
}

void updateViewportPosition(int position, int clientWidth, int clientHeight)
{
    switch(viewportPlacement)
    {
        case 1:
            viewportRect.left = 0;
            viewportRect.top = 0;
            viewportRect.right = clientWidth / 2;
            viewportRect.bottom = clientHeight / 2;
        break;

        case 2:
            viewportRect.left = clientWidth / 2;
            viewportRect.top = 0;
            viewportRect.right = clientWidth;
            viewportRect.bottom = clientHeight / 2;
        break;

        case 3:
            viewportRect.left = clientWidth / 2;
            viewportRect.top = clientHeight / 2;
            viewportRect.right = clientWidth;
            viewportRect.bottom = clientHeight;
        break;

        case 4:
            viewportRect.left = 0;
            viewportRect.top = clientHeight / 2;
            viewportRect.right = clientWidth / 2;
            viewportRect.bottom = clientHeight;
        break;

        case 5:
            viewportRect.left = 0;
            viewportRect.top = 0;
            viewportRect.right = clientWidth / 2;
            viewportRect.bottom = clientHeight;
        break;

        case 6:
            viewportRect.left = clientWidth / 2;
            viewportRect.top = 0;
            viewportRect.right = clientWidth ;
            viewportRect.bottom = clientHeight;
        break;

        case 7:
            viewportRect.left = 0;
            viewportRect.top = 0;
            viewportRect.right = clientWidth;
            viewportRect.bottom = clientHeight / 2;
        break;

        case 8:
            viewportRect.left = 0;
            viewportRect.top = clientHeight / 2;
            viewportRect.right = clientWidth;
            viewportRect.bottom = clientHeight;
        break;

        case 9:
            viewportRect.left = clientWidth / 4;
            viewportRect.top = clientHeight / 4;
            viewportRect.right = clientWidth  - (clientWidth / 4);
            viewportRect.bottom = clientHeight - (clientHeight / 4);
        break;

        case 0:
        default:
            viewportRect.left = 0;
            viewportRect.top = 0;
            viewportRect.right = clientWidth;
            viewportRect.bottom = clientHeight;
        break;
    }
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
