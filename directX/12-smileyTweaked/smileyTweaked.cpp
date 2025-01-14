#include <windows.h>
#include <stdio.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "resources/resource.h"
#include "lib/directXTK/WICTextureLoader.h"

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
    XMMATRIX worldViewProjectionMatrix;
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

float clearColor[4];
int textureMode = 0;

ID3D11Device *device = NULL;
ID3D11DeviceContext *deviceContext = NULL;
ID3D11RenderTargetView *renderTargetView = NULL;
IDXGISwapChain *swapChain = NULL;
ID3D11RasterizerState *rasterizerState = NULL;
ID3D11DepthStencilView *depthStencilView = NULL;
ID3D11VertexShader *vertexShaderObject = NULL;
ID3D11PixelShader *pixelShaderObject = NULL;
ID3D11InputLayout *inputLayout = NULL;
ID3D11Buffer *vertexBufferSmileyPosition = NULL;
ID3D11Buffer *vertexBufferSmileyTexture = NULL;
ID3D11Buffer *constantBuffer = NULL;
ID3D11ShaderResourceView *shaderResourceViewSmileyTexture = NULL;
ID3D11ShaderResourceView *shaderResourceViewWhiteColorTexture = NULL;
ID3D11SamplerState *samplerStateSmileyTexture = NULL;
ID3D11SamplerState *samplerStateWhiteColorTexture = NULL;

XMMATRIX perspectiveProjectionMatrix;

FILE *logFile = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

