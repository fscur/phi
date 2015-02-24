#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif
#include "input.h"
#include "form.h"
#include "globals.h"
#include <string>
#include <locale>
#include <codecvt>
#include "clock.h"


#define THREADS_ON

form::form()
{
    _title = "Form";
    _size = phi::size<unsigned int>(800, 600);
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
    _isFullScreen = false;

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

void form::setSize(phi::size<unsigned int> value)
{
    _size = value;

    if (_window == NULL)
        return;

    SDL_SetWindowSize(_window, (int)_size.width, (int)_size.height);
}

void form::setIsFullScreen(bool value)
{
    _isFullScreen = value;

    if (_isFullScreen)
    {
        if (SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP) < 0)
            printf("SDL could not enter fullscreen mode! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        if (SDL_SetWindowFullscreen(_window, 0) < 0)
            printf("SDL could not leave fullscreen mode! SDL_Error: %s\n", SDL_GetError());
    }
}

void form::initialize(std::string applicationPath)
{
    _applicationPath = applicationPath;
    onInitialize();
}

void form::initWindow()
{
    //createGLWindow
    _window = SDL_CreateWindow(
        _title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        _size.width,
        _size.height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (_window == NULL)
        LOG("Window could not be created! SDL_Error: " << SDL_GetError());

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_ShowCursor(0);

    _glContext = SDL_GL_CreateContext(_window);

    if (!_glContext)
        LOG("Could not create context: " << SDL_GetError());

#ifdef WIN32
    glewExperimental = GL_TRUE;

    GLenum glewInitStatus = glewInit();

    if(glewInitStatus != GLEW_OK)
        LOG("Error: " << glewGetErrorString(glewInitStatus));
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
    onBeginInput();
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
            me.x = e.motion.x;
            me.y = e.motion.y;
            me.clicks = e.button.clicks;
            onMouseDown(me);
            phi::input::notifyMouseDown(me);
            break;
        case SDL_MOUSEMOTION:
            _lastMousePos = glm::vec2(e.motion.x, e.motion.y);
            me = phi::mouseEventArgs();
            me.x = e.motion.x;
            me.y = e.motion.y;
            onMouseMove(me);
            phi::input::notifyMouseMove(me);
            break;
        case SDL_MOUSEBUTTONUP:
            me = phi::mouseEventArgs();
            me.leftButtonPressed = e.button.button == SDL_BUTTON_LEFT;
            me.rightButtonPressed = e.button.button == SDL_BUTTON_RIGHT;
            me.middleButtonPressed = e.button.button == SDL_BUTTON_MIDDLE;
            me.x = e.motion.x;
            me.y = e.motion.y;
            me.clicks = e.button.clicks;
            onMouseUp(me);
            phi::input::notifyMouseUp(me);
            break;
        case SDL_MOUSEWHEEL:
            me = phi::mouseEventArgs();
            me.wheelDelta = (float)e.wheel.y;
            me.x = _lastMousePos.x;
            me.y = _lastMousePos.y;
            onMouseWheel(me);
            phi::input::notifyMouseWheel(me);
            break;
        case SDL_KEYDOWN:
            if (!SDL_IsTextInputActive())
            {
                ke.key = e.key.keysym.sym;
                ke.isCtrlPressed = currentKeyStates[SDL_SCANCODE_LCTRL];
                ke.isShiftPressed = currentKeyStates[SDL_SCANCODE_LSHIFT];
                ke.isAltPressed = currentKeyStates[SDL_SCANCODE_LALT];
                onKeyDown(ke);
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
                    ke.isCtrlPressed = currentKeyStates[SDL_SCANCODE_LCTRL];
                    ke.isShiftPressed = currentKeyStates[SDL_SCANCODE_LSHIFT];
                    ke.isAltPressed = currentKeyStates[SDL_SCANCODE_LALT];
                    onKeyDown(ke);
                    phi::input::notifyKeyDown(ke);
                }
            }
            break;

        case SDL_KEYUP:
            ke.key = e.key.keysym.sym;
            ke.isCtrlPressed = (e.key.keysym.sym & (PHIK_RCTRL | PHIK_LCTRL)) > 0;
            ke.isShiftPressed = (e.key.keysym.sym & (PHIK_RSHIFT | PHIK_LSHIFT)) > 0;
            ke.isAltPressed = (e.key.keysym.sym & (PHIK_RALT | PHIK_LALT)) > 0;
            onKeyUp(ke);

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

    onEndInput();
}

bool form::loop()
{
    _now = SDL_GetTicks();
    phi::clock::millisecondsElapsed = _now;

    _inputCost0 = SDL_GetTicks();
    input();
    _inputCost = SDL_GetTicks() - _inputCost0;

    _updateCost0 = SDL_GetTicks(); 
    update();
    _updateCost = SDL_GetTicks() - _updateCost0;

    //Uncomment both lines to run with a single thread:
#ifndef THREADS_ON
    render();
    SDL_GL_SwapWindow(_window);
#endif
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
        LOG(SDL_GetError());

    while (!_isClosed)
    {
        Uint32 now = SDL_GetTicks();
        _dt = ((double)(now - _lastTime)) * 1e-3;

        _renderCost0 = SDL_GetTicks();
        render();
        _renderCost = SDL_GetTicks() - _renderCost0;
        SDL_GL_SwapWindow(_window);

        _frames++;
        _processedTime += _dt;

        if (_processedTime > 1.0)
        {
            _fps = _frames;
            _frames = 0;
            _processedTime -= 1.0;
        }

        _lastTime = now;
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