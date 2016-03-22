#include <precompiled.h>
#include "application.h"
#include <core\time.h>
#include <io\path.h>

namespace phi
{
    bool application::_initialized = false;
    string application::exeFileName = string();
    string application::path = string();
    string application::resourcesPath = string();
    string application::libraryPath = string();
    uint application::framesPerSecond = 0;
    double application::millisecondsPerFrame = 0.0;

    application::application(const applicationStartInfo& startInfo) :
        _running(false),
        _window(nullptr)
    {
        assert(!_initialized);

        exeFileName = startInfo.exeFileName;
        path = phi::path::getDirectoryFullName(exeFileName);
        resourcesPath = startInfo.resourcesPath;
        libraryPath = startInfo.libraryPath;

        if (resourcesPath.empty())
            resourcesPath = path + "\\resources\\";

        if (libraryPath.empty())
            libraryPath = path + "\\library\\";

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

            onClear();
            onRender();
            onSwapbuffers();
            onInput();
            onUpdate();

            time::update();
            
            if (time::totalSeconds - timer > 1.0)
            {
                timer += 1.0;
                framesPerSecond = frames;
                frames = 0;
                
                onTick();
            }

            frames++;

            if (_window->closed)
            {
                onClose();
                _running = false;
            }
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
        _window->close();
    }
}