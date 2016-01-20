#include "phi/demo/form.h"

#include "phi/core/input.h"
#include "phi/core/globals.h"
#include "phi/core/clock.h"

#include <string>
#include <locale>
#include <codecvt>

#if WIN32
#include <GL/glew.h>
#include <SDL\SDL_syswm.h>
#else
#include <OpenGL/gl3.h>
#endif

#define THREADS_OFF

form::form()
{
    _title = "Form";
    _window = nullptr;
    _glContext = nullptr;
    _isClosed = false;

    _lastTime = SDL_GetTicks();
    _processedTime = 0.0;
    _frames = 0;
    _now = 0;
    _inputCost = 0;
    _updateCost = 0;
    _renderCost = 0;

    initWindow();
}

form::~form()
{
    SDL_GL_DeleteContext(_glContext);
    _glContext = nullptr;

    SDL_DestroyWindow(_window);
    _window = nullptr;
}

void form::setTitle(std::string value)
{ 
    _title = value;

    if (_window == nullptr)
        return;

    SDL_SetWindowTitle(_window, _title.c_str());
}

void form::setSize(phi::sizef value)
{
    _size = value;
}

void form::setIsFullScreen(bool value)
{
    _isFullScreen = value;

    if (_isFullScreen)
    {
        if (SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN) < 0)
            printf("SDL could not enter fullscreen mode! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        if (SDL_SetWindowFullscreen(_window, 0) < 0)
            printf("SDL could not leave fullscreen mode! SDL_Error: %s\n", SDL_GetError());
        else
            SDL_SetWindowPosition(_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
}

void form::initialize(std::string applicationPath)
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0);
    _applicationPath = applicationPath;
    onInitialize();
}

void form::initWindow()
{
    SDL_Rect r;
    if (SDL_GetDisplayBounds(0, &r) != 0)
        SDL_Log("SDL_GetDisplayBounds failed: %s", SDL_GetError());

    //createGLWindow
    _window = SDL_CreateWindow(
        _title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        //r.w,
        //r.h,
        1024,
        768,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
        //SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);

    _isFullScreen = false;

    if (_window == NULL)
        phi::log("Window could not be created! SDL_Error: " + std::string(SDL_GetError()));

    int width = 0;
    int height = 0;
    SDL_GetWindowSize(_window, &width, &height);
    auto s = phi::sizef((float)width, (float)height);
    _size = phi::sizef((float)width, (float)height);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_StopTextInput();
    //SDL_ShowCursor(0);
    _glContext = SDL_GL_CreateContext(_window);

    if (!_glContext)
        phi::log("Could not create context: " + std::string(SDL_GetError()));

#ifdef WIN32
    glewExperimental = GL_TRUE;

    GLenum glewInitStatus = glewInit();

    if(glewInitStatus != GLEW_OK)
        std::cout << "Error: " << glewGetErrorString(glewInitStatus);

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(_window, &wmInfo);
    _hwnd = wmInfo.info.win.window;

#endif
}

void form::show()
{
    //Comment both lines to run with a single thread:
#ifdef THREADS_ON
    SDL_GL_MakeCurrent(_window, nullptr);
    _renderThread = SDL_CreateThread(renderLoopWrapper, "renderThread", this);
#endif
}

void form::input()
{
//    onBeginInput();
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0)
    {
        phi::mouseEventArgs me;
        phi::keyboardEventArgs ke;

        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

        switch (e.type)
        {
        case SDL_QUIT:
            close();
            break;
        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_RESIZED)
                onResize(e);
            break;
        case SDL_MOUSEBUTTONDOWN:
            me = phi::mouseEventArgs();
            me.leftButtonPressed = e.button.button == SDL_BUTTON_LEFT;
            me.rightButtonPressed = e.button.button == SDL_BUTTON_RIGHT;
            me.middleButtonPressed = e.button.button == SDL_BUTTON_MIDDLE;
            me.isCtrlPressed = currentKeyStates[SDL_SCANCODE_LCTRL] == 0 ? false : true;
            me.isShiftPressed = currentKeyStates[SDL_SCANCODE_LSHIFT] == 0 ? false : true;
            me.isAltPressed = currentKeyStates[SDL_SCANCODE_LALT] == 0 ? false : true;
            me.x = e.motion.x;
            me.y = e.motion.y;
            me.clicks = e.button.clicks;
            phi::input::notifyMouseDown(&me);
            if (!me.handled)
                onMouseDown(&me);
            break;
        case SDL_MOUSEMOTION:
            _lastMousePos = phi::vec2(e.motion.x, e.motion.y);
            me = phi::mouseEventArgs();
            me.x = e.motion.x;
            me.y = e.motion.y;
            phi::input::notifyMouseMove(&me);
            if (!me.handled)
                onMouseMove(&me);
            break;
        case SDL_MOUSEBUTTONUP:
            me = phi::mouseEventArgs();
            me.leftButtonPressed = e.button.button == SDL_BUTTON_LEFT;
            me.rightButtonPressed = e.button.button == SDL_BUTTON_RIGHT;
            me.middleButtonPressed = e.button.button == SDL_BUTTON_MIDDLE;
            me.isCtrlPressed = currentKeyStates[SDL_SCANCODE_LCTRL] == 0 ? false : true;
            me.isShiftPressed = currentKeyStates[SDL_SCANCODE_LSHIFT] == 0 ? false : true;
            me.isAltPressed = currentKeyStates[SDL_SCANCODE_LALT] == 0 ? false : true;
            me.x = e.motion.x;
            me.y = e.motion.y;
            me.clicks = e.button.clicks;
            phi::input::notifyMouseUp(&me);
            if (!me.handled)
                onMouseUp(&me);
            break;
        case SDL_MOUSEWHEEL:
            me = phi::mouseEventArgs();
            me.wheelDelta = (float)e.wheel.y;
            me.x = (int)_lastMousePos.x;
            me.y = (int)_lastMousePos.y;
            phi::input::notifyMouseWheel(&me);
            if (!me.handled)
                onMouseWheel(&me);
            break;
        case SDL_KEYDOWN:
            if (!SDL_IsTextInputActive())
            {
                ke.key = e.key.keysym.scancode;
                ke.isCtrlPressed = currentKeyStates[SDL_SCANCODE_LCTRL] == 0 ? false : true;
                ke.isShiftPressed = currentKeyStates[SDL_SCANCODE_LSHIFT] == 0 ? false : true;
                ke.isAltPressed = currentKeyStates[SDL_SCANCODE_LALT] == 0 ? false : true;
                onKeyDown(&ke);
                phi::input::notifyKeyDown(ke);
            }
            else
            {
                if (e.key.keysym.sym == PHIK_BACKSPACE || 
                    e.key.keysym.sym == PHIK_DELETE || 
                    e.key.keysym.sym == PHIK_LEFT ||
                    e.key.keysym.sym == PHIK_RIGHT ||
                    e.key.keysym.sym == PHIK_HOME ||
                    e.key.keysym.sym == PHIK_END)
                {
                    ke.key = e.key.keysym.sym;
                    ke.isCtrlPressed = currentKeyStates[SDL_SCANCODE_LCTRL] == 0 ? false : true;
                    ke.isShiftPressed = currentKeyStates[SDL_SCANCODE_LSHIFT] == 0 ? false : true;
                    ke.isAltPressed = currentKeyStates[SDL_SCANCODE_LALT] == 0 ? false : true;
                    phi::input::notifyKeyDown(ke);
                }
            }
            break;

        case SDL_KEYUP:
            ke.key = e.key.keysym.sym;
            ke.isCtrlPressed = currentKeyStates[SDL_SCANCODE_LCTRL] == 0 ? false : true;
            ke.isShiftPressed = currentKeyStates[SDL_SCANCODE_LSHIFT] == 0 ? false : true;
            ke.isAltPressed = currentKeyStates[SDL_SCANCODE_LALT] == 0 ? false : true;
            onKeyUp(&ke);

            if (e.key.keysym.sym == SDLK_F11)
                setIsFullScreen(!getIsFullScreen());

            phi::input::notifyKeyUp(ke);
            break;

        case SDL_TEXTINPUT:
            ke.key = e.text.text[0];
            phi::input::notifyKeyDown(ke);

            break;
        }
    }

    //onEndInput();
}

