#include <windows.h>
#include <cstdlib>
#include <gl/gl.h>
#include <gl/glu.h>

#include "resources/resource.h"
#include "objParser.cpp"

#define OBJ_FILE_PATH "./resources/models/monkeyHead.obj"

HWND hWnd = NULL;
HDC hdc = NULL;
HGLRC hrc = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
RECT windowRect = {0, 0, 800, 600};

MeshData monkeyHeadMeshData;

bool isFullscreen = false;
bool isActive = false;
bool isEscapeKeyPressed = false;
bool isLightingEnabled = false;

GLboolean isWireframeEnabled = GL_TRUE;

GLfloat angleMonkeyHead = 0.0f;
GLfloat speed = 0.1f;

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
GLfloat materialSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat matrialShininess[] = {50.0f, 50.0f, 50.0f, 50.0f};

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

void initialize(void);
void cleanUp(void);
void update();
void display(void);
void drawMonkeyHead();
void resize(int width, int height);
void toggleFullscreen(HWND hWnd, bool isFullscreen);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsatnce, LPSTR lpszCmdLine, int nCmdShow)
{
    WNDCLASSEX wndClassEx;
    MSG message;
    TCHAR szApplicationTitle[] = TEXT("CG - Monkey Head");
    TCHAR szApplicationClassName[] = TEXT("RTR_OPENGL_MONKEY_HEAD");
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
                    }
                    else {
                        isLightingEnabled = false;
                        glDisable(GL_LIGHTING);
                        glDisable(GL_LIGHT0);
                        glDisable(GL_LIGHT1);
                    }
                break;

                case 'w':
                case 'W':
                    if(isWireframeEnabled == GL_TRUE)
                    {
                        isWireframeEnabled = GL_FALSE;
                    }
                    else
                    {
                        isWireframeEnabled = GL_TRUE;
                    };
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
    glLightfv(GL_LIGHT0, GL_POSITION, lightZeroPosition);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightZeroSpecular);

    glLightfv(GL_LIGHT1, GL_AMBIENT, lightOneAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightOneDiffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, lightOnePosition);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightOneSpecular);

    glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matrialShininess);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    bool meshDataLoaded = loadMeshData(OBJ_FILE_PATH, &monkeyHeadMeshData);

    if(!meshDataLoaded)
    {
        cleanUp();
        MessageBox(NULL, TEXT("Not able to load mesh data, file not found."), TEXT("Error"), MB_OK | MB_ICONERROR);
        exit(EXIT_FAILURE);
    }

    // This is required for DirectX
    resize(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
}

void update(void)
{
    angleMonkeyHead += speed;

    if(angleMonkeyHead >= 360.0f)
    {
        angleMonkeyHead = 0.0f;
    }
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -7.0f);
    glRotatef(angleMonkeyHead, 0.0f, 1.0f, 0.0f);
    glScalef(1.5f, 1.5f, 1.5f);

    drawMonkeyHead();

    SwapBuffers(hdc);
}

void drawMonkeyHead()
{
    glFrontFace(GL_CCW);

    if(isWireframeEnabled == GL_TRUE)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    for(int triangleCounter = 0; triangleCounter < monkeyHeadMeshData.faceTriangles.size(); ++triangleCounter)
    {
        glBegin(GL_TRIANGLES);

        for(int vertexCounter = 0; vertexCounter < monkeyHeadMeshData.faceTriangles[triangleCounter].size(); ++vertexCounter)
        {
            int vertexIndex = monkeyHeadMeshData.faceTriangles[triangleCounter][vertexCounter] - 1;
            int normalIndex = monkeyHeadMeshData.faceNormals[triangleCounter][vertexCounter] - 1;

            glNormal3fv(monkeyHeadMeshData.normals[normalIndex].data());
            glVertex3fv(monkeyHeadMeshData.vertices[vertexIndex].data());
        }

        glEnd();
    }
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
