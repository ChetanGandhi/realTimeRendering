#include <windows.h>
#include <stdio.h>
#include <gl/glew.h>
#include <gl/gl.h>

#include "resources/resource.h"
#include "vmath.h"

HWND hWnd = NULL;
HDC hdc = NULL;
HGLRC hrc = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
RECT windowRect = {0, 0, 800, 600};

bool isFullscreen = false;
bool isActive = false;
bool isEscapeKeyPressed = false;

enum
{
    CG_ATTRIBUTE_VERTEX_POSITION = 0,
	CG_ATTRIBUTE_COLOR,
	CG_ATTRIBUTE_NORMAL,
	CG_ATTRIBUTE_TEXTURE,
};

GLuint vertexShaderObject = 0;
GLuint tessellationControllShaderObject = 0;
GLuint tessellationEvaluationShaderObject = 0;
GLuint fragmentShaderObject = 0;
GLuint shaderProgramObject = 0;

GLuint vao = 0;
GLuint vbo = 0;
GLuint mvpUniform = 0;
GLuint numberOfSegmentsUniform = 0;
GLuint numberOfStripsUniform = 0;
GLuint lineColorUniform = 0;

vmath::mat4 perspectiveProjectionMatrix;

unsigned int numberOfLineSegments = 1;

FILE *logFile = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

