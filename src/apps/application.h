#pragma once
#include <phi.h>
#include "appsApi.h"

#include <diagnostics\logger.h>

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
        APPS_API static string executableFileName;
        APPS_API static string path;
        APPS_API static string resourcesPath;
        APPS_API static string libraryPath;
        APPS_API static uint framesPerSecond;
        APPS_API static double millisecondsPerFrame;

    private:
        bool _running;
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
        APPS_API application(const applicationStartInfo& startInfo);
        APPS_API ~application();

        APPS_API void run(window* window);
        APPS_API static void logError(string message);
        APPS_API static void logWarning(string message);
        APPS_API static void logInfo(string message);
    };
}