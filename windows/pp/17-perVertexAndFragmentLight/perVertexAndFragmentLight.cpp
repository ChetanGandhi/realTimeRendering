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
bool isPerFragmentLightEnabled = false;

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

GLuint vertexShaderObjectPerVertex = 0;
GLuint fragmentShaderObjectPerVertex = 0;
GLuint shaderProgramObjectPerVertex = 0;

GLuint vaoSpherePerVertex = 0;
GLuint vboSpherePositionPerVertex = 0;
GLuint vboSphereNormalPerVertex = 0;
GLuint vboSphereElementPerVertex = 0;
GLuint modelMatrixUniformPerVertex = 0;
GLuint viewMatrixUniformPerVertex = 0;
GLuint projectionMatrixUniformPerVertex = 0;
GLuint laUniformPerVertex = 0;
GLuint ldUniformPerVertex = 0;
GLuint lsUniformPerVertex = 0;
GLuint kaUniformPerVertex = 0;
GLuint kdUniformPerVertex = 0;
GLuint ksUniformPerVertex = 0;
GLuint materialShininessUniformPerVertex = 0;
GLuint lightPositionUniformPerVertex = 0;
GLuint isLightingEnabledUniformPerVertex = 0;

GLuint vertexShaderObjectPerFragment = 0;
GLuint fragmentShaderObjectPerFragment = 0;
GLuint shaderProgramObjectPerFragment = 0;

GLuint vaoSpherePerFragment = 0;
GLuint vboSpherePositionPerFragment = 0;
GLuint vboSphereNormalPerFragment = 0;
GLuint vboSphereElementPerFragment = 0;
GLuint modelMatrixUniformPerFragment = 0;
GLuint viewMatrixUniformPerFragment = 0;
GLuint projectionMatrixUniformPerFragment = 0;
GLuint laUniformPerFragment = 0;
GLuint ldUniformPerFragment = 0;
GLuint lsUniformPerFragment = 0;
GLuint kaUniformPerFragment = 0;
GLuint kdUniformPerFragment = 0;
GLuint ksUniformPerFragment = 0;
GLuint materialShininessUniformPerFragment = 0;
GLuint lightPositionUniformPerFragment = 0;
GLuint isLightingEnabledUniformPerFragment = 0;

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
void initializeVertexShaderPerVertex(void);
void initializeFragmentShaderPerVertex(void);
void initializeShaderProgramPerVertex(void);
void initializeSphereBuffersPerVertex(void);
void initializeVertexShaderPerFragment(void);
void initializeFragmentShaderPerFragment(void);
void initializeShaderProgramPerFragment(void);
void initializeSphereBuffersPerFragment(void);
void cleanUp(void);
void display(void);
void usePerVertexLight(void);
void usePerFragmentLight(void);
void drawSpherePerVertex(void);
void drawSpherePerFragment(void);
void resize(int width, int height);
void toggleFullscreen(HWND hWnd, bool isFullscreen);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsatnce, LPSTR lpszCmdLine, int nCmdShow)
{
    WNDCLASSEX wndClassEx;
    MSG message;
    TCHAR szApplicationTitle[] = TEXT("CG - PP - Per Vertex and Fragment Light");
    TCHAR szApplicationClassName[] = TEXT("RTR_OPENGL_PP_PER_VERTEX_FRAGMENT_LIGHT");
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

                case VK_F11:
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
                case 'F':
                case 'f':
                    isPerFragmentLightEnabled = true;
                break;

                case 'L':
                case 'l':
                    isLightingEnabled = !isLightingEnabled;
                break;

                case 'V':
                case 'v':
                    isPerFragmentLightEnabled = false;
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
    initializeVertexShaderPerVertex();
    initializeFragmentShaderPerVertex();
    initializeShaderProgramPerVertex();
    initializeSphereBuffersPerVertex();
    initializeVertexShaderPerFragment();
    initializeFragmentShaderPerFragment();
    initializeShaderProgramPerFragment();
    initializeSphereBuffersPerFragment();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);

    // Disable face culling to see back side of object when rotated.
    // glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);

    perspectiveProjectionMatrix = vmath::mat4::identity();

    isLightingEnabled = false;
    isPerFragmentLightEnabled = false;

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