void initialize(void);
void listExtensions(void);
void initializeVertexShader(void);
void initializeTessellationControllShader(void);
void initializeTessellationEvaluationShader(void);
void initializeFragmentShader(void);
void initializeShaderProgram(void);
void initializeBuffers(void);
void cleanUp(void);
void display(void);
void resize(int width, int height);
void toggleFullscreen(HWND hWnd, bool isFullscreen);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsatnce, LPSTR lpszCmdLine, int nCmdShow)
{
    WNDCLASSEX wndClassEx;
    MSG message;
    TCHAR szApplicationTitle[] = TEXT("CG - PP - Tessellation Shader");
    TCHAR szApplicationClassName[] = TEXT("RTR_OPENGL_PP_TESSELLATION_SHADER");
    bool done = false;

	if (fopen_s(&logFile, "debug.log", "w") != 0)
	{
		MessageBox(NULL, TEXT("Unable to open log file."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(EXIT_FAILURE);
	}

    fprintf(logFile, "---------- CG: OpenGL Debug Logs Start ----------\n");
    fflush(logFile);

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
                    display();
                }
            }
        }
    }

    cleanUp();

    fprintf(logFile, "---------- CG: OpenGL Debug Logs End ----------\n");
    fflush(logFile);
    fclose(logFile);

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

                case VK_UP:
                    numberOfLineSegments++;

                    if(numberOfLineSegments >= 50)
                    {
                        numberOfLineSegments = 50;
                    }

                break;

                case VK_DOWN:
                    numberOfLineSegments--;

                    if(numberOfLineSegments <= 1)
                    {
                        numberOfLineSegments = 1;
                    }

                break;

                default:
                break;
            }

        break;

        case WM_CHAR:
            switch(wParam)
            {
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
    initializeTessellationControllShader();
    initializeTessellationEvaluationShader();
    initializeFragmentShader();
    initializeShaderProgram();
    initializeBuffers();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glLineWidth(3.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    perspectiveProjectionMatrix = vmath::mat4::identity();
    numberOfLineSegments = 1;

    // This is required for DirectX
    resize(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
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
        "in vec2 vertexPosition;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   gl_Position = vec4(vertexPosition, 0.0, 1.0);" \
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

void initializeTessellationControllShader()
{
    tessellationControllShaderObject = glCreateShader(GL_TESS_CONTROL_SHADER);

    const GLchar *tessellationControllShaderCode = "#version 450 core" \
        "\n" \
        "layout(vertices = 4) out;" \
        "uniform int numberOfSegments;" \
		"uniform int numberOfStrips;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;" \
        "   gl_TessLevelOuter[0] = float(numberOfStrips);" \
        "   gl_TessLevelOuter[1] = float(numberOfSegments);" \
        "}";

    glShaderSource(tessellationControllShaderObject, 1, (const char**)&tessellationControllShaderCode, NULL);
    glCompileShader(tessellationControllShaderObject);

    GLint infoLogLength = 0;
    GLint shaderCompileStatus = 0;
    char *infoLog = NULL;

    glGetShaderiv(tessellationControllShaderObject, GL_COMPILE_STATUS, &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(tessellationControllShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetShaderInfoLog(tessellationControllShaderObject, infoLogLength, &written, infoLog);
                fprintf(logFile, "CG: Tessellation controll shader compilation log: %s\n", infoLog);
                free(infoLog);
                cleanUp();
                exit(EXIT_FAILURE);
            }
        }
    }
}

void initializeTessellationEvaluationShader()
{
    tessellationEvaluationShaderObject = glCreateShader(GL_TESS_EVALUATION_SHADER);

    const GLchar *tessellationEvaluationShaderCode = "#version 450 core" \
        "\n" \
        "layout(isolines) in;" \
        "uniform mat4 mvpMatrix;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   vec3 p0 = gl_in[0].gl_Position.xyz;" \
		"   vec3 p1 = gl_in[1].gl_Position.xyz;" \
		"   vec3 p2 = gl_in[2].gl_Position.xyz;" \
		"   vec3 p3 = gl_in[3].gl_Position.xyz;" \
        "\n" \
        "   /* Bernstein equation */" \
        "   float u = gl_TessCoord.x;" \
        "   float u1 = (1.0 - u);" \
		"   float u2 = u * u;" \
        "\n" \
        "   float b3 = u2 * u;" \
		"   float b2 = 3.0 * u2 * u1;" \
		"   float b1 = 3.0 * u * u1 * u1;" \
		"   float b0 = u1 * u1 * u1;" \
        "\n" \
        "   vec3 p = p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;" \
		"   gl_Position = mvpMatrix * vec4(p, 1.0);" \
        "}";

    glShaderSource(tessellationEvaluationShaderObject, 1, (const char**)&tessellationEvaluationShaderCode, NULL);
    glCompileShader(tessellationEvaluationShaderObject);

    GLint infoLogLength = 0;
    GLint shaderCompileStatus = 0;
    char *infoLog = NULL;

    glGetShaderiv(tessellationEvaluationShaderObject, GL_COMPILE_STATUS, &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(tessellationEvaluationShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetShaderInfoLog(tessellationEvaluationShaderObject, infoLogLength, &written, infoLog);
                fprintf(logFile, "CG: Tessellation evaluation shader compilation log: %s\n", infoLog);
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
        "uniform vec4 lineColor;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   fragmentColor = lineColor;" \
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
    glAttachShader(shaderProgramObject, tessellationControllShaderObject);
    glAttachShader(shaderProgramObject, tessellationEvaluationShaderObject);
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
    numberOfSegmentsUniform = glGetUniformLocation(shaderProgramObject, "numberOfSegments");
    numberOfStripsUniform = glGetUniformLocation(shaderProgramObject, "numberOfStrips");
    lineColorUniform = glGetUniformLocation(shaderProgramObject, "lineColor");
}

void initializeBuffers()
{
    const GLfloat vertices[] = {
        -1.0f, -1.0f,
        -0.5f, 1.0f,
        0.5f, -1.0f,
        1.0f, 1.0f
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_VERTEX_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_VERTEX_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glUseProgram(shaderProgramObject);

    vmath::mat4 modelViewMatrix = vmath::mat4::identity();
    vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();

    modelViewMatrix = vmath::translate(0.5f, 0.5f, -2.0f);
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glUniform1i(numberOfSegmentsUniform, numberOfLineSegments);
    glUniform1i(numberOfStripsUniform, 1);
	glUniform4fv(lineColorUniform, 1, vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f));

    TCHAR title[255];
    memset(title, 0, 255 * sizeof(TCHAR));

	wsprintf(title, TEXT("CG - PP - Tessellation Shader | Segments = %d"), numberOfLineSegments);
	SetWindowText(hWnd, title);

    glBindVertexArray(vao);
    glDrawArrays(GL_PATCHES, 0, 4);
    glBindVertexArray(0);

    glUseProgram(0);
    SwapBuffers(hdc);
}

void resize(int width, int height)
{
    if(height == 0)
    {
        height = 1;
    }

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
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

        if(tessellationControllShaderObject)
        {
            glDetachShader(shaderProgramObject, tessellationControllShaderObject);
        }

        if(tessellationEvaluationShaderObject)
        {
            glDetachShader(shaderProgramObject, tessellationEvaluationShaderObject);
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

    if(tessellationControllShaderObject)
    {
        glDeleteShader(tessellationControllShaderObject);
        tessellationControllShaderObject = 0;
    }

    if(tessellationEvaluationShaderObject)
    {
        glDeleteShader(tessellationEvaluationShaderObject);
        tessellationEvaluationShaderObject = 0;
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

    wglMakeCurrent(NULL, NULL);

    wglDeleteContext(hrc);
    hrc = NULL;

    ReleaseDC(hWnd, hdc);
    hdc = NULL;

    DestroyWindow(hWnd);
    hWnd = NULL;
}
