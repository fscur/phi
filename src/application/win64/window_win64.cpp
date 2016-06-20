#include <precompiled.h>
#include "..\window.h"
#include <core\input.h>
#include <core\exception.h>

#include <core\invalidInitializationException.h>

namespace phi
{
    //TODO: free memory
    HWND _windowHandle;
    HINSTANCE _applicationInstance;
    HDC _deviceContext;
    HGLRC _renderingContext;
    DWORD _windowExStyle = 0;
    DWORD _windowStyle = WS_OVERLAPPEDWINDOW;
    LPARAM _lastMouseMove;
    resolution _currentResolution;

    bool _isBeingFirstShown = true;
    bool _hasEnteredSizeMove = false;
    bool _isResizing = false;
    bool _isBeingRestored = false;
    bool _isBeingMaximized = false;
    bool _isBeingMinimized = false;
    bool _wasMaximized = false;
    bool _wasMinimized = false;

    int convertToKey(WPARAM wParam)
    {
        switch (wParam)
        {
        case VK_BACK: return PHIK_BACKSPACE;
        case VK_TAB: return PHIK_TAB;
        case VK_CLEAR: return PHIK_CLEAR;
        case VK_RETURN: return PHIK_RETURN;
        case VK_SHIFT: return PHIK_SHIFT;
        case VK_CONTROL: return PHIK_CTRL;
        case VK_MENU: return PHIK_MENU;
        case VK_PAUSE: return PHIK_PAUSE;
        case VK_CAPITAL: return PHIK_CAPSLOCK;
        case VK_ESCAPE: return PHIK_ESCAPE;
        case VK_SPACE: return PHIK_SPACE;
        case VK_PRIOR: return PHIK_PAGEUP;
        case VK_NEXT: return PHIK_PAGEDOWN;
        case VK_END: return PHIK_END;
        case VK_HOME: return PHIK_HOME;
        case VK_LEFT: return PHIK_LEFT;
        case VK_UP: return PHIK_UP;
        case VK_RIGHT: return PHIK_RIGHT;
        case VK_DOWN: return PHIK_DOWN;
        case VK_SELECT: return PHIK_SELECT;
        case VK_EXECUTE: return PHIK_EXECUTE;
        case VK_SNAPSHOT: return PHIK_PRINTSCREEN;
        case VK_INSERT: return PHIK_INSERT;
        case VK_DELETE: return PHIK_DELETE;
        case VK_HELP: return PHIK_HELP;
        case 0x30: return PHIK_0;
        case 0x31: return PHIK_1;
        case 0x32: return PHIK_2;
        case 0x33: return PHIK_3;
        case 0x34: return PHIK_4;
        case 0x35: return PHIK_5;
        case 0x36: return PHIK_6;
        case 0x37: return PHIK_7;
        case 0x38: return PHIK_8;
        case 0x39: return PHIK_9;
        case 0x41: return PHIK_a;
        case 0x42: return PHIK_b;
        case 0x43: return PHIK_c;
        case 0x44: return PHIK_d;
        case 0x45: return PHIK_e;
        case 0x46: return PHIK_f;
        case 0x47: return PHIK_g;
        case 0x48: return PHIK_h;
        case 0x49: return PHIK_i;
        case 0x4A: return PHIK_j;
        case 0x4B: return PHIK_k;
        case 0x4C: return PHIK_l;
        case 0x4D: return PHIK_m;
        case 0x4E: return PHIK_n;
        case 0x4F: return PHIK_o;
        case 0x50: return PHIK_p;
        case 0x51: return PHIK_q;
        case 0x52: return PHIK_r;
        case 0x53: return PHIK_s;
        case 0x54: return PHIK_t;
        case 0x55: return PHIK_u;
        case 0x56: return PHIK_v;
        case 0x57: return PHIK_w;
        case 0x58: return PHIK_x;
        case 0x59: return PHIK_y;
        case 0x5A: return PHIK_z;
        case VK_APPS: return PHIK_APPLICATION; // TODO: is this the right key?
        case VK_SLEEP: return PHIK_SLEEP;
        case VK_NUMPAD0: return PHIK_KP_0;
        case VK_NUMPAD1: return PHIK_KP_1;
        case VK_NUMPAD2: return PHIK_KP_2;
        case VK_NUMPAD3: return PHIK_KP_3;
        case VK_NUMPAD4: return PHIK_KP_4;
        case VK_NUMPAD5: return PHIK_KP_5;
        case VK_NUMPAD6: return PHIK_KP_6;
        case VK_NUMPAD7: return PHIK_KP_7;
        case VK_NUMPAD8: return PHIK_KP_8;
        case VK_NUMPAD9: return PHIK_KP_9;
        case VK_MULTIPLY: return PHIK_KP_MULTIPLY;
        case VK_ADD: return PHIK_KP_PLUS;
        case VK_SUBTRACT: return PHIK_KP_MINUS;
        case VK_DIVIDE: return PHIK_KP_DIVIDE;
        case VK_DECIMAL: return PHIK_KP_DECIMAL;
        case VK_F1: return PHIK_F1;
        case VK_F2: return PHIK_F2;
        case VK_F3: return PHIK_F3;
        case VK_F4: return PHIK_F4;
        case VK_F5: return PHIK_F5;
        case VK_F6: return PHIK_F6;
        case VK_F7: return PHIK_F7;
        case VK_F8: return PHIK_F8;
        case VK_F9: return PHIK_F9;
        case VK_F10: return PHIK_F10;
        case VK_F11: return PHIK_F11;
        case VK_F12: return PHIK_F12;
        case VK_F13: return PHIK_F13;
        case VK_F14: return PHIK_F14;
        case VK_F15: return PHIK_F15;
        case VK_F16: return PHIK_F16;
        case VK_F17: return PHIK_F17;
        case VK_F18: return PHIK_F18;
        case VK_F19: return PHIK_F19;
        case VK_F20: return PHIK_F20;
        case VK_F21: return PHIK_F21;
        case VK_F22: return PHIK_F22;
        case VK_F23: return PHIK_F23;
        case VK_F24: return PHIK_F24;
        case VK_NUMLOCK: return PHIK_NUMLOCKCLEAR;
        case VK_SCROLL: return PHIK_SCROLLLOCK;
        case VK_BROWSER_BACK: return PHIK_AC_BACK;
        case VK_BROWSER_FORWARD: return PHIK_AC_FORWARD;
        case VK_BROWSER_REFRESH: return PHIK_AC_REFRESH;
        case VK_BROWSER_STOP: return PHIK_AC_STOP;
        case VK_BROWSER_SEARCH: return PHIK_AC_SEARCH;
        case VK_BROWSER_FAVORITES: return PHIK_AC_BOOKMARKS;
        case VK_BROWSER_HOME: return PHIK_AC_HOME;
        case VK_VOLUME_MUTE: return PHIK_MUTE;
        case VK_VOLUME_DOWN: return PHIK_VOLUMEDOWN;
        case VK_VOLUME_UP: return PHIK_VOLUMEUP;
        case VK_MEDIA_NEXT_TRACK: return PHIK_AUDIONEXT;
        case VK_MEDIA_PREV_TRACK: return PHIK_AUDIOPREV;
        case VK_MEDIA_STOP: return PHIK_AUDIOSTOP;
        case VK_MEDIA_PLAY_PAUSE: return PHIK_AUDIOPLAY;
        case VK_LAUNCH_MAIL: return PHIK_MAIL;
        case VK_LAUNCH_MEDIA_SELECT: return PHIK_MEDIASELECT;
        }

        return 0;
    }

