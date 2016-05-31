#include <precompiled.h>
#include "application.h"
#include <core\time.h>
#include <io\path.h>

#include <core\time.h>

#include <rendering\gl.h>

#include <core\invalidInitializationException.h>

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

        onInit();

        auto timer = 0.0;
        auto frames = 0u;
        auto milliseconds = 0.0;

        while (!_window->closed)
        {
            time::update();

            onInput();
            onUpdate();
            onClear();
            onRender();
            onSwapbuffers();
            gl::SyncPipeline();

            milliseconds += time::deltaSeconds * 1000.0;
            frames++;

            if (time::totalSeconds - timer > 1.0)
            {
                timer += 1.0;

                millisecondsPerFrame = milliseconds / static_cast<double>(frames);
                framesPerSecond = frames;

                frames = 0;
                milliseconds = 0.0;

                onTick();
            }
        }

        onClose();
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