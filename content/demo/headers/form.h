#ifndef _FORM_H_
#define _FORM_H_

#include <string>
#include <SDL\SDL.h>
#include "demo.h"
#include "size.h"

class form
{
private:
    std::string _title;
    phi::size<unsigned int> _size;
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

	std::string _applicationPath;

private:
	void initWindow();
protected:
    virtual void update() = 0;
    virtual void render() = 0;
    void handleEvents();

public:
    form();
    ~form();

    std::string getTitle() const { return _title; }
    phi::size<unsigned int> getSize() const { return _size; }
    bool getIsFullScreen() const { return _isFullScreen; }
    unsigned int getFps() const { return _fps; }
    double getDt() const { return _dt; }
    double getInputCost() const { return _inputCost; }
    double getUpdateCost() const { return _updateCost; }
    double getRenderCost() const { return _renderCost; }
	std::string getApplicationPath() const { return _applicationPath; }
	void setTitle(std::string value);
    void setSize(phi::size<unsigned int> value);
	void setIsFullScreen(bool value);

    virtual void onResize(SDL_Event e){}
    virtual void onMouseDown(SDL_Event e){}
    virtual void onMouseMove(SDL_Event e){}
    virtual void onMouseUp(SDL_Event e){}
    virtual void onMouseWheel(SDL_Event e){}
    virtual void onKeyDown(SDL_Event e){}
    virtual void onKeyUp(SDL_Event e){}
    virtual void onClosing(){}
    
	virtual void initialize(std::string applicationPath) { _applicationPath = applicationPath; }

    bool loop();
    void centerScreen();
    void close();
};

#endif