HRESULT initialize(void);
HRESULT initializeSwapChain(void);
HRESULT initializeVertexShader(ID3DBlob **vertexShaderCode);
HRESULT initializePixelShader(ID3DBlob **pixelShaderCode);
HRESULT initializeInputLayout(ID3DBlob **vertexShaderCode);
HRESULT initializeSmileyBuffers(void);
HRESULT initializeConstantBuffers(void);
HRESULT disableBackFaceCulling(void);
HRESULT loadTexture(const wchar_t *fileName, ID3D11SamplerState **samplerState, ID3D11ShaderResourceView **shaderResourceView);
HRESULT makeWhiteColorTexture(ID3D11SamplerState **samplerState, ID3D11ShaderResourceView **shaderResourceView);
UINT createBitmapImage(long imageWidth, long imageHeight, byte *imageColorData, UINT imageColorDataSize, byte **bitmap);
void update(void);
void display(void);
void drawSmiley(void);
HRESULT resize(int width, int height);
void toggleFullscreen(HWND hWnd, bool isFullscreen);
void cleanUp(void);
void log(const char* message, ...);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsatnce, LPSTR lpszCmdLine, int nCmdShow)
{
    WNDCLASSEX wndClassEx;
    MSG message;
    TCHAR szApplicationTitle[] = TEXT("CG - Smiley Tweaked");
    TCHAR szApplicationClassName[] = TEXT("RTR_D3D_SMILEY_TWEAKED");
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

        break;

        case WM_CHAR:
            switch(wParam)
            {
                case 'F':
                case 'f':
                    isFullscreen = !isFullscreen;
                    toggleFullscreen(hWnd, isFullscreen);
                break;

                default:
                break;
            }

            if(wParam >= 0x30 && wParam <= 0x34)
            {
                textureMode = (wParam - 0x30);
            }
            else if(wParam >= 0x60 && wParam <= 0x64)
            {
                textureMode = (wParam - 0x60);
            }
            else
            {
                textureMode = 0;
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

    result = initializeInputLayout(&vertexShaderCode);

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

    result = initializeSmileyBuffers();

    if(FAILED(result))
    {
        return result;
    }

    result = disableBackFaceCulling();

    if(FAILED(result))
    {
        return result;
    }

    result = loadTexture(L"resources/smiley.bmp", &samplerStateSmileyTexture,  &shaderResourceViewSmileyTexture);

    if(FAILED(result))
    {
        return result;
    }

    result = makeWhiteColorTexture(&samplerStateWhiteColorTexture,  &shaderResourceViewWhiteColorTexture);

    if(FAILED(result))
    {
        return result;
    }

    clearColor[0] = 0.0f;
    clearColor[1] = 0.0f;
    clearColor[2] = 0.0f;
    clearColor[3] = 1.0f;

    perspectiveProjectionMatrix = XMMatrixIdentity();

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
    const char *vertexShaderSourceCode = "cbuffer ConstantBuffer" \
    "{" \
    "   float4x4 worldViewProjectionMatrix;" \
    "} // No semicolon" \
    "\n" \
    "struct vertex_shader_output" \
    "{" \
    "   float4 position: SV_POSITION;" \
    "   float2 textureCoordinates: TEXTURE_COORDINATES;" \
    "};" \
    "\n" \
    "vertex_shader_output main(float4 inPosition: POSITION, float2 inTextureCoordinates: TEXTURE_COORDINATES)" \
    "{" \
    "   vertex_shader_output vsOutput;" \
    "   vsOutput.position = mul(worldViewProjectionMatrix, inPosition);" \
    "   vsOutput.textureCoordinates = inTextureCoordinates;" \
    "   return vsOutput;" \
    "}";

    ID3DBlob *error = NULL;

    HRESULT result = D3DCompile(vertexShaderSourceCode,
        lstrlenA(vertexShaderSourceCode) + 1,
        "VS",
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
    const char *pixelShaderSourceCode = "Texture2D _texture2D;" \
    "SamplerState samplerState;" \
    "\n" \
    "float4 main(float4 inPosition: SV_POSITION, float4 inTextureCoordinates: TEXTURE_COORDINATES): SV_TARGET" \
    "{" \
    "   float4 outColor = _texture2D.Sample(samplerState, inTextureCoordinates);" \
    "   return outColor;" \
    "}";

    ID3DBlob *error = NULL;

    HRESULT result = D3DCompile(pixelShaderSourceCode,
        lstrlenA(pixelShaderSourceCode) + 1,
        "PS",
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
        log("[Error] | Failed to create pixel shader, error code: %#010x", result);
        return result;
    }

    deviceContext->PSSetShader(pixelShaderObject, 0, 0);

    return S_OK;
}

HRESULT initializeInputLayout(ID3DBlob **vertexShaderCode)
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

    inputLayoutDescriptor[1].SemanticName = "TEXTURE_COORDINATES";
    inputLayoutDescriptor[1].SemanticIndex = 0;
    inputLayoutDescriptor[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputLayoutDescriptor[1].InputSlot = CG_INPUT_SLOT_TEXTURE;
    inputLayoutDescriptor[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    inputLayoutDescriptor[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    inputLayoutDescriptor[1].InstanceDataStepRate = 0;

    HRESULT result = device->CreateInputLayout(inputLayoutDescriptor,
        2,
        (*vertexShaderCode)->GetBufferPointer(),
        (*vertexShaderCode)->GetBufferSize(),
        &inputLayout
    );

    if(FAILED(result))
    {
        log("[Error] | Failed to create input layout, error code: %#010x", result);
        return result;
    }

    deviceContext->IASetInputLayout(inputLayout);

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

HRESULT initializeSmileyBuffers(void)
{
    const float smileyVertices[] = {
        -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
    };

    // Position
    D3D11_BUFFER_DESC vertexBufferDescriptor = {};
    ZeroMemory((void *)&vertexBufferDescriptor, sizeof(D3D11_BUFFER_DESC));

    vertexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDescriptor.ByteWidth = sizeof(float) * ARRAYSIZE(smileyVertices);
    vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT result = device->CreateBuffer(&vertexBufferDescriptor, nullptr, &vertexBufferSmileyPosition);

    if(FAILED(result))
    {
        log("[Error] | Cannot create vertex buffer for position, error code: %#010x\n", result);
        return result;
    }

    D3D11_MAPPED_SUBRESOURCE mappedSubresource = {};
    ZeroMemory((void *)&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    deviceContext->Map(vertexBufferSmileyPosition, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
    memcpy(mappedSubresource.pData, smileyVertices, sizeof(smileyVertices));
    deviceContext->Unmap(vertexBufferSmileyPosition, NULL);

    // Texture
    ZeroMemory((void *)&vertexBufferDescriptor, sizeof(D3D11_BUFFER_DESC));

    vertexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDescriptor.ByteWidth = sizeof(float) * 12;
    vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    result = device->CreateBuffer(&vertexBufferDescriptor, nullptr, &vertexBufferSmileyTexture);

    if(FAILED(result))
    {
        log("[Error] | Cannot create vertex buffer for texture, error code: %#010x\n", result);
        return result;
    }

    return S_OK;
}

void display(void)
{
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    drawSmiley();

    swapChain->Present(0, 0);
}

void drawSmiley(void)
{
    UINT stride = sizeof(float) * 3;
    UINT offset = 0;
    UINT textureStride = sizeof(float) * 2;
    UINT textureOffset = 0;

    // As we are using dynamic draw, pass the coordinates here as per texture mode.
    float smileyTextureCoordinates[12] = {0.0f};
    ID3D11ShaderResourceView *currentShaderResourceView = shaderResourceViewSmileyTexture;
    ID3D11SamplerState *currentSamplerState = samplerStateSmileyTexture;

    if(textureMode == 1)
    {
        smileyTextureCoordinates[0] = 0.0f;
        smileyTextureCoordinates[1] = 0.5f;
        smileyTextureCoordinates[2] = 0.5f;
        smileyTextureCoordinates[3] = 0.5f;
        smileyTextureCoordinates[4] = 0.0f;
        smileyTextureCoordinates[5] = 1.0f;
        smileyTextureCoordinates[6] = 0.0f;
        smileyTextureCoordinates[7] = 1.0f;
        smileyTextureCoordinates[8] = 0.5f;
        smileyTextureCoordinates[9] = 0.5f;
        smileyTextureCoordinates[10] = 0.5f;
        smileyTextureCoordinates[11] = 1.0f;
    }
    else if(textureMode == 2)
    {
        smileyTextureCoordinates[0] = 0.0f;
        smileyTextureCoordinates[1] = 0.0f;
        smileyTextureCoordinates[2] = 1.0f;
        smileyTextureCoordinates[3] = 0.0f;
        smileyTextureCoordinates[4] = 0.0f;
        smileyTextureCoordinates[5] = 1.0f;
        smileyTextureCoordinates[6] = 0.0f;
        smileyTextureCoordinates[7] = 1.0f;
        smileyTextureCoordinates[8] = 1.0f;
        smileyTextureCoordinates[9] = 0.0f;
        smileyTextureCoordinates[10] = 1.0f;
        smileyTextureCoordinates[11] = 1.0f;
    }
    else if(textureMode == 3)
    {
        smileyTextureCoordinates[0] = 0.0f;
        smileyTextureCoordinates[1] = 0.0f;
        smileyTextureCoordinates[2] = 2.0f;
        smileyTextureCoordinates[3] = 0.0f;
        smileyTextureCoordinates[4] = 0.0f;
        smileyTextureCoordinates[5] = 2.0f;
        smileyTextureCoordinates[6] = 0.0f;
        smileyTextureCoordinates[7] = 2.0f;
        smileyTextureCoordinates[8] = 2.0f;
        smileyTextureCoordinates[9] = 0.0f;
        smileyTextureCoordinates[10] = 2.0f;
        smileyTextureCoordinates[11] = 2.0f;
    }
    else if(textureMode == 4)
    {
        smileyTextureCoordinates[0] = 0.5f;
        smileyTextureCoordinates[1] = 0.5f;
        smileyTextureCoordinates[2] = 0.5f;
        smileyTextureCoordinates[3] = 0.5f;
        smileyTextureCoordinates[4] = 0.5f;
        smileyTextureCoordinates[5] = 0.5f;
        smileyTextureCoordinates[6] = 0.5f;
        smileyTextureCoordinates[7] = 0.5f;
        smileyTextureCoordinates[8] = 0.5f;
        smileyTextureCoordinates[9] = 0.5f;
        smileyTextureCoordinates[10] = 0.5f;
        smileyTextureCoordinates[11] = 0.5f;
    }
    else
    {
        currentShaderResourceView = shaderResourceViewWhiteColorTexture;
        currentSamplerState = samplerStateWhiteColorTexture;
        smileyTextureCoordinates[0] = 0.0f;
        smileyTextureCoordinates[1] = 0.0f;
        smileyTextureCoordinates[2] = 1.0f;
        smileyTextureCoordinates[3] = 0.0f;
        smileyTextureCoordinates[4] = 0.0f;
        smileyTextureCoordinates[5] = 1.0f;
        smileyTextureCoordinates[6] = 0.0f;
        smileyTextureCoordinates[7] = 1.0f;
        smileyTextureCoordinates[8] = 1.0f;
        smileyTextureCoordinates[9] = 0.0f;
        smileyTextureCoordinates[10] = 1.0f;
        smileyTextureCoordinates[11] = 1.0f;
    }

    D3D11_MAPPED_SUBRESOURCE mappedSubresource = {};
    ZeroMemory((void *)&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    deviceContext->Map(vertexBufferSmileyTexture, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
    memcpy(mappedSubresource.pData, smileyTextureCoordinates, sizeof(smileyTextureCoordinates));
    deviceContext->Unmap(vertexBufferSmileyTexture, NULL);

    deviceContext->IASetVertexBuffers(CG_INPUT_SLOT_VERTEX_POSITION, 1, &vertexBufferSmileyPosition, &stride, &offset);
    deviceContext->IASetVertexBuffers(CG_INPUT_SLOT_TEXTURE, 1, &vertexBufferSmileyTexture, &textureStride, &textureOffset);
    deviceContext->PSSetShaderResources(0, 1, &currentShaderResourceView);
    deviceContext->PSSetSamplers(0, 1, &currentSamplerState);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    XMMATRIX worldMatrix = XMMatrixIdentity();
    XMMATRIX viewMatrix = XMMatrixIdentity();
    XMMATRIX translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 6.0f);

    worldMatrix = translationMatrix;
    XMMATRIX worldViewProjectionMatrix = worldMatrix * viewMatrix * perspectiveProjectionMatrix;

    CBuffer cBuffer;
    ZeroMemory((void *)&cBuffer, sizeof(CBuffer));

    cBuffer.worldViewProjectionMatrix = worldViewProjectionMatrix;

    deviceContext->UpdateSubresource(constantBuffer, 0, NULL, &cBuffer, 0, 0);
    deviceContext->Draw(6, 0);
}

HRESULT loadTexture(const wchar_t *fileName, ID3D11SamplerState **samplerState, ID3D11ShaderResourceView **shaderResourceView)
{
    HRESULT result = DirectX::CreateWICTextureFromFile(device, deviceContext, fileName, NULL, shaderResourceView);

    if(FAILED(result))
    {
        log("[Error] | Cannot load '%s', error code: %#010x", fileName, result);
        return result;
    }

    D3D11_SAMPLER_DESC samplerDescriptor = {};
    ZeroMemory((void *)&samplerDescriptor, sizeof(D3D11_SAMPLER_DESC));

    samplerDescriptor.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDescriptor.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDescriptor.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDescriptor.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

    result = device->CreateSamplerState(&samplerDescriptor, samplerState);

    if(FAILED(result))
    {
        log("[Error] | Cannot create sampler state for '%s', error code: %#010x", fileName, result);
        return result;
    }

    return S_OK;
}

HRESULT makeWhiteColorTexture(ID3D11SamplerState **samplerState, ID3D11ShaderResourceView **shaderResourceView)
{
    const long imageWidth = 1;
    const long imageHeight = 1;

    byte whiteColorImage[4] = { (byte)255, (byte)255, (byte)255, (byte)255 };

    byte *bitmapImage = NULL;
    UINT totalBitmapImageSize = createBitmapImage(imageWidth, imageHeight, whiteColorImage, sizeof(whiteColorImage), &bitmapImage);

    HRESULT result = DirectX::CreateWICTextureFromMemory(device, deviceContext, (const uint8_t*)bitmapImage, (size_t)totalBitmapImageSize, NULL, shaderResourceView, NULL);

    if(bitmapImage != NULL)
    {
        free(bitmapImage);
        bitmapImage = NULL;
    }

    if(FAILED(result))
    {
        log("[Error] | Cannot create white color texture, error code: %#010x\n", result);
        return result;
    }

    D3D11_SAMPLER_DESC samplerDescriptor = {};
    ZeroMemory((void *)&samplerDescriptor, sizeof(D3D11_SAMPLER_DESC));

    samplerDescriptor.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDescriptor.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDescriptor.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDescriptor.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

    result = device->CreateSamplerState(&samplerDescriptor, samplerState);

    if(FAILED(result))
    {
        log("[Error] | Cannot create sampler state for white color texture, error code: %#010x\n", result);
        return result;
    }

    return S_OK;
}

UINT createBitmapImage(long imageWidth, long imageHeight, byte *imageColorData, UINT imageColorDataSize, byte **bitmap)
{
    BITMAPINFOHEADER bitmapInfoHeader;
	memset((void *)&bitmapInfoHeader, 0, sizeof(BITMAPINFOHEADER));

    bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfoHeader.biWidth = imageWidth;
    bitmapInfoHeader.biHeight = imageHeight;
    bitmapInfoHeader.biPlanes = 1;
    bitmapInfoHeader.biBitCount = 32; // 4 bytes for each color component.
    bitmapInfoHeader.biCompression = BI_RGB;
    bitmapInfoHeader.biXPelsPerMeter = 0;
    bitmapInfoHeader.biYPelsPerMeter = 0;

    WORD cClrBits = (WORD)(bitmapInfoHeader.biPlanes * bitmapInfoHeader.biBitCount);

    if (cClrBits == 1)
    {
        cClrBits = 1;
    }
    else if (cClrBits <= 4)
    {
        cClrBits = 4;
    }
    else if (cClrBits <= 8)
    {
        cClrBits = 8;
    }
    else if (cClrBits <= 16)
    {
        cClrBits = 16;
    }
    else if (cClrBits <= 24)
    {
        cClrBits = 24;
    }
    else
    {
        cClrBits = 32;
    }

    if (cClrBits < 24)
    {
        bitmapInfoHeader.biClrUsed = (1 << cClrBits);
    }

    // If the bitmap is not compressed, set the BI_RGB flag.
    bitmapInfoHeader.biCompression = BI_RGB;

    // Compute the number of bytes in the array of color
    // indices and store the result in biSizeImage.
    // The width must be DWORD aligned unless the bitmap is RLE
    // compressed.
    bitmapInfoHeader.biSizeImage = ((bitmapInfoHeader.biWidth * cClrBits + 31) & ~31) / 8 * bitmapInfoHeader.biHeight;

    // Set biClrImportant to 0, indicating that all of the
    // device colors are important.
    bitmapInfoHeader.biClrImportant = 0;

    BITMAPFILEHEADER bitmapFileHeader;
    memset((void *)&bitmapFileHeader, 0, sizeof(BITMAPFILEHEADER));

    bitmapFileHeader.bfType = 0x4d42; // 0x42 = "B" 0x4d = "M"

    // Compute the offset to the array of color indices.
    bitmapFileHeader.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + bitmapInfoHeader.biSize + bitmapInfoHeader.biClrUsed * sizeof (RGBQUAD); ;

    // Compute the size of the entire file.
    bitmapFileHeader.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) +  bitmapInfoHeader.biSize + bitmapInfoHeader.biClrUsed * sizeof(RGBQUAD) + bitmapInfoHeader.biSizeImage);

    bitmapFileHeader.bfReserved1 = 0;
    bitmapFileHeader.bfReserved2 = 0;

    BITMAPINFO bitmapInfo;
	memset((void *)&bitmapInfo, 0, sizeof(BITMAPINFO));
	bitmapInfo.bmiHeader = bitmapInfoHeader;

    UINT sizeofBitmapFileHeader = sizeof(bitmapFileHeader);
    UINT sizeofBitmapInfoHeader = sizeof(bitmapInfoHeader);
    UINT totalBitmapImageSize =  sizeofBitmapFileHeader + sizeofBitmapInfoHeader + imageColorDataSize;

    *bitmap = (BYTE *)malloc(totalBitmapImageSize);
    memset((void *)*bitmap, 0, totalBitmapImageSize);

	memcpy_s(*bitmap, totalBitmapImageSize, &bitmapFileHeader, sizeofBitmapFileHeader);
	memcpy_s(*bitmap + sizeofBitmapFileHeader, totalBitmapImageSize, &bitmapInfoHeader, sizeofBitmapInfoHeader);
	memcpy_s(*bitmap + sizeofBitmapFileHeader + sizeofBitmapInfoHeader, totalBitmapImageSize, imageColorData, imageColorDataSize);

    return totalBitmapImageSize;
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

    if(samplerStateSmileyTexture != NULL)
    {
        samplerStateSmileyTexture->Release();
        samplerStateSmileyTexture = NULL;
    }

    if(shaderResourceViewSmileyTexture != NULL)
    {
        shaderResourceViewSmileyTexture->Release();
        shaderResourceViewSmileyTexture = NULL;
    }

    if(samplerStateWhiteColorTexture != NULL)
    {
        samplerStateWhiteColorTexture->Release();
        samplerStateWhiteColorTexture = NULL;
    }

    if(shaderResourceViewWhiteColorTexture != NULL)
    {
        shaderResourceViewWhiteColorTexture->Release();
        shaderResourceViewWhiteColorTexture = NULL;
    }

    if(rasterizerState != NULL)
    {
        rasterizerState->Release();
        rasterizerState = NULL;
    }

    if(vertexBufferSmileyPosition != NULL)
    {
        vertexBufferSmileyPosition->Release();
        vertexBufferSmileyPosition = NULL;
    }

    if(vertexBufferSmileyTexture != NULL)
    {
        vertexBufferSmileyTexture->Release();
        vertexBufferSmileyTexture = NULL;
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

    if(pixelShaderObject != NULL)
    {
        pixelShaderObject->Release();
        pixelShaderObject = NULL;
    }

    if(vertexShaderObject != NULL)
    {
        vertexShaderObject->Release();
        vertexShaderObject = NULL;
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
