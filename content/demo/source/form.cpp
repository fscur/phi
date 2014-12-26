#if WIN32
    #include <GL/glew.h>
#else
    #include <OpenGL/gl3.h>
#endif
#include "input.h"
#include "form.h"
#include "globals.h"

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

    _glContext = SDL_GL_CreateContext(_window);

    if (!_glContext)
        LOG("Could not create context: " << SDL_GetError());

#ifdef WIN32
    glewExperimental = GL_TRUE;

    GLenum glewInitStatus = glewInit();

    if(glewInitStatus != GLEW_OK)
        LOG("Error: " << glewGetErrorString(glewInitStatus));
#endif

    // initGL
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void form::handleEvents()
{
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0)
    {
        phi::mouseEventArgs me;

        switch (e.type)
        {
        case SDL_QUIT:
            onClosing();
            _isClosed = true;
            break;
        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_RESIZED)
                onResize(e);
            break;
        case SDL_MOUSEBUTTONDOWN:
            onMouseDown(e);
            me = phi::mouseEventArgs();
            me.leftButtonPressed = e.button.button == SDL_BUTTON_LEFT;
            me.rightButtonPressed = e.button.button == SDL_BUTTON_RIGHT;
            me.middleButtonPressed = e.button.button == SDL_BUTTON_MIDDLE;
            me.x = e.motion.x;
            me.y = e.motion.y;
            phi::input::notifyMouseDown(me);
            break;
        case SDL_MOUSEMOTION:
            onMouseMove(e);
            me = phi::mouseEventArgs();
            me.x = e.motion.x;
            me.y = e.motion.y;
            phi::input::notifyMouseMove(me);
            break;
        case SDL_MOUSEBUTTONUP:
            onMouseUp(e);
            me = phi::mouseEventArgs();
            me.leftButtonPressed = e.button.button == SDL_BUTTON_LEFT;
            me.rightButtonPressed = e.button.button == SDL_BUTTON_RIGHT;
            me.middleButtonPressed = e.button.button == SDL_BUTTON_MIDDLE;
            me.x = e.motion.x;
            me.y = e.motion.y;
            phi::input::notifyMouseUp(me);
            break;
        case SDL_MOUSEWHEEL:
            onMouseWheel(e);
            me = phi::mouseEventArgs();
            me.wheelDelta = (float)e.wheel.y;
            me.x = e.motion.x;
            me.y = e.motion.y;
            phi::input::notifyMouseWheel(me);
        case SDL_KEYDOWN:
            onKeyDown(e);
            break;
        case SDL_KEYUP:
            onKeyUp(e);
            if (e.key.keysym.sym == SDLK_F11)
                setIsFullScreen(!getIsFullScreen());
            break;
        }
    }
}

bool form::loop()
{
    _now = SDL_GetTicks();
    _dt = (double)(_now - _lastTime) * 10e-3;
    _lastTime = _now;

    _inputCost = SDL_GetTicks();
    handleEvents();
    _inputCost = (double)(SDL_GetTicks() - (double)_inputCost) * 10e-3;

    _updateCost = SDL_GetTicks(); 
    update();
    _updateCost = (double)(SDL_GetTicks() - (double)_updateCost) * 10e-3;

    _renderCost = SDL_GetTicks(); 
    render();
    SDL_GL_SwapWindow(_window);
    _renderCost = (double)(SDL_GetTicks() - (double)_renderCost) * 10e-3;

    _frames++;
    _processedTime += _dt;

    if (_processedTime > 1.0f)
    {
        _fps = _frames;
        _frames = 0;
        _processedTime -= 1.0;
    }

    return !_isClosed;
}

void form::centerScreen()
{
    SDL_SetWindowPosition(_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void form::close()
{
    onClosing();

    _isClosed = true;
}