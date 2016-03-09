#include <precompiled.h>
#include "window.h"

namespace phi
{
    static HINSTANCE hInstance;
    static HDC hDc;
    HWND hWnd;

    namespace eventType
    {
        enum eventType
        {
            none,
            onMouseMove,
            onMouseDown,
            onMouseUp,
            onMouseWhell,
            onKeyboardDown,
            onKeyboardUp
        };
    }

    mouseEventArgs _mouseEventArgs;
    keyboardEventArgs _keyboardEventArgs;
    eventType::eventType _eventType = eventType::none;
    bool _shouldRaiseMouseEvent = false;
    bool _shouldRaiseKeyboardEvent = false;

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

    void setKeyboardState(uint message, int flags, int key)
    {
        /*bool pressed = message == WM_KEYDOWN || message == WM_SYSKEYDOWN;
        inputManager->m_KeyState[key] = pressed;

        bool repeat = (flags >> 30) & 1;

        int modifier = 0;
        switch (key)
        {
        case SP_KEY_CONTROL:
        modifier = SP_MODIFIER_LEFT_CONTROL;
        break;
        case SP_KEY_ALT:
        modifier = SP_MODIFIER_LEFT_ALT;
        break;
        case SP_KEY_SHIFT:
        modifier = SP_MODIFIER_LEFT_SHIFT;
        break;
        }
        if (pressed)
        inputManager->m_KeyModifiers |= modifier;
        else
        inputManager->m_KeyModifiers &= ~(modifier);

        if (pressed)
        inputManager->m_EventCallback(KeyPressedEvent(key, repeat, inputManager->m_KeyModifiers));
        else
        inputManager->m_EventCallback(KeyReleasedEvent(key));*/

    }

    void setMouseModifiers(uint message, int flags, int key)
    {}

    void setMouseState(uint message, int x, int y)
    {
        _mouseEventArgs.x = x;
        _mouseEventArgs.y = y;

        switch (message)
        {
        case WM_LBUTTONDOWN:
            _mouseEventArgs.leftButtonPressed = true;
            _eventType = eventType::onMouseDown;
            break;
        case WM_LBUTTONUP:
            _mouseEventArgs.leftButtonPressed = true;
            _eventType = eventType::onMouseUp;
            break;
        case WM_RBUTTONDOWN:
            _mouseEventArgs.rightButtonPressed = true;
            _eventType = eventType::onMouseDown;
            break;
        case WM_RBUTTONUP:
            _mouseEventArgs.rightButtonPressed = true;
            _eventType = eventType::onMouseUp;
            break;
        case WM_MBUTTONDOWN:
            _mouseEventArgs.middleButtonPressed = true;
            _eventType = eventType::onMouseDown;
            break;
        case WM_MBUTTONUP:
            _mouseEventArgs.middleButtonPressed = true;
            _eventType = eventType::onMouseUp;
            break;
        default:
            return;
        }

        _shouldRaiseMouseEvent = true;
    }

    void raiseMouseEvent() 
    {
        phi::debug("mouse event raised: (" + std::to_string(_mouseEventArgs.x) + ", " + std::to_string(_mouseEventArgs.y) + ")");
    }

    void raiseKeyboardEvent() {}

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
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
            setKeyboardState(message, lParam, wParam);
            setMouseModifiers(message, lParam, wParam);
            break;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            setMouseState(message, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
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
    }

    void window::input()
    {
        _shouldRaiseMouseEvent = false;
        _shouldRaiseKeyboardEvent = false;

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

        if (_shouldRaiseMouseEvent)
            raiseMouseEvent();

        if (_shouldRaiseKeyboardEvent)
            raiseKeyboardEvent();
    }

    /*
   

    void VulkanExampleBase::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_CLOSE:
            prepared = false;
            DestroyWindow(hWnd);
            PostQuitMessage(0);
            break;
        case WM_PAINT:
            ValidateRect(window, NULL);
            break;
        case WM_KEYDOWN:
            switch (wParam)
            {
            case 0x50:
                paused = !paused;
                break;
            case VK_ESCAPE:
                exit(0);
                break;
            }
            keyPressed((uint32_t)wParam);
            break;
        case WM_RBUTTONDOWN:
        case WM_LBUTTONDOWN:
            mousePos.x = (float)LOWORD(lParam);
            mousePos.y = (float)HIWORD(lParam);
            break;
        case WM_MOUSEMOVE:
            if (wParam & MK_RBUTTON)
            {
                int32_t posx = LOWORD(lParam);
                int32_t posy = HIWORD(lParam);
                zoom += (mousePos.y - (float)posy) * .005f * zoomSpeed;
                mousePos = glm::vec2((float)posx, (float)posy);
                viewChanged();
            }
            if (wParam & MK_LBUTTON)
            {
                int32_t posx = LOWORD(lParam);
                int32_t posy = HIWORD(lParam);
                rotation.x += (mousePos.y - (float)posy) * 1.25f * rotationSpeed;
                rotation.y -= (mousePos.x - (float)posx) * 1.25f * rotationSpeed;
                mousePos = glm::vec2((float)posx, (float)posy);
                viewChanged();
            }
            break;
        }
    }
    */
}