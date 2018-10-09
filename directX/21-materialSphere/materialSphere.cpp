#include <windows.h>
#include <stdio.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <Sphere.h>

#include "resources/resource.h"

#pragma warning(disable: 4838)
#include "lib/xnaMath/xnamath.h"

enum
{
    CG_INPUT_SLOT_VERTEX_POSITION = 0,
	CG_INPUT_SLOT_COLOR,
	CG_INPUT_SLOT_NORMAL,
	CG_INPUT_SLOT_TEXTURE,
};

struct CBuffer
{
    XMMATRIX worldMatrix;
    XMMATRIX viewMatrix;
    XMMATRIX projectionMatrix;
    XMVECTOR laZero;
    XMVECTOR ldZero;
    XMVECTOR lsZero;
    XMVECTOR ka;
    XMVECTOR kd;
    XMVECTOR ks;
    XMVECTOR lightZeroPosition;
    float materialShininess;
    unsigned int isLightingEnabled;
};

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
bool isAnimationEnabled = false;
bool rotateLightOnXAxis = true;
bool rotateLightOnYAxis = false;
bool rotateLightOnZAxis = false;

unsigned short sphereElements[2280];

unsigned int numberOfSphereVertices = 0;
unsigned int numberOfSphereElements = 0;

const int numberOfRows = 6;
const int numberOfColumns = 4;

int viewportWidth = 1;
int viewportHeight = 1;
int viewportInitialX = 0;
int viewportInitialY = 0;

float angleLightRotation = 0.0f;
float speed = 0.001f;

float sphereVertices[1146];
float sphereNormals[1146];
float sphereTextures[764];

float clearColor[4];
float lightZeroAmbient[] = {0.5f, 0.5f, 0.5f, 1.0f};
float lightZeroDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
float lightZeroSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
float lightZeroPosition[] = {0.0f, 0.0f, 1.0f, 0.0f};

