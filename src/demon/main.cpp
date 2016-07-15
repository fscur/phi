#include <precompiled.h>

#include <core\input.h>
#include <core\invalidInitializationException.h>
#include <core\eventHandler.h>

#include <diagnostic\stackTracer.h>
#include <diagnostic\stopwatch.h>
#include <diagnostic\platformProcLoader.h>

#include <io\path.h>

#include <application\application.h>

#include "screen.h"
#include "demonApi.h"

using namespace phi;

extern "C"
{
    API unsigned long NvOptimusEnablement = 1;
    API int AmdPowerXpressRequestHighPerformance = 1;
}

struct commandLineCommand
{
public:
    string name;
    vector<string> args;
    void(*func)(vector<string>);
    bool shouldExecute;

public:
    commandLineCommand(string name, void(*func)(vector<string>)) :
        name(name),
        args(vector<string>()),
        func(func),
        shouldExecute(false)
    {
    }
};

vector<commandLineCommand> commandLineCommands;
string _resourcesPath;
string _libraryPath;

void rpCommandFunction(vector<string> args)
{
    _resourcesPath = args[0];
}

void lpCommandFunction(vector<string> args)
{
    _libraryPath = args[0];
}

void initializeCommandLineCommands()
{
    /*
    /rp <path> resources path
    /lp <path> library path
    */

    commandLineCommands.push_back(commandLineCommand("/rp", &rpCommandFunction));
    commandLineCommands.push_back(commandLineCommand("/lp", &lpCommandFunction));
}

void processCommandLine(int argc, char* args[])
{
    size_t lastCommand = 0;
    auto foundLastCommand = false;

    for (int i = 1; i < argc; i++)
    {
        string arg(args[i]);

        bool foundCommand = false;

        for (size_t cmdIndex = 0; cmdIndex < commandLineCommands.size(); cmdIndex++)
        {
            if (arg.compare(commandLineCommands[cmdIndex].name) == 0)
            {
                commandLineCommands[cmdIndex].shouldExecute = true;
                lastCommand = cmdIndex;
                foundLastCommand = true;
                foundCommand = true;
                break;
            }
        }

        if (!foundCommand && foundLastCommand)
            commandLineCommands[lastCommand].args.push_back(arg);
    }

    return;
}

void executeCommands()
{
    auto count = commandLineCommands.size();

    for (unsigned int i = 0; i < count; i++)
        if (commandLineCommands[i].shouldExecute)
            commandLineCommands[i].func(commandLineCommands[i].args);
}

void initializeGetText()
{
    setlocale(LC_ALL, "");
    bindtextdomain("demon", "./locale");
    textdomain("demon");
}

void initializeDiagnostics(const char* executableFileName)
{
    phi::platformProcLoader::load(executableFileName);
}

int main(int argc, char* args[])
{
    char executableFileName[FILENAME_MAX];
    strcpy_s(executableFileName, args[0]);

    initializeDiagnostics(executableFileName);
    initializeGetText();
    initializeCommandLineCommands();
    processCommandLine(argc, args);
    executeCommands();

    try
    {
        auto appStartInfo = applicationStartInfo(executableFileName, _resourcesPath, "");
        auto application = phi::application(appStartInfo);
        auto screen = demon::screen(L"‎(ﾉಥ益ಥ）ﾉ﻿ ┻━┻", resolution(1920, 1080));

        application.run(&screen);
    }
    catch (const phi::invalidInitializationException& exception)
    {
        application::logError(exception.what());
        system("pause");
    }

    phi::input::release();

    return 0;
}