void initializeVertexShaderPerVertex()
{
    vertexShaderObjectPerVertex = glCreateShader(GL_VERTEX_SHADER);

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

    glShaderSource(vertexShaderObjectPerVertex, 1, (const char**)&vertexShaderCode, NULL);
    glCompileShader(vertexShaderObjectPerVertex);

    GLint infoLogLength = 0;
    GLint shaderCompileStatus = 0;
    char *infoLog = NULL;

    glGetShaderiv(vertexShaderObjectPerVertex, GL_COMPILE_STATUS, &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObjectPerVertex, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetShaderInfoLog(vertexShaderObjectPerVertex, infoLogLength, &written, infoLog);
                fprintf(logFile, "CG: Vertex shader compilation log: %s\n", infoLog);
                free(infoLog);
                cleanUp();
                exit(EXIT_FAILURE);
            }
        }
    }
}

void initializeFragmentShaderPerVertex()
{
    fragmentShaderObjectPerVertex = glCreateShader(GL_FRAGMENT_SHADER);

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

    glShaderSource(fragmentShaderObjectPerVertex, 1, (const char**)&fragmentShaderCode, NULL);
    glCompileShader(fragmentShaderObjectPerVertex);

    GLint infoLogLength = 0;
    GLint shaderCompileStatus = 0;
    char *infoLog = NULL;

    glGetShaderiv(fragmentShaderObjectPerVertex, GL_COMPILE_STATUS, &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObjectPerVertex, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetShaderInfoLog(fragmentShaderObjectPerVertex, infoLogLength, &written, infoLog);
                fprintf(logFile, "CG: Fragment shader compilation log: %s\n", infoLog);
                free(infoLog);
                cleanUp();
                exit(EXIT_FAILURE);
            }
        }
    }
}

void initializeShaderProgramPerVertex()
{
    shaderProgramObjectPerVertex = glCreateProgram();
    glAttachShader(shaderProgramObjectPerVertex, vertexShaderObjectPerVertex);
    glAttachShader(shaderProgramObjectPerVertex, fragmentShaderObjectPerVertex);

    // Bind the position attribute location before linking.
    glBindAttribLocation(shaderProgramObjectPerVertex, CG_ATTRIBUTE_VERTEX_POSITION, "vertexPosition");

    // Bind the normal attribute location before linking.
    glBindAttribLocation(shaderProgramObjectPerVertex, CG_ATTRIBUTE_NORMAL, "vertexNormal");

    // Now link and check for error.
    glLinkProgram(shaderProgramObjectPerVertex);

    GLint infoLogLength = 0;
    GLint shaderProgramLinkStatus = 0;
    char *infoLog = NULL;

    glGetProgramiv(shaderProgramObjectPerVertex, GL_LINK_STATUS, &shaderProgramLinkStatus);

    if(shaderProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObjectPerVertex, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetProgramInfoLog(shaderProgramObjectPerVertex, infoLogLength, &written, infoLog);
                fprintf(logFile, "CG: Shader program link log: %s\n", infoLog);
                free(infoLog);
                cleanUp();
                exit(EXIT_FAILURE);
            }
        }
    }

    // After linking get the value of modelView and projection uniform location from the shader program.
    modelMatrixUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "modelMatrix");
    viewMatrixUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "viewMatrix");
    projectionMatrixUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "projectionMatrix");

    isLightingEnabledUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "isLightingEnabled");

    laUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "la");
    ldUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "ld");
    lsUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "ls");
    kaUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "ka");
    kdUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "kd");
    ksUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "ks");
    lightPositionUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "lightPosition");
    materialShininessUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "materialShininess");
}

