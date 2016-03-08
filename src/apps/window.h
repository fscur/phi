#pragma once
#include <phi.h>
#include "apps.h"
#include <core\mouseEventArgs.h>
#include <core\keyboardEventArgs.h>

namespace phi
{
    class window
    {
    protected:
        string _name;
        uint _width;
        uint _height;

    public:
        bool closed;

    public:
        APPS_API window(string name, uint width, uint height);
        APPS_API ~window();

        APPS_API void init();
        APPS_API void clear();
        APPS_API void render();
        APPS_API void input();
        APPS_API void update();
        APPS_API void tick();
        APPS_API void swapBuffers();
        APPS_API void close();

        APPS_API virtual void onTick();

        APPS_API virtual void onInitialize() = 0;
        APPS_API virtual void onUpdate() = 0;
        APPS_API virtual void onRender() = 0;
        APPS_API virtual void onClosing() = 0;

        APPS_API virtual void onMouseDown(phi::mouseEventArgs* e);
        APPS_API virtual void onMouseMove(phi::mouseEventArgs* e);
        APPS_API virtual void onMouseUp(phi::mouseEventArgs* e);
        APPS_API virtual void onMouseWheel(phi::mouseEventArgs* e);
        APPS_API virtual void onKeyDown(phi::keyboardEventArgs* e);
        APPS_API virtual void onKeyUp(phi::keyboardEventArgs* e);
    };
}