#include <windows.h>
#include <cstdlib>

#include "resources/resource.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsatnce, LPSTR lpszCmdLine, int nCmdShow)
{
    WNDCLASSEX wndClassEx;
    HWND hWnd;
    MSG message;
    TCHAR szApplicationTitle[] = TEXT("CG - Empty Window");
    TCHAR szApplicationClassName[] = TEXT("RTR_WINDOW");

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

    switch(iMessage)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_PAINT:
            GetUpdateRect(hWnd, &rect, 0);

            if (IsRectEmpty(&rect))
            {
                GetClientRect(hWnd, &rect);
            }

            hBrush = CreateSolidBrush(RGB(0, 0, 255));

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
