#include <precompiled.h>
#include "..\window.h"

#include <core\input.h>

namespace phi
{
    static HINSTANCE hInstance;
    static HDC hDc;
    HWND hWnd;

    static PIXELFORMATDESCRIPTOR getPixelFormat()
    {
        PIXELFORMATDESCRIPTOR result = {};
        result.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        result.nVersion = 1;
        result.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        result.iPixelType = PFD_TYPE_RGBA;
        result.cColorBits = 32;
        result.cDepthBits = 24;
        result.cStencilBits = 8;
        result.cAuxBuffers = 0;
        result.iLayerType = PFD_MAIN_PLANE;
        return result;
    }

    LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        LRESULT result = NULL;
        switch (message)
        {
        case WM_ACTIVATE:
        {
            if (!HIWORD(wParam)) // Is minimized
            {
                // active
            }
            else
            {
                // inactive
            }

            return 0;
        }
        case WM_SYSCOMMAND:
        {
            switch (wParam)
            {
            case SC_SCREENSAVE:
            case SC_MONITORPOWER:
                return 0;
            }
            result = DefWindowProc(hWnd, message, wParam, lParam);
        } break;
        case WM_SETFOCUS:
            //FocusCallback(window, true);
            break;
        case WM_KILLFOCUS:
            //FocusCallback(window, false);
            break;
        case WM_CLOSE:
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_CHAR:
            // TODO: handle text
            break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (wParam >= 65 && wParam <= 90)
                input::notifyKeyDown(wParam + 32);
            else
                input::notifyKeyDown(wParam);
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            if (wParam >= 65 && wParam <= 90)
                input::notifyKeyUp(wParam + 32);
            else
                input::notifyKeyUp(wParam);
            break;
        case WM_MOUSEWHEEL:
            input::notifyMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
            break;
        case WM_MOUSEMOVE:
            input::notifyMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;
        case WM_LBUTTONDOWN:
            input::notifyLeftMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;
        case WM_LBUTTONUP:
            input::notifyLeftMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;
        case WM_RBUTTONDOWN:
            input::notifyRightMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;
        case WM_RBUTTONUP:
            input::notifyRightMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;
        case WM_MBUTTONDOWN:
            input::notifyMiddleMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;
        case WM_MBUTTONUP:
            input::notifyMiddleMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;
        case WM_SIZE:
            //resize
            break;
        default:
            result = DefWindowProc(hWnd, message, wParam, lParam);
        }

        return result;
    }

    void window::init()
    {
        hInstance = GetModuleHandle(NULL);
        WNDCLASSEX wndClass;

        wndClass.cbSize = sizeof(WNDCLASSEX);
        wndClass.style = CS_HREDRAW | CS_VREDRAW;
        wndClass.lpfnWndProc = (WNDPROC)wndProc;
        wndClass.cbClsExtra = 0;
        wndClass.cbWndExtra = 0;
        wndClass.hInstance = hInstance;
        wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wndClass.lpszMenuName = NULL;
        wndClass.lpszClassName = _name.c_str();
        wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

        if (!RegisterClassEx(&wndClass))
            throw "Could not register window class!\n";

        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

        RECT windowRect;
        windowRect.left = (long)screenWidth / 2 - _width / 2;
        windowRect.right = (long)_width;
        windowRect.top = (long)screenHeight / 2 - _height / 2;
        windowRect.bottom = (long)_height;

        AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

        hWnd = CreateWindowEx(0,
            _name.c_str(),
            _name.c_str(),
            dwStyle,
            windowRect.left,
            windowRect.top,
            windowRect.right,
            windowRect.bottom,
            NULL,
            NULL,
            hInstance,
            NULL);

        if (!hWnd)
            throw ("Could not create window!\n");

        hDc = GetDC(hWnd);
        PIXELFORMATDESCRIPTOR pfd = getPixelFormat();

        int pixelFormat = ChoosePixelFormat(hDc, &pfd);
        if (pixelFormat)
        {
            if (!SetPixelFormat(hDc, pixelFormat, &pfd))
            {
                throw "Failed setting pixel format!";
            }
        }
        else
        {
            throw "Failed choosing pixel format!";
        }

        HGLRC hrc = wglCreateContext(hDc);
        if (hrc)
        {
            if (!wglMakeCurrent(hDc, hrc))
            {
                throw "Failed setting OpenGL context!";
            }
        }
        else
        {
            throw "Failed creating OpenGL context!";
        }

        if (glewInit() != GLEW_OK)
        {
            throw "Could not initialize GLEW!";
        }

        ShowWindow(hWnd, SW_SHOW);
        SetForegroundWindow(hWnd);
        SetFocus(hWnd);

        onInitialize();
    }

    void window::input()
    {   
        MSG msg;

        while (PeekMessage(&msg, hWnd, NULL, NULL, PM_REMOVE) > 0)
        {
            if (msg.message == WM_QUIT)
            {
                closed = true;
                return;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}