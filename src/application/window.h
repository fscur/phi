#pragma once
#include <phi.h>
#include "applicationApi.h"

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

    private:
        void adjustWindowToScreenBounds();
    public:
        APPLICATION_API window(string name, uint width, uint height);
        APPLICATION_API virtual ~window();

        APPLICATION_API void init();
        APPLICATION_API void clear();
        APPLICATION_API void render();
        APPLICATION_API void input();
        APPLICATION_API void update();
        APPLICATION_API void tick();
        APPLICATION_API void swapBuffers();
        APPLICATION_API void close();

        APPLICATION_API virtual void onInit() = 0;
        APPLICATION_API virtual void onUpdate() = 0;
        APPLICATION_API virtual void onRender() = 0;
        APPLICATION_API virtual void onClosing() = 0;

        APPLICATION_API virtual void onTick();
    };
}