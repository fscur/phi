#ifndef _FORM_H_
#define _FORM_H_

#include "demo.h"

#include <phi/core/globals.h>
#include <phi/core/size.h>
#include <phi/core/mouseEventArgs.h>
#include <phi/core/keyboardEventArgs.h>

#include <SDL/SDL.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <string>

class form
{
private:
    std::string _title;
    phi::sizef _size;
    bool _isClosed;
    bool _isFullScreen;
    SDL_Window* _window;
    SDL_GLContext _glContext;
    SDL_Thread* _renderThread;

    double _dt;
    Uint32 _lastTime;
    double _processedTime;
    unsigned int _frames;
    Uint32 _now;
    Uint32 _inputCost0;
    Uint32 _updateCost0;
    Uint32 _renderCost0;
    Uint32 _inputCost;
    Uint32 _updateCost;
    Uint32 _renderCost;
    float _renderSecondSum;
    unsigned int _fps;

    std::string _applicationPath;
    phi::vec2 _lastMousePos;

    HWND _hwnd;

private:
    void initWindow();
    void input();
    static int renderLoopWrapper(void *data);
    int renderLoop();
protected:
    /*virtual void onBeginInput() = 0;
    virtual void onEndInput() = 0;*/
    virtual void update() = 0;
    virtual void render() = 0;

public:
    form();
    ~form();

    std::string getTitle() const { return _title; }
    phi::sizef getSize() const { return _size; }
    bool getIsFullScreen() const { return _isFullScreen; }
    unsigned int getFps() const { return _fps; }
    double getDt() const { return _dt; }
    double getInputCost() const { return _inputCost; }
    double getUpdateCost() const { return _updateCost; }
    double getRenderCost() const { return _renderCost; }
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