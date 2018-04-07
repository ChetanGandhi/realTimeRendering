#include <windows.h>
#include <stdio.h>
#include <gl/glew.h>
#include <gl/gl.h>
#include <Sphere.h>

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
bool isLightingEnabled = false;

float sphereVertices[1146];
float sphereNormals[1146];
float sphereTextures[764];

unsigned short sphereElements[2280];

unsigned int numberOfSphereVertices = getNumberOfSphereVertices();
unsigned int numberOfSphereElements = getNumberOfSphereElements();

enum
{
    CG_ATTRIBUTE_VERTEX_POSITION = 0,
	CG_ATTRIBUTE_COLOR,
	CG_ATTRIBUTE_NORMAL,
	CG_ATTRIBUTE_TEXTURE0,
};

GLuint vertexShaderObject = 0;
GLuint fragmentShaderObject = 0;
GLuint shaderProgramObject = 0;

GLuint vaoSphere = 0;
GLuint vboSpherePosition = 0;
GLuint vboSphereNormal = 0;
GLuint vboSphereElement = 0;
GLuint modelMatrixUniform = 0;
GLuint viewMatrixUniform = 0;
GLuint projectionMatrixUniform = 0;
GLuint laUniform = 0;
GLuint ldUniform = 0;
GLuint lsUniform = 0;
GLuint kaUniform = 0;
GLuint kdUniform = 0;
GLuint ksUniform = 0;
GLuint materialShininessUniform = 0;
GLuint lightPositionUniform = 0;
GLuint isLightingEnabledUniform = 0;

vmath::mat4 perspectiveProjectionMatrix;

GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialShininess = 50.0f;

FILE *logFile = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

void initialize(void);
void listExtensions(void);
void initializeVertexShader(void);
void initializeFragmentShader(void);
void initializeShaderProgram(void);
void initializeSphereBuffers(void);
void cleanUp(void);
void display(void);
void drawSphere(void);
void resize(int width, int height);
void toggleFullscreen(HWND hWnd, bool isFullscreen);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsatnce, LPSTR lpszCmdLine, int nCmdShow)
{
    WNDCLASSEX wndClassEx;
    MSG message;
    TCHAR szApplicationTitle[] = TEXT("CG - PP - Per Vertex Light");
    TCHAR szApplicationClassName[] = TEXT("RTR_OPENGL_PP_PER_VERTEX_LIGHT");
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

                default:
                break;
            }

        break;

        case WM_CHAR:
            switch(wParam)
            {
                case 'L':
                case 'l':
                    isLightingEnabled = !isLightingEnabled;
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

    GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
        fprintf(logFile, "Cannot initialize GLEW, Error: %d", glewError);
        fflush(logFile);

        cleanUp();
        exit(EXIT_FAILURE);
	}

    listExtensions();

    getSphereVertexData(sphereVertices, sphereNormals, sphereTextures, sphereElements);
    numberOfSphereVertices = getNumberOfSphereVertices();
    numberOfSphereElements = getNumberOfSphereElements();

    // Initialize the shaders and shader program object.
    initializeVertexShader();
    initializeFragmentShader();
    initializeShaderProgram();
    initializeSphereBuffers();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);

    // Disable face culling to see back side of object when rotated.
    // glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);

    perspectiveProjectionMatrix = vmath::mat4::identity();

    isLightingEnabled = false;

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
        "in vec4 vertexPosition;" \
        "in vec3 vertexNormal;" \
        "\n" \
        "out vec3 phongAdsColor;" \
        "\n" \
        "uniform mat4 modelMatrix;" \
        "uniform mat4 viewMatrix;" \
        "uniform mat4 projectionMatrix;" \
        "uniform int isLightingEnabled;" \
        "uniform vec3 la;" \
        "uniform vec3 ld;" \
        "uniform vec3 ls;" \
        "uniform vec3 ka;" \
        "uniform vec3 kd;" \
        "uniform vec3 ks;" \
        "uniform vec4 lightPosition;" \
        "uniform float materialShininess;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   if(isLightingEnabled == 1)" \
        "   {" \
        "       vec4 eyeCoordinates = viewMatrix * modelMatrix * vertexPosition;" \
        "       vec3 tNormal = normalize(mat3(viewMatrix * modelMatrix) * vertexNormal);" \
        "       vec3 source = normalize(vec3(lightPosition) - eyeCoordinates.xyz);" \
        "       float tNormalDotLightDirection = max(dot(tNormal, source), 0.0);" \
        "       vec3 ambient = la * ka;" \
        "       vec3 diffuse = ld * kd * tNormalDotLightDirection;" \
        "       vec3 reflectionVector = reflect(-source, tNormal);" \
        "       vec3 viewVector = normalize(-eyeCoordinates.xyz);" \
        "       vec3 specular = ls * ks * pow(max(dot(reflectionVector, viewVector), 0.0), materialShininess);" \
        "       phongAdsColor = ambient + diffuse + specular;"
        "   }" \
        "   else" \
        "   {" \
        "       phongAdsColor = vec3(1.0, 1.0, 1.0);" \
        "   }" \
        "\n" \
        "   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;" \
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

