#include <precompiled.h>
#include "..\window.h"
#include <core\input.h>

namespace phi
{
    //TODO: free memory
    HWND _windowHandle;
    HINSTANCE _applicationInstance;
    HDC _deviceContext;
    HGLRC _renderingContext;

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
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            input::notifyKeyDown(wParam);
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
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

    void createWindow(string name, uint width, uint height)
    {
        _applicationInstance = GetModuleHandle(NULL);
        WNDCLASSEX wndClass;

        wndClass.cbSize = sizeof(WNDCLASSEX);
        wndClass.style = CS_HREDRAW | CS_VREDRAW;
        wndClass.lpfnWndProc = (WNDPROC)wndProc;
        wndClass.cbClsExtra = 0;
        wndClass.cbWndExtra = 0;
        wndClass.hInstance = _applicationInstance;
        wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wndClass.lpszMenuName = NULL;
        wndClass.lpszClassName = name.c_str();
        wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

        if (!RegisterClassEx(&wndClass))
            throw "Could not register window class!";

        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

        RECT windowRect;
        windowRect.left = (long)screenWidth / 2 - width / 2;
        windowRect.right = (long)width;
        windowRect.top = (long)screenHeight / 2 - height / 2;
        windowRect.bottom = (long)height;

        AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

        _windowHandle = CreateWindowEx(
            0,
            name.c_str(),
            name.c_str(),
            dwStyle,
            windowRect.left,
            windowRect.top,
            windowRect.right,
            windowRect.bottom,
            NULL,
            NULL,
            _applicationInstance,
            NULL);

        if (!_windowHandle)
            throw "Could not create the window!";
    }
    
    PIXELFORMATDESCRIPTOR getPixelFormatDescriptor()
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

    HGLRC createFakeGLContext()
    {
        HGLRC renderingContext = wglCreateContext(_deviceContext);

        if (!renderingContext)
            throw "Could not create fake OpenGL context!";

        if (!wglMakeCurrent(_deviceContext, renderingContext))
                throw "Could not set fake OpenGL context!";

        return renderingContext;
    }

    void createGLContext()
    {
        _deviceContext = GetDC(_windowHandle);

        PIXELFORMATDESCRIPTOR pixelFormatDescriptor = getPixelFormatDescriptor();

        int pixelFormat = ChoosePixelFormat(_deviceContext, &pixelFormatDescriptor);

        if (!pixelFormat)
            throw "Could not choose pixel format!";
        
        if (!SetPixelFormat(_deviceContext, pixelFormat, &pixelFormatDescriptor))
            throw "Could not set pixel format!";

        //we need to create a fake context so we have access to the opengl extensions
        HGLRC tempRenderingContext = createFakeGLContext();

        GLenum err = glewInit();

        if (err != GLEW_OK)
            throw "GLEW is not initialized!";

        int attribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 5,
            WGL_CONTEXT_FLAGS_ARB, 0, 0
        };

        if (wglewIsSupported("WGL_ARB_create_context") == GL_TRUE)
        {
            _renderingContext = wglCreateContextAttribsARB(_deviceContext, 0, attribs);
            wglMakeCurrent(NULL, NULL);
            wglDeleteContext(tempRenderingContext);
            wglMakeCurrent(_deviceContext, _renderingContext);
        }
        else
            _renderingContext = tempRenderingContext;

        if (!_renderingContext)
            throw "Could not create a GL context!";
    }

    void releaseGLContext()
    {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(_renderingContext);
    }

    void window::init()
    {
        createWindow(_name, _width, _height);
        createGLContext();

        ShowWindow(_windowHandle, SW_SHOW);
        SetForegroundWindow(_windowHandle);
        SetFocus(_windowHandle);

        onInit();
    }

    void window::input()
    {   
        MSG msg;

        while (PeekMessage(&msg, _windowHandle, NULL, NULL, PM_REMOVE) > 0)
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

    void window::swapBuffers()
    {
        SwapBuffers(_deviceContext);
    }

    void window::close()
    {
        onClosing();

        releaseGLContext();
        ReleaseDC(_windowHandle, _deviceContext);
        DestroyWindow(_windowHandle);
    }
}