float materialAmbient[numberOfColumns][numberOfRows][4] = {
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

float materialDiffuse[numberOfColumns][numberOfRows][4] = {
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

float materialSpecular[numberOfColumns][numberOfRows][4] = {
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

float materialShininess[numberOfColumns][numberOfRows] = {
    {   // Column 1
        0.6f * 128.0f,
        0.1f * 128.0f,
        0.3f * 128.0f,
        0.088f * 128.0f,
        0.6f * 128.0f,
        0.1f * 128.0f
    },
    {   // Column 2
        0.21794872f * 128.0f,
        0.2f * 128.0f,
        0.6f * 128.0f,
        0.1f * 128.0f,
        0.4f * 128.0f,
        0.4f * 128.0f
    },
    {   // Column 3
        0.25f * 128.0f,
        0.25f * 128.0f,
        0.25f * 128.0f,
        0.25f * 128.0f,
        0.25f * 128.0f,
        0.25f * 128.0f
    },
    {   // Column 4
        0.078125f * 128.0f,
        0.078125f * 128.0f,
        0.078125f * 128.0f,
        0.078125f * 128.0f,
        0.078125f * 128.0f,
        0.078125f * 128.0f
    }
};

ID3D11Device *device = NULL;
ID3D11DeviceContext *deviceContext = NULL;
ID3D11RenderTargetView *renderTargetView = NULL;
IDXGISwapChain *swapChain = NULL;
ID3D11RasterizerState *rasterizerState = NULL;
ID3D11DepthStencilView *depthStencilView = NULL;
ID3D11VertexShader *vertexShaderObject = NULL;
ID3D11PixelShader *pixelShaderObject = NULL;
ID3D11InputLayout *inputLayout = NULL;
ID3D11Buffer *vertexBufferSpherePosition = NULL;
ID3D11Buffer *vertexBufferSphereNormals = NULL;
ID3D11Buffer *vertexBufferSphereElements = NULL;
ID3D11Buffer *constantBuffer = NULL;

XMMATRIX perspectiveProjectionMatrix;

FILE *logFile = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

HRESULT initialize(void);
HRESULT initializeSwapChain(void);
HRESULT initializeVertexShader(ID3DBlob **vertexShaderCode);
HRESULT initializePixelShader(ID3DBlob **pixelShaderCode);
HRESULT initializeInputLayout(ID3DBlob *vertexShaderCode, ID3D11InputLayout **inputLayout);
HRESULT initializeSphereBuffers(void);
HRESULT initializeConstantBuffers(void);
HRESULT disableBackFaceCulling(void);
void update(void);
void display(void);
void drawSphere(int columnCounter, int rowCounter);
HRESULT resize(int width, int height);
void toggleFullscreen(HWND hWnd, bool isFullscreen);
void cleanUp(void);
void log(const char* message, ...);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsatnce, LPSTR lpszCmdLine, int nCmdShow)
{
    WNDCLASSEX wndClassEx;
    MSG message;
    TCHAR szApplicationTitle[] = TEXT("CG - 24 Material Sphere");
    TCHAR szApplicationClassName[] = TEXT("RTR_MATERIAL_SPHERE");
    bool done = false;

	if (fopen_s(&logFile, "debug.log", "w") != 0)
	{
		MessageBox(NULL, TEXT("Unable to open log file."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}

    log("---------- CG: DirectX Debug Logs Start ----------\n");

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

    HRESULT result = initialize();

    if(FAILED(result))
    {
        log("[Error] | Initialize failed, error code: %#010x\n", result);
        cleanUp();
        exit(EXIT_FAILURE);
    }

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
    HRESULT result = S_OK;

    switch(iMessage)
    {
        case WM_ACTIVATE:
            isActive = (HIWORD(wParam) == 0);
        break;

        case WM_ERASEBKGND:
		return 0;

        case WM_SIZE:
            GetClientRect(hWnd, &windowRect);

            if(deviceContext != NULL)
            {
                result = resize(LOWORD(lParam), HIWORD(lParam));

                if(FAILED(result))
                {
                    log("[Error] | Resize failed, error code: %#010x\n", result);
                    return result;
                }
            }

        break;

        case WM_KEYDOWN:
            switch(wParam)
            {
                case VK_ESCAPE:
                    isEscapeKeyPressed = true;
                break;

                default:
                break;
            }

            if(wParam > '0' && wParam <= '9')
            {
                speed = 0.01f * (wParam - '0');
            }
            else if(wParam > VK_NUMPAD0 && wParam <= VK_NUMPAD9)
            {
                speed = 0.01f * (wParam - VK_NUMPAD0);
            }

        break;

        case WM_CHAR:
            switch(wParam)
            {
                case 'A':
                case 'a':
                    isAnimationEnabled = !isAnimationEnabled;
                break;

                case 'F':
                case 'f':
                    isFullscreen = !isFullscreen;
                    toggleFullscreen(hWnd, isFullscreen);
                break;

                case 'L':
                case 'l':
                    isLightingEnabled = !isLightingEnabled;
                break;

                case 'X':
                case 'x':
                    rotateLightOnXAxis = true;
                    rotateLightOnYAxis = false;
                    rotateLightOnZAxis = false;
                break;

                case 'Y':
                case 'y':
                    rotateLightOnXAxis = false;
                    rotateLightOnYAxis = true;
                    rotateLightOnZAxis = false;
                break;

                case 'Z':
                case 'z':
                    rotateLightOnXAxis = false;
                    rotateLightOnYAxis = false;
                    rotateLightOnZAxis = true;
                break;

                default:
                break;
            }

        break;

        case WM_LBUTTONDOWN:
        break;

        case WM_CLOSE:
            cleanUp();
        break;

        case WM_DESTROY:
            PostQuitMessage(0);
        break;

        default:
        break;
    }

    return DefWindowProc(hWnd, iMessage, wParam, lParam);
}

HRESULT initialize(void)
{
    HRESULT result = S_OK;
    ID3DBlob *vertexShaderCode = NULL;
    ID3DBlob *pixelShaderCode = NULL;

    result = initializeSwapChain();

    if(FAILED(result))
    {
        return result;
    }

    result = initializeVertexShader(&vertexShaderCode);

    if(FAILED(result))
    {
        if(vertexShaderCode != NULL)
        {
            vertexShaderCode->Release();
            vertexShaderCode = NULL;
        }

        return result;
    }

    result = initializePixelShader(&pixelShaderCode);

    if(FAILED(result))
    {
        if(vertexShaderCode != NULL)
        {
            vertexShaderCode->Release();
            vertexShaderCode = NULL;
        }

        if(pixelShaderCode != NULL)
        {
            pixelShaderCode->Release();
            pixelShaderCode = NULL;
        }

        return result;
    }

    result = initializeInputLayout(vertexShaderCode, &inputLayout);

    // Now we do not need vertexShaderCode and pixelShaderCode, so release them.
   if(vertexShaderCode != NULL)
    {
        vertexShaderCode->Release();
        vertexShaderCode = NULL;
    }

    if(pixelShaderCode != NULL)
    {
        pixelShaderCode->Release();
        pixelShaderCode = NULL;
    }

    if(FAILED(result))
    {
        return result;
    }

    result = initializeConstantBuffers();

    if(FAILED(result))
    {
        return result;
    }

    result = initializeSphereBuffers();

    if(FAILED(result))
    {
        return result;
    }

    result = disableBackFaceCulling();

    if(FAILED(result))
    {
        return result;
    }

    clearColor[0] = 0.25f;
    clearColor[1] = 0.25f;
    clearColor[2] = 0.25f;
    clearColor[3] = 0.25f;

    perspectiveProjectionMatrix = XMMatrixIdentity();

    isAnimationEnabled = false;
    isLightingEnabled = false;
    rotateLightOnXAxis = true;
    rotateLightOnYAxis = false;
    rotateLightOnZAxis = false;

    result = resize(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);

    if(FAILED(result))
    {
        log("[Error] | Initial resize failed, error code: %#010x\n", result);
        return result;
    }

    return S_OK;
}

HRESULT initializeSwapChain(void)
{
    HRESULT result = S_OK;

    D3D_DRIVER_TYPE driverType;
    D3D_DRIVER_TYPE requiredDriverTypes[] = {D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE};
    D3D_FEATURE_LEVEL requiredFeatureLevel = D3D_FEATURE_LEVEL_11_0;
    D3D_FEATURE_LEVEL acquiredFeatureLevel = D3D_FEATURE_LEVEL_10_0;
    UINT deviceCreationFlags = 0;
    UINT numberOfDriverTypes = sizeof(requiredDriverTypes) / sizeof(requiredDriverTypes[0]);
    UINT numberOfFeatureLevels = 1;

    DXGI_SWAP_CHAIN_DESC swapChainDescriptor = {};
    ZeroMemory((void *)&swapChainDescriptor, sizeof(DXGI_SWAP_CHAIN_DESC));

    swapChainDescriptor.BufferCount = 1;
    swapChainDescriptor.BufferDesc.Width = windowRect.right - windowRect.left;
    swapChainDescriptor.BufferDesc.Height = windowRect.bottom - windowRect.top;
    swapChainDescriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDescriptor.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDescriptor.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescriptor.OutputWindow = hWnd;
    swapChainDescriptor.Windowed = TRUE;

    // Try between 1 to 4
    swapChainDescriptor.SampleDesc.Count = 1;

    // As we have SampleDesc.Count = 4. If its 1, then use 0.
    swapChainDescriptor.SampleDesc.Quality = 0;

    for(UINT driverTypeCounter = 0; driverTypeCounter < numberOfDriverTypes; ++driverTypeCounter)
    {
        driverType = requiredDriverTypes[driverTypeCounter];
        result = D3D11CreateDeviceAndSwapChain(
            NULL,
            driverType,
            NULL,
            deviceCreationFlags,
            &requiredFeatureLevel,
            numberOfFeatureLevels,
            D3D11_SDK_VERSION,
            &swapChainDescriptor,
            &swapChain,
            &device,
            &acquiredFeatureLevel,
            &deviceContext
        );

        if(SUCCEEDED(result))
        {
            break;
        }
    }

    if(FAILED(result))
    {
        log("[Error] | Could not created swap chain, device and context, error code: %#010x\n", result);
        return result;
    }

    log("[Info] | Swap chain, device and context created.\n");

    if(driverType == D3D_DRIVER_TYPE_HARDWARE)
    {
        log("[Info] | Selected driver type is: D3D_DRIVER_TYPE_HARDWARE (%d)\n", D3D_DRIVER_TYPE_HARDWARE);
    }
    else if(driverType == D3D_DRIVER_TYPE_WARP)
    {
        log("[Info] | Selected driver type is: D3D_DRIVER_TYPE_WARP (%d)\n", D3D_DRIVER_TYPE_WARP);
    }
    else if(driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        log("[Info] | Selected driver type is: D3D_DRIVER_TYPE_REFERENCE (%d)\n", D3D_DRIVER_TYPE_REFERENCE);
    }
    else
    {
        log("[Info] | Selected driver type is: Unknown (%d)\n", driverType);
    }

    if(acquiredFeatureLevel == D3D_FEATURE_LEVEL_11_0)
    {
        log("[Info] | Selected feature level is: D3D_FEATURE_LEVEL_11_0 (%#010x)\n", D3D_FEATURE_LEVEL_11_0);
    }
    else if(acquiredFeatureLevel == D3D_FEATURE_LEVEL_10_1)
    {
        log("[Info] | Selected feature level is: D3D_FEATURE_LEVEL_10_1 (%#010x)\n", D3D_FEATURE_LEVEL_10_1);
    }
    else if(acquiredFeatureLevel == D3D_FEATURE_LEVEL_10_0)
    {
        log("[Info] | Selected feature level is: D3D_FEATURE_LEVEL_10_0 (%#010x)\n", D3D_FEATURE_LEVEL_10_0);
    }
    else
    {
        log("[Info] | Selected feature level is: Unknown (%d)\n", acquiredFeatureLevel);
    }

    return S_OK;
}

HRESULT initializeVertexShader(ID3DBlob **vertexShaderCode)
{
    ID3DBlob *error = NULL;
    HRESULT result = D3DCompileFromFile(L"./shaders/vertexShader.hlsl",
        NULL,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "vs_5_0",
        0,
        0,
        vertexShaderCode,
        &error
    );

    if(FAILED(result))
    {
        if(error != NULL)
        {
            log("[Error] | Failed to compile vertex shader, error message: %s\n", (char *)error->GetBufferPointer());
            error->Release();
            error = NULL;
        }
        else
        {
            log("[Error] | Failed to compile vertex shader, error code: %#010x\n", result);
        }

        return result;
    }

    result = device->CreateVertexShader((*vertexShaderCode)->GetBufferPointer(),
        (*vertexShaderCode)->GetBufferSize(),
        NULL,
        &vertexShaderObject
    );

    if(FAILED(result))
    {
        log("[Error] | Failed to create vertex shader, error code: %#010x\n", result);
        return result;
    }

    deviceContext->VSSetShader(vertexShaderObject, 0, 0);

    return S_OK;
}

HRESULT initializePixelShader(ID3DBlob **pixelShaderCode)
{
    ID3DBlob *error = NULL;
    HRESULT result = D3DCompileFromFile(L"./shaders/pixelShader.hlsl",
        NULL,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "ps_5_0",
        0,
        0,
        pixelShaderCode,
        &error
    );

    if(FAILED(result))
    {
        if(error != NULL)
        {
            log("[Error] | Failed to compile pixel shader, error message: %s\n", (char *)error->GetBufferPointer());
            error->Release();
            error = NULL;
        }
        else
        {
            log("[Error] | Failed to compile pixel shader, error code: %#010x\n", result);
        }

        return result;
    }

    result = device->CreatePixelShader((*pixelShaderCode)->GetBufferPointer(),
        (*pixelShaderCode)->GetBufferSize(),
        NULL,
        &pixelShaderObject
    );

    if(FAILED(result))
    {
        log("[Error] | Failed to create pixel shader, error code: %#010x\n", result);
        return result;
    }

    deviceContext->PSSetShader(pixelShaderObject, 0, 0);

    return S_OK;
}

HRESULT initializeInputLayout(ID3DBlob *vertexShaderCode, ID3D11InputLayout **inputLayout)
{
    D3D11_INPUT_ELEMENT_DESC inputLayoutDescriptor[2];
    ZeroMemory((void *)&inputLayoutDescriptor, sizeof(D3D11_INPUT_ELEMENT_DESC) * 2);

    inputLayoutDescriptor[0].SemanticName = "POSITION";
    inputLayoutDescriptor[0].SemanticIndex = 0;
    inputLayoutDescriptor[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputLayoutDescriptor[0].InputSlot = CG_INPUT_SLOT_VERTEX_POSITION;
    inputLayoutDescriptor[0].AlignedByteOffset = 0;
    inputLayoutDescriptor[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    inputLayoutDescriptor[0].InstanceDataStepRate = 0;

    inputLayoutDescriptor[1].SemanticName = "NORMAL";
    inputLayoutDescriptor[1].SemanticIndex = 0;
    inputLayoutDescriptor[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputLayoutDescriptor[1].InputSlot = CG_INPUT_SLOT_NORMAL;
    inputLayoutDescriptor[1].AlignedByteOffset = 0;
    inputLayoutDescriptor[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    inputLayoutDescriptor[1].InstanceDataStepRate = 0;

    HRESULT result = device->CreateInputLayout(inputLayoutDescriptor,
        2,
        vertexShaderCode->GetBufferPointer(),
        vertexShaderCode->GetBufferSize(),
        inputLayout
    );

    if(FAILED(result))
    {
        log("[Error] | Failed to create input layout, error code: %#010x\n", result);
        return result;
    }

    deviceContext->IASetInputLayout(*inputLayout);

    return S_OK;
}

HRESULT initializeConstantBuffers(void)
{
    D3D11_BUFFER_DESC constantBufferDescriptor = {};
    ZeroMemory((void *)&constantBufferDescriptor, sizeof(D3D11_BUFFER_DESC));

    constantBufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    constantBufferDescriptor.ByteWidth = sizeof(CBuffer);
    constantBufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    HRESULT result = device->CreateBuffer(&constantBufferDescriptor, nullptr, &constantBuffer);

    if(FAILED(result))
    {
        log("[Error] | Cannot create constant buffer, error code: %#010x\n", result);
        return result;
    }

    deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
    deviceContext->PSSetConstantBuffers(0, 1, &constantBuffer);

    return S_OK;
}

HRESULT disableBackFaceCulling(void)
{
    D3D11_RASTERIZER_DESC rasterizerDescriptor = {};
    ZeroMemory((void *)&rasterizerDescriptor, sizeof(D3D11_RASTERIZER_DESC));

    rasterizerDescriptor.AntialiasedLineEnable = FALSE;
    rasterizerDescriptor.MultisampleEnable = FALSE;
    rasterizerDescriptor.DepthBias = 0;
    rasterizerDescriptor.DepthBiasClamp = 0.0f;
    rasterizerDescriptor.SlopeScaledDepthBias = 0.0f;
    rasterizerDescriptor.CullMode = D3D11_CULL_NONE;
    rasterizerDescriptor.DepthClipEnable = TRUE;
    rasterizerDescriptor.FillMode = D3D11_FILL_SOLID;
    rasterizerDescriptor.FrontCounterClockwise = FALSE;
    rasterizerDescriptor.ScissorEnable = FALSE;

    HRESULT result = device->CreateRasterizerState(&rasterizerDescriptor, &rasterizerState);

    if(FAILED(result))
    {
        log("[Error] | Cannot get rasterizer state, error code: %#010x\n", result);
        return result;
    }

    deviceContext->RSSetState(rasterizerState);

    return S_OK;
}

HRESULT initializeSphereBuffers(void)
{
    getSphereVertexData(sphereVertices, sphereNormals, sphereTextures, sphereElements);
    numberOfSphereVertices = getNumberOfSphereVertices();
    numberOfSphereElements = getNumberOfSphereElements();

    // Position
    D3D11_BUFFER_DESC vertexBufferDescriptor = {};
    ZeroMemory((void *)&vertexBufferDescriptor, sizeof(D3D11_BUFFER_DESC));

    vertexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDescriptor.ByteWidth = sizeof(float) * ARRAYSIZE(sphereVertices);
    vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT result = device->CreateBuffer(&vertexBufferDescriptor, nullptr, &vertexBufferSpherePosition);

    if(FAILED(result))
    {
        log("[Error] | Cannot create vertex buffer for position, error code: %#010x\n", result);
        return result;
    }

    D3D11_MAPPED_SUBRESOURCE mappedSubresource = {};
    ZeroMemory((void *)&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    deviceContext->Map(vertexBufferSpherePosition, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
    memcpy(mappedSubresource.pData, sphereVertices, sizeof(sphereVertices));
    deviceContext->Unmap(vertexBufferSpherePosition, NULL);

    // Normals
    ZeroMemory((void *)&vertexBufferDescriptor, sizeof(D3D11_BUFFER_DESC));

    vertexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDescriptor.ByteWidth = sizeof(float) * ARRAYSIZE(sphereNormals);
    vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    result = device->CreateBuffer(&vertexBufferDescriptor, nullptr, &vertexBufferSphereNormals);

    if(FAILED(result))
    {
        log("[Error] | Cannot create vertex buffer for normals, error code: %#010x\n", result);
        return result;
    }

    ZeroMemory((void *)&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    deviceContext->Map(vertexBufferSphereNormals, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
    memcpy(mappedSubresource.pData, sphereNormals, sizeof(sphereNormals));
    deviceContext->Unmap(vertexBufferSphereNormals, NULL);

    // Elements
    ZeroMemory((void *)&vertexBufferDescriptor, sizeof(D3D11_BUFFER_DESC));

    vertexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDescriptor.ByteWidth = sizeof(short) * numberOfSphereElements;
    vertexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
    vertexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    result = device->CreateBuffer(&vertexBufferDescriptor, nullptr, &vertexBufferSphereElements);

    if(FAILED(result))
    {
        log("[Error] | Cannot create vertex buffer for elements, error code: %#010x\n", result);
        return result;
    }

    ZeroMemory((void *)&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    deviceContext->Map(vertexBufferSphereElements, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
    memcpy(mappedSubresource.pData, sphereElements, sizeof(short) * numberOfSphereElements);
    deviceContext->Unmap(vertexBufferSphereElements, NULL);

    return S_OK;
}

void update(void)
{
    if(!isAnimationEnabled)
    {
        return;
    }

    angleLightRotation += speed;

    if(angleLightRotation >= 360.0f)
    {
        angleLightRotation = 0.0f;
    }
}

void display(void)
{
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    D3D11_VIEWPORT viewPort = {};
    ZeroMemory((void *)&viewPort, sizeof(D3D11_VIEWPORT));

    viewPort.Width = (float)viewportWidth;
    viewPort.Height = (float)viewportHeight;
    viewPort.MinDepth = 0.0f;
    viewPort.MaxDepth = 1.0f;

    for(int columnCounter = 0; columnCounter < numberOfColumns; ++columnCounter)
    {
        for(int rowCounter = 0; rowCounter < numberOfRows; ++rowCounter)
        {
            viewPort.TopLeftX = viewportInitialX + (columnCounter * viewportWidth);
            viewPort.TopLeftY = viewportInitialY + (rowCounter * viewportHeight);

            deviceContext->RSSetViewports(1, &viewPort);

            drawSphere(columnCounter, rowCounter);
        }
    }

    swapChain->Present(0, 0);
}

void drawSphere(int columnCounter, int rowCounter)
{
    // stride and offset is same for position and color as they both have 3 values, i.e xyz.
    UINT stride = sizeof(float) * 3;
    UINT offset = 0;

    deviceContext->IASetVertexBuffers(CG_INPUT_SLOT_VERTEX_POSITION, 1, &vertexBufferSpherePosition, &stride, &offset);
    deviceContext->IASetVertexBuffers(CG_INPUT_SLOT_NORMAL, 1, &vertexBufferSphereNormals, &stride, &offset);
    deviceContext->IASetIndexBuffer(vertexBufferSphereElements, DXGI_FORMAT_R16_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    XMMATRIX worldMatrix = XMMatrixIdentity();
    XMMATRIX viewMatrix = XMMatrixIdentity();
    XMMATRIX translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 3.0f);

    worldMatrix = translationMatrix;

    if(rotateLightOnXAxis)
    {
        lightZeroPosition[0] = 0.0f;
        lightZeroPosition[1] = sinf(angleLightRotation) * 100.0f + 3.0f;
        lightZeroPosition[2] = cosf(angleLightRotation) * 100.0f + 3.0f;
    }
    else if(rotateLightOnYAxis)
    {
        lightZeroPosition[0] = sinf(angleLightRotation) * 100.0f + 3.0f;
        lightZeroPosition[1] = 0.0f;
        lightZeroPosition[2] = cosf(angleLightRotation) * 100.0f + 3.0f;
    }
    else if(rotateLightOnZAxis)
    {
        lightZeroPosition[0] = sinf(angleLightRotation) * 100.0f + 3.0f;
        lightZeroPosition[1] = cosf(angleLightRotation) * 100.0f + 3.0f;
        lightZeroPosition[2] = 0.0f;
    }

    CBuffer cBuffer;
    ZeroMemory((void *)&cBuffer, sizeof(CBuffer));

    cBuffer.worldMatrix = worldMatrix;
    cBuffer.viewMatrix = viewMatrix;
    cBuffer.projectionMatrix = perspectiveProjectionMatrix;
    cBuffer.laZero = XMVectorSet(lightZeroAmbient[0], lightZeroAmbient[1], lightZeroAmbient[2], lightZeroAmbient[3]);
    cBuffer.ldZero = XMVectorSet(lightZeroDiffuse[0], lightZeroDiffuse[1], lightZeroDiffuse[2], lightZeroDiffuse[3]);
    cBuffer.lsZero = XMVectorSet(lightZeroSpecular[0], lightZeroSpecular[1], lightZeroSpecular[2], lightZeroSpecular[3]);
    cBuffer.ka = XMVectorSet(
        materialAmbient[columnCounter][rowCounter][0],
        materialAmbient[columnCounter][rowCounter][1],
        materialAmbient[columnCounter][rowCounter][2],
        materialAmbient[columnCounter][rowCounter][3]
    );

    cBuffer.kd = XMVectorSet(
        materialDiffuse[columnCounter][rowCounter][0],
        materialDiffuse[columnCounter][rowCounter][1],
        materialDiffuse[columnCounter][rowCounter][2],
        materialDiffuse[columnCounter][rowCounter][3]
    );

    cBuffer.ks = XMVectorSet(
        materialSpecular[columnCounter][rowCounter][0],
        materialSpecular[columnCounter][rowCounter][1],
        materialSpecular[columnCounter][rowCounter][2],
        materialSpecular[columnCounter][rowCounter][3]
    );

    cBuffer.lightZeroPosition = XMVectorSet(lightZeroPosition[0], lightZeroPosition[1], lightZeroPosition[2], lightZeroPosition[3]);
    cBuffer.materialShininess = materialShininess[columnCounter][rowCounter];
    cBuffer.isLightingEnabled = isLightingEnabled ? 1 : 0;

    deviceContext->UpdateSubresource(constantBuffer, 0, NULL, &cBuffer, 0, 0);
    deviceContext->DrawIndexed(numberOfSphereElements, 0, 0);
}

HRESULT resize(int width, int height)
{
    HRESULT result = S_OK;

    if(depthStencilView != NULL)
    {
        depthStencilView->Release();
        depthStencilView = NULL;
    }

    D3D11_TEXTURE2D_DESC textureDescriptor = {};
    ZeroMemory((void *)&textureDescriptor, sizeof(D3D11_TEXTURE2D_DESC));

    textureDescriptor.Width = width;
    textureDescriptor.Height = height;

    // As we are using it for depth, so there is no texture data, hence using 1.
    textureDescriptor.ArraySize = 1;

    // As we are using it for depth, hence using 1.
    textureDescriptor.MipLevels = 1;

    // Try between 1 to 4
    textureDescriptor.SampleDesc.Count = 1;

    // As we have SampleDesc.Count = 4. If its 1, then use 0.
    textureDescriptor.SampleDesc.Quality = 0;

    textureDescriptor.Format = DXGI_FORMAT_D32_FLOAT;
    textureDescriptor.Usage = D3D11_USAGE_DEFAULT;
    textureDescriptor.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    textureDescriptor.CPUAccessFlags = 0;
    textureDescriptor.MiscFlags = 0;

    ID3D11Texture2D *depthTextureBuffer = NULL;
    result =  device->CreateTexture2D(&textureDescriptor, NULL, &depthTextureBuffer);

    if(FAILED(result))
    {
        log("[Error] | Cannot create texture buffer for depth stencil view, error code: %#010x\n", result);
        return result;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescriptor = {};
    ZeroMemory((void *)&depthStencilViewDescriptor, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

    depthStencilViewDescriptor.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDescriptor.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

    result = device->CreateDepthStencilView(depthTextureBuffer, &depthStencilViewDescriptor, &depthStencilView);

    if(FAILED(result))
    {
        log("[Error] | Cannot create depth stencil view, error code: %#010x\n", result);
        return result;
    }

    if(renderTargetView != NULL)
    {
        renderTargetView->Release();
        renderTargetView = NULL;
    }

    swapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

    ID3D11Texture2D *backBuffer = NULL;
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&backBuffer);

    result = device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);

    if(FAILED(result))
    {
        if(backBuffer != NULL)
        {
            backBuffer->Release();
            backBuffer = NULL;
        }

        log("[Error] | Cannot create render target view, error code: %#010x\n", result);
        return result;
    }

    result = S_OK;

    if(backBuffer != NULL)
    {
        backBuffer->Release();
        backBuffer = NULL;
    }

    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    if(height == 0)
    {
        height = 1;
    }

    D3D11_VIEWPORT viewPort = {};
    ZeroMemory((void *)&viewPort, sizeof(D3D11_VIEWPORT));

    viewPort.TopLeftX = 0;
    viewPort.TopLeftY = 0;
    viewPort.Width = (float)width;
    viewPort.Height = (float)height;
    viewPort.MinDepth = 0.0f;
    viewPort.MaxDepth = 1.0f;

    deviceContext->RSSetViewports(1, &viewPort);

    viewportWidth = width / numberOfRows;
    viewportHeight = height / numberOfRows;
    viewportInitialX = (width - (viewportWidth * numberOfColumns)) / 2;
    viewportInitialY = (height - (viewportHeight * numberOfRows)) / 2;

    perspectiveProjectionMatrix= XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)width/(float)height, 0.1f, 100.0f);

    return S_OK;
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

void log(const char* message, ...)
{
    if(logFile == NULL)
    {
        return;
    }

    va_list args;
    va_start(args, message );
    vfprintf(logFile, message, args );
    va_end( args );

    fflush(logFile);
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

    if(rasterizerState != NULL)
    {
        rasterizerState->Release();
        rasterizerState = NULL;
    }

    if(vertexBufferSpherePosition != NULL)
    {
        vertexBufferSpherePosition->Release();
        vertexBufferSpherePosition = NULL;
    }

    if(vertexBufferSphereNormals != NULL)
    {
        vertexBufferSphereNormals->Release();
        vertexBufferSphereNormals = NULL;
    }

    if(vertexBufferSphereElements != NULL)
    {
        vertexBufferSphereElements->Release();
        vertexBufferSphereElements = NULL;
    }

    if(constantBuffer != NULL)
    {
        constantBuffer->Release();
        constantBuffer = NULL;
    }

    if(inputLayout != NULL)
    {
        inputLayout->Release();
        inputLayout = NULL;
    }

    if(vertexShaderObject != NULL)
    {
        vertexShaderObject->Release();
        vertexShaderObject = NULL;
    }

    if(pixelShaderObject != NULL)
    {
        pixelShaderObject->Release();
        pixelShaderObject = NULL;
    }

    if(depthStencilView != NULL)
    {
        depthStencilView->Release();
        depthStencilView = NULL;
    }

    if(renderTargetView != NULL)
    {
        renderTargetView->Release();
        renderTargetView = NULL;
    }

    if(swapChain != NULL)
    {
        swapChain->Release();
        swapChain = NULL;
    }

    if(deviceContext != NULL)
    {
        deviceContext->Release();
        deviceContext = NULL;
    }

    if(device != NULL)
    {
        device->Release();
        device = NULL;
    }

    DestroyWindow(hWnd);
    hWnd = NULL;

    log("---------- CG: DirectX Debug Logs End ----------\n");
    fclose(logFile);
}