void initializeFragmentShader()
{
    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar *fragmentShaderCode = "#version 450 core" \
        "\n" \
        "in vec3 phongAdsColor;" \
        "\n" \
        "out vec4 fragmentColor;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   fragmentColor = vec4(phongAdsColor, 1.0);" \
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
    glAttachShader(shaderProgramObject, fragmentShaderObject);

    // Bind the position attribute location before linking.
    glBindAttribLocation(shaderProgramObject, CG_ATTRIBUTE_VERTEX_POSITION, "vertexPosition");

    // Bind the normal attribute location before linking.
    glBindAttribLocation(shaderProgramObject, CG_ATTRIBUTE_NORMAL, "vertexNormal");

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

    // After linking get the value of modelView and projection uniform location from the shader program.
    modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "modelMatrix");
    viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "viewMatrix");
    projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "projectionMatrix");

    isLightingEnabledUniform = glGetUniformLocation(shaderProgramObject, "isLightingEnabled");

    laUniform = glGetUniformLocation(shaderProgramObject, "la");
    ldUniform = glGetUniformLocation(shaderProgramObject, "ld");
    lsUniform = glGetUniformLocation(shaderProgramObject, "ls");
    kaUniform = glGetUniformLocation(shaderProgramObject, "ka");
    kdUniform = glGetUniformLocation(shaderProgramObject, "kd");
    ksUniform = glGetUniformLocation(shaderProgramObject, "ks");
    lightPositionUniform = glGetUniformLocation(shaderProgramObject, "lightPosition");
    materialShininessUniform = glGetUniformLocation(shaderProgramObject, "materialShininess");
}

void initializeSphereBuffers()
{
    glGenVertexArrays(1, &vaoSphere);
    glBindVertexArray(vaoSphere);

    glGenBuffers(1, &vboSpherePosition);
    glBindBuffer(GL_ARRAY_BUFFER, vboSpherePosition);

    glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertices), sphereVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_VERTEX_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboSphereNormal);
    glBindBuffer(GL_ARRAY_BUFFER, vboSphereNormal);

    glBufferData(GL_ARRAY_BUFFER, sizeof(sphereNormals), sphereNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_NORMAL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboSphereElement);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSphereElement);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphereElements), sphereElements, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glUseProgram(shaderProgramObject);

    if(isLightingEnabled)
    {
        glUniform1i(isLightingEnabledUniform, 1);
        glUniform3fv(laUniform, 1, lightAmbient);
        glUniform3fv(ldUniform, 1, lightDiffuse);
        glUniform3fv(lsUniform, 1, lightSpecular);
        glUniform4fv(lightPositionUniform, 1, lightPosition);

        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDiffuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        glUniform1f(materialShininessUniform, materialShininess);
    }
    else
    {
        glUniform1i(isLightingEnabledUniform, 0);
    }

    drawSphere();

    glUseProgram(0);
    SwapBuffers(hdc);
}

void drawSphere()
{
    vmath::mat4 modelMatrix = vmath::mat4::identity();
    vmath::mat4 viewMatrix = vmath::mat4::identity();

    // Translate the modal view matrix.
    modelMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

    // Pass modelMatrix to vertex shader in 'modelMatrix' variable defined in shader.
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

    // Pass viewMatrix to vertex shader in 'viewMatrix' variable defined in shader.
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);

    // Pass perspectiveProjectionMatrix to vertex shader in 'projectionMatrix' variable defined in shader.
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

    // Now bind the VAO to which we want to use
    glBindVertexArray(vaoSphere);

    // Draw the sphere
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSphereElement);
    glDrawElements(GL_TRIANGLES, numberOfSphereElements, GL_UNSIGNED_SHORT, 0);

    // unbind the vao
    glBindVertexArray(0);
}

void resize(int width, int height)
{
    if(height == 0)
    {
        height = 1;
    }

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);
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

    if(vaoSphere)
    {
        glDeleteVertexArrays(1, &vaoSphere);
        vaoSphere = 0;
    }

    if(vboSpherePosition)
    {
        glDeleteBuffers(1, &vboSpherePosition);
        vboSpherePosition = 0;
    }

    if(vboSphereNormal)
    {
        glDeleteBuffers(1, &vboSphereNormal);
        vboSphereNormal = 0;
    }

    if(vboSphereElement)
    {
        glDeleteBuffers(1, &vboSphereElement);
        vboSphereElement = 0;
    }

    if(shaderProgramObject)
    {
        if(vertexShaderObject)
        {
            glDetachShader(shaderProgramObject, vertexShaderObject);
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
