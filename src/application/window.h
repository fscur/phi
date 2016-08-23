#pragma once
#include <phi.h>
#include "applicationApi.h"
#include <core\eventHandler.h>
#include <core\resolution.h>
#include <core\mouseEventArgs.h>
#include <core\keyboardEventArgs.h>

namespace phi
{
    class window
    {
    private:
        static eventToken _resizeToken;

    public:
        static eventHandler<resolution> resize;

    protected:
        wstring _title;
        resolution _resolution;
        vec2 _dpi;

    public:
        bool closed;

    private:
        void adjustWindowToScreenBounds();
    public:
        APPLICATION_API window(wstring title, resolution resolution);
        APPLICATION_API virtual ~window();

        APPLICATION_API void init();
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
        virtual void onSwapBuffers() {};

        APPLICATION_API virtual void onTick();
        APPLICATION_API virtual void onResize(resolution resolution);

    public:
        APPLICATION_API static void freezeMouse();
        APPLICATION_API static void unfreezeMouse();
        APPLICATION_API static void showCursor();
        APPLICATION_API static void hideCursor();
    };
}