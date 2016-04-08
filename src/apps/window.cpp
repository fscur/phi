#include <precompiled.h>
#include "window.h"

namespace phi
{
    window::window(string name, uint width, uint height) :
        _name(name),
        _width(width),
        _height(height),
        closed(false)
    {
    }

    window::~window()
    {
    }

    void window::clear() 
    {
        //onClear();
    }

    void window::render() 
    {
        onRender();
    }

    void window::update() 
    {
        onUpdate();
    }

    void window::tick() 
    {
        onTick();
    }

    void window::onTick(){}
}