void initializeSphereBuffersPerVertex()
{
    glGenVertexArrays(1, &vaoSpherePerVertex);
    glBindVertexArray(vaoSpherePerVertex);

    glGenBuffers(1, &vboSpherePositionPerVertex);
    glBindBuffer(GL_ARRAY_BUFFER, vboSpherePositionPerVertex);

    glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertices), sphereVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_VERTEX_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboSphereNormalPerVertex);
    glBindBuffer(GL_ARRAY_BUFFER, vboSphereNormalPerVertex);

    glBufferData(GL_ARRAY_BUFFER, sizeof(sphereNormals), sphereNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_NORMAL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboSphereElementPerVertex);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSphereElementPerVertex);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphereElements), sphereElements, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void initializeVertexShaderPerFragment()
{
    vertexShaderObjectPerFragment = glCreateShader(GL_VERTEX_SHADER);

    const GLchar *vertexShaderCode = "#version 450 core" \
        "\n" \
        "in vec4 vertexPosition;" \
        "in vec3 vertexNormal;" \
        "\n" \
        "out vec3 tNormal;" \
		"out vec3 source;" \
		"out vec3 viewVector;" \
		"\n" \
        "uniform mat4 modelMatrix;" \
        "uniform mat4 viewMatrix;" \
        "uniform mat4 projectionMatrix;" \
        "uniform vec4 lightPosition;" \
        "uniform int isLightingEnabled;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   if(isLightingEnabled == 1)" \
        "   {" \
        "       vec4 eyeCoordinates = viewMatrix * modelMatrix * vertexPosition;" \
        "       tNormal = mat3(viewMatrix * modelMatrix) * vertexNormal;" \
        "       source = vec3(lightPosition) - eyeCoordinates.xyz;" \
        "       viewVector = -eyeCoordinates.xyz;" \
        "   }" \
        "\n" \
        "   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;" \
        "}";

    glShaderSource(vertexShaderObjectPerFragment, 1, (const char**)&vertexShaderCode, NULL);
    glCompileShader(vertexShaderObjectPerFragment);

    GLint infoLogLength = 0;
    GLint shaderCompileStatus = 0;
    char *infoLog = NULL;

    glGetShaderiv(vertexShaderObjectPerFragment, GL_COMPILE_STATUS, &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObjectPerFragment, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetShaderInfoLog(vertexShaderObjectPerFragment, infoLogLength, &written, infoLog);
                fprintf(logFile, "CG: Vertex shader compilation log: %s\n", infoLog);
                free(infoLog);
                cleanUp();
                exit(EXIT_FAILURE);
            }
        }
    }
}

void initializeFragmentShaderPerFragment()
{
    fragmentShaderObjectPerFragment = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar *fragmentShaderCode = "#version 450 core" \
        "\n" \
		"in vec3 tNormal;" \
		"in vec3 source;" \
		"in vec3 viewVector;" \
        "\n" \
        "out vec4 fragmentColor;" \
        "\n" \
        "uniform int isLightingEnabled;" \
        "uniform vec3 la;" \
        "uniform vec3 ld;" \
        "uniform vec3 ls;" \
        "uniform vec3 ka;" \
        "uniform vec3 kd;" \
        "uniform vec3 ks;" \
        "uniform float materialShininess;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   vec3 phongAdsColor;" \
        "   if(isLightingEnabled == 1)" \
        "   {" \
        "       vec3 normalizedTNormal = normalize(tNormal);" \
        "       vec3 normalizedSource = normalize(source);" \
        "       vec3 normalizedViewVector = normalize(viewVector);" \
        "       float tNormalDotLightDirection = max(dot(normalizedTNormal, normalizedSource), 0.0);" \
        "       vec3 ambient = la * ka;" \
        "       vec3 diffuse = ld * kd * tNormalDotLightDirection;" \
        "       vec3 reflectionVector = reflect(-normalizedSource, normalizedTNormal);" \
        "       vec3 specular = ls * ks * pow(max(dot(reflectionVector, normalizedViewVector), 0.0), materialShininess);" \
        "       phongAdsColor = ambient + diffuse + specular;"
        "   }" \
        "   else" \
        "   {" \
        "       phongAdsColor = vec3(1.0, 1.0, 1.0);" \
        "   }" \
        "\n" \
        "   fragmentColor = vec4(phongAdsColor, 1.0);" \
        "}";

    glShaderSource(fragmentShaderObjectPerFragment, 1, (const char**)&fragmentShaderCode, NULL);
    glCompileShader(fragmentShaderObjectPerFragment);

    GLint infoLogLength = 0;
    GLint shaderCompileStatus = 0;
    char *infoLog = NULL;

    glGetShaderiv(fragmentShaderObjectPerFragment, GL_COMPILE_STATUS, &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObjectPerFragment, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetShaderInfoLog(fragmentShaderObjectPerFragment, infoLogLength, &written, infoLog);
                fprintf(logFile, "CG: Fragment shader compilation log: %s\n", infoLog);
                free(infoLog);
                cleanUp();
                exit(EXIT_FAILURE);
            }
        }
    }
}

