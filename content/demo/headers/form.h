#ifndef _FORM_H_
#define _FORM_H_

#include <string>
#include "demo.h"
#include <SDL\SDL.h>

class form
{
private:
    std::string _title;
    int _width;
    int _height;
    bool _isClosed;
    bool _isFullScreen;
    SDL_Window* _window;
    SDL_GLContext _glContext;

    double _dt;
    Uint32 _lastTime;
    double _processedTime;
    unsigned int _frames;
    Uint32 _now;
    Uint32 _inputCost;
    Uint32 _updateCost;
    Uint32 _renderCost;
    unsigned int _fps;

protected:
    virtual void update() = 0;
    virtual void render() = 0;
    void handleEvents();

public:
    form();
    ~form();

    std::string getTitle() { return _title; }
    int getWidth() { return _width; }
    int getHeight() { return _height; }
    bool getIsFullScreen() { return _isFullScreen; }
    unsigned int getFps() { return _fps; }
    double getDt() { return _dt; }
    double getInputCost() { return _inputCost; }
    double getUpdateCost() { return _updateCost; }
    double getRenderCost() { return _renderCost; }

	void setTitle(std::string value);
    void setWidth(int value);
	void setHeight(int value);
	void setIsFullScreen(bool value);

    virtual void onResize(SDL_Event e){}
    virtual void onMouseDown(SDL_Event e){}
    virtual void onMouseMove(SDL_Event e){}
    virtual void onMouseUp(SDL_Event e){}
    virtual void onMouseWheel(SDL_Event e){}
    virtual void onKeyDown(SDL_Event e){}
    virtual void onKeyUp(SDL_Event e){}
    virtual void onClosing(){}

    virtual void initialize();
    bool loop();
    void centerScreen();
    void close();
};

#endif