bool form::loop()
{
    //_now = SDL_GetTicks();
    //phi::clock::millisecondsElapsed = _now - phi::clock::totalMillisecondsElapsed;
    //phi::clock::totalMillisecondsElapsed = _now;

    //_inputCost0 = SDL_GetTicks();
    //input();
    //_inputCost = SDL_GetTicks() - _inputCost0;

    //_updateCost0 = SDL_GetTicks(); 
    update();
    //_updateCost = SDL_GetTicks() - _updateCost0;

    //Uncomment both lines to run with a single thread:
    render();
    SDL_GL_SwapWindow(_window);

    //_frames++;
    //_processedTime += _now - _lastTime;

    //if (_processedTime > 1000.0f)
    //{
    //    _fps = _frames;
    //    _frames = 0;
    //    _processedTime -= 1000.0f;
    //    //log(std::string(_fps, '+') + "[" + std::to_string(_fps) + "]");
    //}

    //_lastTime = _now;

    return !_isClosed;
}

int form::renderLoopWrapper(void *data)
{
    form* self = static_cast<form*>(data);
    return self->renderLoop();
}

int form::renderLoop()
{
    int s = SDL_GL_MakeCurrent(_window, _glContext);

    if (s != 0)
        phi::log(std::string(SDL_GetError()));

    while (!_isClosed)
    {
        //Uint32 now = SDL_GetTicks();
        //_dt = ((double)(now - _lastTime)) * 1e-3;

        //_renderCost0 = SDL_GetTicks();
        render();
        //_renderCost = SDL_GetTicks() - _renderCost0;
        SDL_GL_SwapWindow(_window);

        _frames++;
        _processedTime += _dt;

        if (_processedTime > 1.0)
        {
            _fps = _frames;
            _frames = 0;
            _processedTime -= 1.0;
        }

        //_lastTime = now;
    }

    return 0;
}

void form::centerScreen()
{
    SDL_SetWindowPosition(_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void form::close()
{
    onClosing();

    _isClosed = true;
#ifdef THREADS_ON
    SDL_WaitThread(_renderThread, nullptr);
#endif
}

void form::onResize(SDL_Event e)
{
    auto sz = phi::sizef((float)e.window.data1, (float)e.window.data2);
    setSize(sz);
}

void form::resize(phi::sizef size)
{
    setSize(size);

    if (_isFullScreen)
        SDL_SetWindowFullscreen(_window, 0);

    SDL_SetWindowSize(_window, (int)size.w, (int)size.h);

    if (_isFullScreen)
        SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);

    auto a = SDL_Event();
    a.window.data1 = (uint)size.w;
    a.window.data2 = (uint)size.h;

    onResize(a);
}