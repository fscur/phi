#include <precompiled.h>
#include "window.h"

namespace phi
{
    window::window(string name, uint width, uint height) :
        _name(name),
        _width(width),
        _height(height),
        closed(false){}

    window::~window(){}

    void window::clear() {}
    void window::render() {}
    void window::update() {}
    void window::tick() {}
    void window::swapBuffers() {}
    void window::close() {}
    void window::onTick(){}
    void window::onMouseDown(phi::mouseEventArgs* e){}
    void window::onMouseMove(phi::mouseEventArgs* e){}
    void window::onMouseUp(phi::mouseEventArgs* e){}
    void window::onMouseWheel(phi::mouseEventArgs* e){}
    void window::onKeyDown(phi::keyboardEventArgs* e){}
    void window::onKeyUp(phi::keyboardEventArgs* e){}
}