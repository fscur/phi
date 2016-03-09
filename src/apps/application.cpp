#include <precompiled.h>
#include "application.h"
#include <core\time.h>

namespace phi
{
    bool application::_initialized = false;

    application::application(string fileName) :
        _window(nullptr),
        _running(false),
        fileName(fileName),
        framesPerSecond(0),
        millisecondsPerFrame(0.0f)
    {
        assert(!_initialized);
        _initialized = true;
        time::start();
    }

    application::~application()
    {
    }

    void application::run(window* window)
    {
        _window = window;

        double timer = 0.0;
        uint frames = 0;
        onInit();

        _running = true;

        while (_running)
        {
            millisecondsPerFrame = time::deltaSeconds * 1000;

            //onClear();
            //onRender();
            onInput();
            onUpdate();

            time::update();
            
            if (time::totalSeconds - timer > 1.0)
            {
                timer += 1.0;
                framesPerSecond = frames;
                frames = 0;

                phi::debug("fps:" + std::to_string(framesPerSecond));
                phi::debug("timer:" + std::to_string(timer));

                //onTick();
            }

            if (_window->closed)
                _running = false;

            frames++;

            //onSwapbuffers();
        }
    }

    void application::onInit()
    {
        _window->init();
    }

    void application::onClear()
    {
        _window->clear();
    }

    void application::onRender()
    {
        _window->render();
    }

    void application::onInput()
    {
        _window->input();
    }

    void application::onUpdate()
    {
        _window->update();
    }

    void application::onTick()
    {
        _window->tick();
    }

    void application::onSwapbuffers()
    {
        _window->swapBuffers();
    }

    void application::onClose()
    {
    }
}