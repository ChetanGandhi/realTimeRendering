#include <windows.h>
#include <cstdlib>

#include "resources/resource.h"

HWND hWnd;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

int colorCode = 8;

bool isFullscreen = FALSE;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

void toggleFullscreen(bool isFullscreen);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsatnce, LPSTR lpszCmdLine, int nCmdShow)
{
    WNDCLASSEX wndClassEx;
    MSG message;
    TCHAR szApplicationTitle[] = TEXT("CG - Fullscreen Color Window");
    TCHAR szApplicationClassName[] = TEXT("RTR_FULLSCREEN_COLOR_WINDOW");

    wndClassEx.cbSize = sizeof(WNDCLASSEX);
    wndClassEx.style = CS_HREDRAW | CS_VREDRAW;
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

    // DWORD styleExtra = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    DWORD style = WS_OVERLAPPEDWINDOW;

    // RECT windowRect = {0, 0, 800, 600};
    // AdjustWindowRectEx(&windowRect, style, FALSE, styleExtra);

    hWnd = CreateWindow(
        szApplicationClassName,
        szApplicationTitle,
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
        );

    if(!hWnd)
    {
        MessageBox(NULL, TEXT("Cannot create windows."), TEXT("Error"), MB_OK | MB_ICONERROR);
        exit(EXIT_FAILURE);
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while(GetMessage(&message, NULL, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return (int)message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    RECT rect;
    PAINTSTRUCT paintStruct;
    HBRUSH hBrush;
    COLORREF color;

    switch(iMessage)
    {
        case WM_DESTROY:
        PostQuitMessage(0);
        break;

        case WM_KEYDOWN:
        switch(wParam)
        {
            case VK_ESCAPE:

            DestroyWindow(hWnd);

            break;

            // 0x46 is hex value for key 'F' or 'f'
            case 0x46:

            isFullscreen = !isFullscreen;
            toggleFullscreen(isFullscreen);

            break;

            case 0x52: // r
            colorCode = 1;
            InvalidateRect(hWnd, NULL, TRUE);
            break;

            case 0x47: // g
            colorCode = 2;
            InvalidateRect(hWnd, NULL, TRUE);
            break;

            case 0x42: // b
            colorCode = 3;
            InvalidateRect(hWnd, NULL, TRUE);
            break;

            case 0x43: // c
            colorCode = 4;
            InvalidateRect(hWnd, NULL, TRUE);
            break;

            case 0x4D: // m
            colorCode = 5;
            InvalidateRect(hWnd, NULL, TRUE);
            break;

            case 0x59: // y
            colorCode = 6;
            InvalidateRect(hWnd, NULL, TRUE);
            break;

            case 0x57: // w
            colorCode = 7;
            InvalidateRect(hWnd, NULL, TRUE);
            break;

            case 0x4B: // k
            colorCode = 8;
            InvalidateRect(hWnd, NULL, TRUE);
            break;

            default:
            MessageBox(hWnd, TEXT("No color assigned to this key."), TEXT("Error"), MB_OK | MB_ICONERROR);
            break;
        }

        break;

        case WM_PAINT:

        switch(colorCode)
        {
            case 1: // red
            color = RGB(255, 0, 0);
            break;

            case 2: // green
            color = RGB(0, 255, 0);
            break;

            case 3: // blue
            color = RGB(0, 0, 255);
            break;

            case 4: // cyan
            color = RGB(0, 255, 255);
            break;

            case 5: // magenta
            color = RGB(255, 0, 255);
            break;

            case 6: // yellow
            color = RGB(255, 255, 0);
            break;

            case 7: // white
            color = RGB(255, 255, 255);
            break;

            // Default and case 8 (black) is same.
            case 8: // black
            default:
            color = RGB(0, 0, 0);
            break;
        }

        GetUpdateRect(hWnd, &rect, 0);

        if (IsRectEmpty(&rect))
        {
            GetClientRect(hWnd, &rect);
        }

        hBrush = CreateSolidBrush(color);

        if(hBrush == NULL)
        {
            break;
        }

        hdc = BeginPaint(hWnd, &paintStruct);

        if(hdc == NULL)
        {
            DeleteObject(hBrush);
            break;
        }

        FillRect(hdc, &rect, hBrush);
        EndPaint(hWnd, &paintStruct);
        ReleaseDC(hWnd, hdc);
        DeleteObject(hBrush);

        return 0L;

        default:
        break;
    }

    return DefWindowProc(hWnd, iMessage, wParam, lParam);
}

void toggleFullscreen(bool isFullscreen)
{
    MONITORINFO monitorInfo;
    DWORD dwStyle = dwStyle = GetWindowLong(hWnd, GWL_STYLE);

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