void initializeShaderProgramPerFragment()
{
    shaderProgramObjectPerFragment = glCreateProgram();
    glAttachShader(shaderProgramObjectPerFragment, vertexShaderObjectPerFragment);
    glAttachShader(shaderProgramObjectPerFragment, fragmentShaderObjectPerFragment);

    // Bind the position attribute location before linking.
    glBindAttribLocation(shaderProgramObjectPerFragment, CG_ATTRIBUTE_VERTEX_POSITION, "vertexPosition");

    // Bind the normal attribute location before linking.
    glBindAttribLocation(shaderProgramObjectPerFragment, CG_ATTRIBUTE_NORMAL, "vertexNormal");

    // Now link and check for error.
    glLinkProgram(shaderProgramObjectPerFragment);

    GLint infoLogLength = 0;
    GLint shaderProgramLinkStatus = 0;
    char *infoLog = NULL;

    glGetProgramiv(shaderProgramObjectPerFragment, GL_LINK_STATUS, &shaderProgramLinkStatus);

    if(shaderProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObjectPerFragment, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetProgramInfoLog(shaderProgramObjectPerFragment, infoLogLength, &written, infoLog);
                fprintf(logFile, "CG: Shader program link log: %s\n", infoLog);
                free(infoLog);
                cleanUp();
                exit(EXIT_FAILURE);
            }
        }
    }

    // After linking get the value of modelView and projection uniform location from the shader program.
    modelMatrixUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "modelMatrix");
    viewMatrixUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "viewMatrix");
    projectionMatrixUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "projectionMatrix");

    isLightingEnabledUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "isLightingEnabled");

    laUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "la");
    ldUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "ld");
    lsUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "ls");
    kaUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "ka");
    kdUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "kd");
    ksUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "ks");
    lightPositionUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "lightPosition");
    materialShininessUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "materialShininess");
}

void initializeSphereBuffersPerFragment()
{
    glGenVertexArrays(1, &vaoSpherePerFragment);
    glBindVertexArray(vaoSpherePerFragment);

    glGenBuffers(1, &vboSpherePositionPerFragment);
    glBindBuffer(GL_ARRAY_BUFFER, vboSpherePositionPerFragment);

    glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertices), sphereVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_VERTEX_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboSphereNormalPerFragment);
    glBindBuffer(GL_ARRAY_BUFFER, vboSphereNormalPerFragment);

    glBufferData(GL_ARRAY_BUFFER, sizeof(sphereNormals), sphereNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_NORMAL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboSphereElementPerFragment);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSphereElementPerFragment);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphereElements), sphereElements, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if(isPerFragmentLightEnabled)
    {
        usePerFragmentLight();
    }
    else
    {
        usePerVertexLight();
    }

    SwapBuffers(hdc);
}

void usePerVertexLight()
{
    glUseProgram(shaderProgramObjectPerVertex);

    if(isLightingEnabled)
    {
        glUniform1i(isLightingEnabledUniformPerVertex, 1);
        glUniform3fv(laUniformPerVertex, 1, lightAmbient);
        glUniform3fv(ldUniformPerVertex, 1, lightDiffuse);
        glUniform3fv(lsUniformPerVertex, 1, lightSpecular);
        glUniform4fv(lightPositionUniformPerVertex, 1, lightPosition);

        glUniform3fv(kaUniformPerVertex, 1, materialAmbient);
        glUniform3fv(kdUniformPerVertex, 1, materialDiffuse);
        glUniform3fv(ksUniformPerVertex, 1, materialSpecular);
        glUniform1f(materialShininessUniformPerVertex, materialShininess);
    }
    else
    {
        glUniform1i(isLightingEnabledUniformPerVertex, 0);
    }

    drawSpherePerVertex();

    glUseProgram(0);
}

void usePerFragmentLight()
{
    glUseProgram(shaderProgramObjectPerFragment);

    if(isLightingEnabled)
    {
        glUniform1i(isLightingEnabledUniformPerFragment, 1);
        glUniform3fv(laUniformPerFragment, 1, lightAmbient);
        glUniform3fv(ldUniformPerFragment, 1, lightDiffuse);
        glUniform3fv(lsUniformPerFragment, 1, lightSpecular);
        glUniform4fv(lightPositionUniformPerFragment, 1, lightPosition);

        glUniform3fv(kaUniformPerFragment, 1, materialAmbient);
        glUniform3fv(kdUniformPerFragment, 1, materialDiffuse);
        glUniform3fv(ksUniformPerFragment, 1, materialSpecular);
        glUniform1f(materialShininessUniformPerFragment, materialShininess);
    }
    else
    {
        glUniform1i(isLightingEnabledUniformPerFragment, 0);
    }

    drawSpherePerFragment();

    glUseProgram(0);
}

