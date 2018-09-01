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

GLfloat angleCube = 0.0f;
GLfloat speed = 0.1f;

GLfloat identityMatrix[16] = {0};
GLfloat translationMatrix[16] = {0};
GLfloat scaleMatrix[16] = {0};
GLfloat rotationXMatrix[16] = {0};
GLfloat rotationYMatrix[16] = {0};
GLfloat rotationZMatrix[16] = {0};

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

void initialize(void);
void initializeIdentityMatrix(void);
void initializeTranslationMatrix(void);
void initializeScaleMatrix(void);
void cleanUp(void);
void update(void);
void display(void);
void drawCube(void);
void resize(int width, int height);
void toggleFullscreen(HWND hWnd, bool isFullscreen);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsatnce, LPSTR lpszCmdLine, int nCmdShow)
{
    WNDCLASSEX wndClassEx;
    MSG message;
    TCHAR szApplicationTitle[] = TEXT("CG - Cube Rotation by Matrix");
    TCHAR szApplicationClassName[] = TEXT("RTR_OPENGL_CUBE_ROTATION_MATRIX");
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

    initializeIdentityMatrix();
    initializeTranslationMatrix();
    initializeScaleMatrix();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // This is required for DirectX
    resize(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
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

void initializeTranslationMatrix(void)
{
    // x, y, z, w
    // 1.0f, 0.0f, 0.0f, tx,
    // 0.0f, 1.0f, 0.0f, ty,
    // 0.0f, 0.0f, 1.0f, tz,
    // 0.0f, 0.0f, 0.0f, 1.0f
    translationMatrix[0] = 1.0f; translationMatrix[4] = 0.0f; translationMatrix[8] = 0.0f; translationMatrix[12] = 0.0f;
    translationMatrix[1] = 0.0f; translationMatrix[5] = 1.0f; translationMatrix[9] = 0.0f; translationMatrix[13] = 0.0f;
    translationMatrix[2] = 0.0f; translationMatrix[6] = 0.0f; translationMatrix[10] = 1.0f; translationMatrix[14] = -6.0f;
    translationMatrix[3] = 0.0f; translationMatrix[7] = 0.0f; translationMatrix[11] = 0.0f; translationMatrix[15] = 1.0f;
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

    if(angleCube <= -360.0f)
    {
        angleCube = 0.0f;
    }
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(identityMatrix);
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

    SwapBuffers(hdc);
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

void resize(int width, int height)
{
    if(height == 0)
    {
        height = 1;
    }

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(identityMatrix);
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