    LRESULT onActivate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

    LRESULT onSysCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (wParam)
        {
        case SC_SCREENSAVE:
        case SC_MONITORPOWER:
            return 0;
        case SC_MINIMIZE:
            _wasMaximized = _isBeingMaximized;
            _isBeingMinimized = true;
            _isBeingMaximized = false;
            _isBeingRestored = false;
            break;
        case SC_MAXIMIZE:
            _wasMinimized = _isBeingMinimized;
            _isBeingMinimized = false;
            _isBeingMaximized = true;
            _isBeingRestored = false;
            break;
        case SC_RESTORE:
            _wasMaximized = _isBeingMaximized;
            _wasMinimized = _isBeingMinimized;
            _isBeingMaximized = false;
            _isBeingMinimized = false;
            _isBeingRestored = true;
            break;
        }

        return DefWindowProcW(hWnd, message, wParam, lParam);
    }

    LRESULT onWindowPosChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        const WINDOWPOS* windowPos = (WINDOWPOS *)lParam;

        RECT rectangle = { 0, 0, 0, 0 };
        GetClientRect(hWnd, &rectangle);
        auto width = rectangle.right - rectangle.left;
        auto height = rectangle.bottom - rectangle.top;

        _currentResolution = resolution(static_cast<float>(width), static_cast<float>(height), getDpi());
        auto flags = windowPos->flags;

        if (flags & SWP_SHOWWINDOW)
        {
            if (_isBeingFirstShown)
            {
                window::resize.raise(_currentResolution);
                _isBeingFirstShown = false;
            }
        }
        
        if (flags & SWP_HIDEWINDOW)
        {
            return 0;
        }

        if (!(flags & SWP_NOMOVE))
        {
            //debug("window_moved_to(pwp->x, pwp->y)");
        }

        if (!(flags & SWP_NOSIZE))
        {
            if (_isBeingMinimized)
                return 0;

            if (_wasMinimized)
            {
                _wasMinimized = false;
                return 0;
            }

            bool hasToRaiseResize = false;

            if (_isBeingMaximized)
            {
                hasToRaiseResize = true;
            }
            else if (_isBeingRestored && _wasMaximized)
            {
                hasToRaiseResize = true;
                _isBeingRestored = false;
                _wasMaximized = false;
            }
            else if (!_hasEnteredSizeMove)
                hasToRaiseResize = true;

            if (_hasEnteredSizeMove)
                _isResizing = true;

            if (hasToRaiseResize)
            {
                window::resize.raise(_currentResolution);
                return 0;
            }
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    LRESULT onEnterSizeMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        _hasEnteredSizeMove = true;
        return 0;
    }

    LRESULT onExitSizeMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (_isResizing)
            window::resize.raise(_currentResolution);

        _hasEnteredSizeMove = false;
        _isResizing = false;
        return 0;
    }

    LRESULT onDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        PostQuitMessage(0);
        return 0;
    }

    LRESULT onKeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        input::notifyKeyDown(convertToKey(wParam));
        return 0;
    }

    LRESULT onKeyUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        input::notifyKeyUp(convertToKey(wParam));
        return 0;
    }

    LRESULT onMouseWheel(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        auto point = POINT();
        point.x = GET_X_LPARAM(lParam);
        point.y = GET_Y_LPARAM(lParam);
        ScreenToClient(_windowHandle, &point);
        input::notifyMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam), point.x, point.y);
        return 0;
    }

    LRESULT onLButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        input::notifyLeftMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        SetCapture(_windowHandle);
        return 0;
    }

    LRESULT onLButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        input::notifyLeftMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        ReleaseCapture();
        return 0;
    }

    LRESULT onRButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        input::notifyRightMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        SetCapture(_windowHandle);
        return 0;
    }

    LRESULT onRButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        input::notifyRightMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        ReleaseCapture();
        return 0;
    }

    LRESULT onMButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        input::notifyMiddleMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        SetCapture(_windowHandle);
        return 0;
    }

    LRESULT onMButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        input::notifyMiddleMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        ReleaseCapture();
        return 0;
    }

    LRESULT onMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (lParam != _lastMouseMove)
            input::notifyMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        _lastMouseMove = lParam;
        return 0;
    }

    LRESULT onCaptureChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        //Any operation that uses SetCapture function and has an end state should set it end state here,
        //for example changing the cursor at the end of a drag and drog operation.
        return 0;
    }

    LRESULT onDpiChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        auto rect = *reinterpret_cast<RECT *>(lParam);

        HWND doesNotHaveRelativeWindow = 0;

        SetWindowPos(hWnd,
            doesNotHaveRelativeWindow,
            rect.left,
            rect.top,
            rect.right - rect.left,
            rect.bottom - rect.top,
            SWP_NOACTIVATE | SWP_NOZORDER);
    }

    LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_ACTIVATE:
            return onActivate(hWnd, message, wParam, lParam);
        case WM_SYSCOMMAND:
            return onSysCommand(hWnd, message, wParam, lParam);
        case WM_CLOSE:
        case WM_DESTROY:
            return onDestroy(hWnd, message, wParam, lParam);
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            return onKeyDown(hWnd, message, wParam, lParam);
        case WM_KEYUP:
        case WM_SYSKEYUP:
            return onKeyUp(hWnd, message, wParam, lParam);
        case WM_MOUSEWHEEL:
            return onMouseWheel(hWnd, message, wParam, lParam);
        case WM_LBUTTONDOWN:
            return onLButtonDown(hWnd, message, wParam, lParam);
        case WM_LBUTTONUP:
            return onLButtonUp(hWnd, message, wParam, lParam);
        case WM_RBUTTONDOWN:
            return onRButtonDown(hWnd, message, wParam, lParam);
        case WM_RBUTTONUP:
            return onRButtonUp(hWnd, message, wParam, lParam);
        case WM_MBUTTONDOWN:
            return onMButtonDown(hWnd, message, wParam, lParam);
        case WM_MBUTTONUP:
            return onMButtonUp(hWnd, message, wParam, lParam);
        case WM_MOUSEMOVE:
            return onMouseMove(hWnd, message, wParam, lParam);
        case WM_CAPTURECHANGED:
            return onCaptureChanged(hWnd, message, wParam, lParam);
        case WM_ENTERSIZEMOVE:
            return onEnterSizeMove(hWnd, message, wParam, lParam);
        case WM_EXITSIZEMOVE:
            return onExitSizeMove(hWnd, message, wParam, lParam);
        case WM_WINDOWPOSCHANGED:
            return onWindowPosChanged(hWnd, message, wParam, lParam);
        case WM_DPICHANGED:
            return onDpiChanged(hWnd, message, wParam, lParam);
        }

        return DefWindowProcW(hWnd, message, wParam, lParam);
    }

    void createWindow(wstring title, resolution resolution)
    {
        _applicationInstance = GetModuleHandle(NULL);
        WNDCLASSEXW wndClass;
        auto cTitle = title.c_str();
        wndClass.cbSize = sizeof(WNDCLASSEXW);
        wndClass.style = CS_HREDRAW | CS_VREDRAW;
        wndClass.lpfnWndProc = (WNDPROC)windowProcedure;
        wndClass.cbClsExtra = 0;
        wndClass.cbWndExtra = 0;
        wndClass.hInstance = _applicationInstance;
        wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wndClass.lpszMenuName = NULL;
        wndClass.lpszClassName = title.c_str();
        wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

        if (!RegisterClassExW(&wndClass))
            throw invalidInitializationException("Could not register window class!");

        auto screenWidth = static_cast<float>(GetSystemMetrics(SM_CXSCREEN));
        auto screenHeight = static_cast<float>(GetSystemMetrics(SM_CYSCREEN));

        RECT windowRect;
        windowRect.left = static_cast<long>(screenWidth * 0.5f - resolution.width * 0.5f);
        windowRect.right = static_cast<long>(windowRect.left + resolution.width);
        windowRect.top = static_cast<long>(screenHeight * 0.5f - resolution.height * 0.5f);
        windowRect.bottom = static_cast<long>(windowRect.top + resolution.height);

        AdjustWindowRectEx(&windowRect, _windowStyle, FALSE, _windowExStyle);

        _windowHandle = CreateWindowExW(
            0,
            cTitle,
            cTitle,
            _windowStyle,
            windowRect.left,
            windowRect.top,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            NULL,
            NULL,
            _applicationInstance,
            NULL);

        if (!_windowHandle)
            throw invalidInitializationException("Could not create the window!");
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

    vec2 getDpi()
    {
        int dpiX = GetDeviceCaps(_deviceContext, LOGPIXELSX);
        int dpiY = GetDeviceCaps(_deviceContext, LOGPIXELSY);

        return vec2(dpiX, dpiY);
    }

    HGLRC createFakeGLContext()
    {
        HGLRC renderingContext = wglCreateContext(_deviceContext);

        if (!renderingContext)
            throw invalidInitializationException("Could not create fake OpenGL context!");

        if (!wglMakeCurrent(_deviceContext, renderingContext))
            throw invalidInitializationException("Could not set fake OpenGL context!");

        return renderingContext;
    }

    void createGLContext()
    {
        _deviceContext = GetDC(_windowHandle);

        PIXELFORMATDESCRIPTOR pixelFormatDescriptor = getPixelFormatDescriptor();

        int pixelFormat = ChoosePixelFormat(_deviceContext, &pixelFormatDescriptor);

        if (!pixelFormat)
            throw invalidInitializationException("Could not choose pixel format!");

        if (!SetPixelFormat(_deviceContext, pixelFormat, &pixelFormatDescriptor))
            throw invalidInitializationException("Could not set pixel format!");

        //we need to create a fake context so we have access to the opengl extensions
        HGLRC tempRenderingContext = createFakeGLContext();

        GLenum err = glewInit();

        if (err != GLEW_OK)
            throw invalidInitializationException("GLEW is not initialized!");

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
            throw invalidInitializationException("Could not create a GL context!");
    }

    void releaseGLContext()
    {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(_renderingContext);
    }

    void window::init()
    {
        adjustWindowToScreenBounds();
        createWindow(_title, _resolution);
        createGLContext();

        ShowWindow(_windowHandle, SW_SHOW);
        SetForegroundWindow(_windowHandle);
        SetFocus(_windowHandle);

        _resolution.dpi = getDpi();

        onInit();

        _resizeToken = window::resize.assign([&](phi::resolution resolution)
        {
            onResize(resolution);
        });
    }

    void window::adjustWindowToScreenBounds()
    {
        auto screenWidth = static_cast<float>(GetSystemMetrics(SM_CXSCREEN));
        auto screenHeight = static_cast<float>(GetSystemMetrics(SM_CYSCREEN));

        RECT rectangle = { 0, 0, 0, 0 };
        AdjustWindowRectEx(&rectangle, _windowStyle, FALSE, _windowExStyle);

        auto verticalBorderSize = static_cast<float>(rectangle.bottom - rectangle.top);

        auto width = std::min(_resolution.width, screenWidth);
        auto height = std::min(_resolution.height, screenHeight - verticalBorderSize);
        _resolution = resolution(width, height);
    }

    void window::input()
    {
        MSG msg;

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
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
        window::resize.unassign(_resizeToken);

        onClosing();

        releaseGLContext();
        ReleaseDC(_windowHandle, _deviceContext);
        DestroyWindow(_windowHandle);
    }
}