void drawSpherePerVertex()
{
    vmath::mat4 modelMatrix = vmath::mat4::identity();
    vmath::mat4 viewMatrix = vmath::mat4::identity();

    // Translate the modal view matrix.
    modelMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

    // Pass modelMatrix to vertex shader in 'modelMatrix' variable defined in shader.
    glUniformMatrix4fv(modelMatrixUniformPerVertex, 1, GL_FALSE, modelMatrix);

    // Pass viewMatrix to vertex shader in 'viewMatrix' variable defined in shader.
    glUniformMatrix4fv(viewMatrixUniformPerVertex, 1, GL_FALSE, viewMatrix);

    // Pass perspectiveProjectionMatrix to vertex shader in 'projectionMatrix' variable defined in shader.
    glUniformMatrix4fv(projectionMatrixUniformPerVertex, 1, GL_FALSE, perspectiveProjectionMatrix);

    // Now bind the VAO to which we want to use
    glBindVertexArray(vaoSpherePerVertex);

    // Draw the sphere
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSphereElementPerVertex);
    glDrawElements(GL_TRIANGLES, numberOfSphereElements, GL_UNSIGNED_SHORT, 0);

    // unbind the vao
    glBindVertexArray(0);
}

void drawSpherePerFragment()
{
    vmath::mat4 modelMatrix = vmath::mat4::identity();
    vmath::mat4 viewMatrix = vmath::mat4::identity();

    // Translate the modal view matrix.
    modelMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

    // Pass modelMatrix to vertex shader in 'modelMatrix' variable defined in shader.
    glUniformMatrix4fv(modelMatrixUniformPerFragment, 1, GL_FALSE, modelMatrix);

    // Pass viewMatrix to vertex shader in 'viewMatrix' variable defined in shader.
    glUniformMatrix4fv(viewMatrixUniformPerFragment, 1, GL_FALSE, viewMatrix);

    // Pass perspectiveProjectionMatrix to vertex shader in 'projectionMatrix' variable defined in shader.
    glUniformMatrix4fv(projectionMatrixUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);

    // Now bind the VAO to which we want to use
    glBindVertexArray(vaoSpherePerFragment);

    // Draw the sphere
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSphereElementPerFragment);
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

    if(vaoSpherePerVertex)
    {
        glDeleteVertexArrays(1, &vaoSpherePerVertex);
        vaoSpherePerVertex = 0;
    }

    if(vboSpherePositionPerVertex)
    {
        glDeleteBuffers(1, &vboSpherePositionPerVertex);
        vboSpherePositionPerVertex = 0;
    }

    if(vboSphereNormalPerVertex)
    {
        glDeleteBuffers(1, &vboSphereNormalPerVertex);
        vboSphereNormalPerVertex = 0;
    }

    if(shaderProgramObjectPerVertex)
    {
        if(vertexShaderObjectPerVertex)
        {
            glDetachShader(shaderProgramObjectPerVertex, vertexShaderObjectPerVertex);
        }

        if(fragmentShaderObjectPerVertex)
        {
            glDetachShader(shaderProgramObjectPerVertex, fragmentShaderObjectPerVertex);
        }
    }

    if(vertexShaderObjectPerVertex)
    {
        glDeleteShader(vertexShaderObjectPerVertex);
        vertexShaderObjectPerVertex = 0;
    }

    if(fragmentShaderObjectPerVertex)
    {
        glDeleteShader(fragmentShaderObjectPerVertex);
        fragmentShaderObjectPerVertex = 0;
    }

    if(shaderProgramObjectPerVertex)
    {
        glDeleteProgram(shaderProgramObjectPerVertex);
        shaderProgramObjectPerVertex = 0;
    }

    if(vaoSpherePerFragment)
    {
        glDeleteVertexArrays(1, &vaoSpherePerFragment);
        vaoSpherePerFragment = 0;
    }

    if(vboSpherePositionPerFragment)
    {
        glDeleteBuffers(1, &vboSpherePositionPerFragment);
        vboSpherePositionPerFragment = 0;
    }

    if(vboSphereNormalPerFragment)
    {
        glDeleteBuffers(1, &vboSphereNormalPerFragment);
        vboSphereNormalPerFragment = 0;
    }

    if(shaderProgramObjectPerFragment)
    {
        if(vertexShaderObjectPerFragment)
        {
            glDetachShader(shaderProgramObjectPerFragment, vertexShaderObjectPerFragment);
        }

        if(fragmentShaderObjectPerFragment)
        {
            glDetachShader(shaderProgramObjectPerFragment, fragmentShaderObjectPerFragment);
        }
    }

    if(vertexShaderObjectPerFragment)
    {
        glDeleteShader(vertexShaderObjectPerFragment);
        vertexShaderObjectPerFragment = 0;
    }

    if(fragmentShaderObjectPerFragment)
    {
        glDeleteShader(fragmentShaderObjectPerFragment);
        fragmentShaderObjectPerFragment = 0;
    }

    if(shaderProgramObjectPerFragment)
    {
        glDeleteProgram(shaderProgramObjectPerFragment);
        shaderProgramObjectPerFragment = 0;
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
