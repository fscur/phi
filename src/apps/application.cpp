#include <precompiled.h>
#include "application.h"

#include <io\path.h>

#include <core\time.h>

#include "invalidInitializationException.h"

namespace phi
{
    bool application::_initialized = false;
    string application::executableFileName = string();
    string application::path = string();
    string application::resourcesPath = string();
    string application::libraryPath = string();
    uint application::framesPerSecond = 0;
    double application::millisecondsPerFrame = 0.0;
    logger application::_logger = phi::logger();

    application::application(const applicationStartInfo& startInfo) :
        _running(false),
        _window(nullptr)
    {
        assert(!_initialized);
        executableFileName = startInfo.executableFileName;
        path = phi::path::getDirectoryFullName(executableFileName);
        resourcesPath = startInfo.resourcesPath;
        libraryPath = startInfo.libraryPath;

        if (resourcesPath.empty())
            resourcesPath = path + "\\resources\\";
        if (libraryPath.empty())
            libraryPath = path + "\\library\\";

        if (!path::exists(resourcesPath))
            throw invalidInitializationException("Resources path not found. [" + resourcesPath + "]");
        if (!path::exists(libraryPath))
            throw invalidInitializationException("Library path not found. [" + libraryPath + "]");

        _initialized = true;
        time::start();
    }

    application::~application()
    {
    }

    void application::run(window* window)
    {
        _window = window;
        _running = true;

        onInit();

        auto timer = 0.0;
        auto frames = 0u;
        while (_running)
        {
            millisecondsPerFrame = time::deltaSeconds * 1000;

            onClear();
            onRender();
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
            onSwapbuffers();

            if (_window->closed)
            {
                onClose();
                _running = false;
            }
        }
    }

    void application::logError(string message)
    {
        _logger.logError(message);
    }

    void application::logWarning(string message)
    {
        _logger.logWarning(message);
    }

    void application::logInfo(string message)
    {
        _logger.logInfo(message);
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