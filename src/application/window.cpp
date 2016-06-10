#include <precompiled.h>
#include "window.h"

namespace phi
{
    window::window(wstring title, phi::resolution resolution) :
        _title(title),
        _resolution(resolution),
        closed(false)
    {
    }

    window::~window()
    {
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

    void window::onResize(resolution resolution){}
}