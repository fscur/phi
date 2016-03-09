#pragma once
#include <phi.h>
#include "apps.h"
#include "window.h"

namespace phi
{
    class application
    {
    private:
        static bool _initialized;
    private:
        window* _window;
        bool _running;

    public:
        string fileName;
        uint framesPerSecond;
        double millisecondsPerFrame;

    private:
        void onInit();
        void onClear();
        void onRender();
        void onInput();
        void onUpdate();
        void onTick();
        void onSwapbuffers();
        void onClose();

    public:
        APPS_API application(string fileName);
        APPS_API ~application();

        APPS_API void run(window* window);
    };
}