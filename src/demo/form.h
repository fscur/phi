#ifndef _FORM_H_
#define _FORM_H_

#include "demo.h"

#include <core/globals.h>
#include <core/size.h>
#include <core/mouseEventArgs.h>
#include <core/keyboardEventArgs.h>

#include <SDL/SDL.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <string>

class form
{
private:
    std::string _title;
    bool _isClosed;
    bool _isFullScreen;
    SDL_Window* _window;
    SDL_GLContext _glContext;
    SDL_Thread* _renderThread;

    std::string _applicationPath;
    phi::vec2 _lastMousePos;

    HWND _hwnd;
protected:
    phi::sizef _size;
private:
    void initWindow();
    void input();
    static int renderLoopWrapper(void *data);
    int renderLoop();
protected:
    //virtual void onBeginInput() = 0;
    //virtual void onEndInput() = 0;
    virtual void update() = 0;
    virtual void render() = 0;

public:
    form();
    ~form();

    std::string getTitle() const { return _title; }
    phi::sizef getSize() const { return _size; }
    bool getIsFullScreen() const { return _isFullScreen; }
    std::string getApplicationPath() const { return _applicationPath; }
    SDL_Window* getWindow() const { return _window; }
    SDL_GLContext getGlContext() const { return _glContext; }

#ifdef WIN32
    HWND getHwnd() const { return _hwnd; }
#endif
    void setTitle(std::string value);
    void setSize(phi::sizef value);
    void setIsFullScreen(bool value);
    void show();
    void resize(phi::sizef size);

    virtual void onResize(SDL_Event e);
    virtual void onClosing(){}

    virtual void onInitialize(){}
    virtual void onMouseDown(phi::mouseEventArgs* e){}
    virtual void onMouseMove(phi::mouseEventArgs* e){}
    virtual void onMouseUp(phi::mouseEventArgs* e){}
    virtual void onMouseWheel(phi::mouseEventArgs* e){}
    virtual void onKeyDown(phi::keyboardEventArgs* e){}
    virtual void onKeyUp(phi::keyboardEventArgs* e){}

    void initialize(std::string applicationPath);
    bool loop();
    void centerScreen();
    void close();
};

#endif