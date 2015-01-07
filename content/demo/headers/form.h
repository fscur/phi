#ifndef _FORM_H_
#define _FORM_H_

#include <string>
#include <SDL\SDL.h>
#include "demo.h"
#include "size.h"
#include "mouseEventArgs.h"
#include "keyboardEventArgs.h"
#include <glm\glm.hpp>

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
    Uint32 _inputCost0;
    Uint32 _updateCost0;
    Uint32 _renderCost0;
	Uint32 _inputCost;
    Uint32 _updateCost;
    Uint32 _renderCost;
    unsigned int _fps;

	std::string _applicationPath;
	glm::vec2 _lastMousePos;

private:
	void initWindow();
    void input();
protected:
	virtual void onBeginInput() = 0;
	virtual void onEndInput() = 0;
    virtual void update() = 0;
    virtual void render() = 0;

public:
    form();
    ~form();

    std::string getTitle() const { return _title; }
    phi::size<unsigned int> getSize() const { return _size; }
    bool getIsFullScreen() const { return _isFullScreen; }
    unsigned int getFps() const { return _fps; }
    double getDt() const { return _dt; }
    double getInputCost() const { return _inputCost * 10e-3; }
    double getUpdateCost() const { return _updateCost * 10e-3; }
    double getRenderCost() const { return _renderCost * 10e-3; }
	std::string getApplicationPath() const { return _applicationPath; }
	void setTitle(std::string value);
    void setSize(phi::size<unsigned int> value);
	void setIsFullScreen(bool value);

    virtual void onResize(SDL_Event e){}
	virtual void onClosing(){}
    
    virtual void onMouseDown(phi::mouseEventArgs e){}
    virtual void onMouseMove(phi::mouseEventArgs e){}
    virtual void onMouseUp(phi::mouseEventArgs e){}
    virtual void onMouseWheel(phi::mouseEventArgs e){}
    virtual void onKeyDown(phi::keyboardEventArgs e){}
    virtual void onKeyUp(phi::keyboardEventArgs e){}
    
	virtual void initialize(std::string applicationPath) { _applicationPath = applicationPath; }

    bool loop();
    void centerScreen();
    void close();
};

#endif