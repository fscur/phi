#pragma once
#include <phi.h>
#include "applicationApi.h"

#include <diagnostic\logger.h>

#include "window.h"

namespace phi
{
    struct applicationStartInfo
    {
        applicationStartInfo(string executableFileName, string resourcesPath, string libraryPath) :
            executableFileName(executableFileName),
            resourcesPath(resourcesPath),
            libraryPath(libraryPath)
        {
        }

        string executableFileName;
        string resourcesPath;
        string libraryPath;
    };

    class application
    {
    private:
        static bool _initialized;

    public:
        APPLICATION_API static string executableFileName;
        APPLICATION_API static string path;
        APPLICATION_API static string resourcesPath;
        APPLICATION_API static string libraryPath;
        APPLICATION_API static uint framesPerSecond;
        APPLICATION_API static double millisecondsPerFrame;

    private:
        window* _window;
        static logger _logger;

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
        APPLICATION_API application(const applicationStartInfo& startInfo);
        APPLICATION_API ~application();

        APPLICATION_API void run(window* window);
        APPLICATION_API static void logError(string message);
        APPLICATION_API static void logWarning(string message);
        APPLICATION_API static void logInfo(string message);
    };
}