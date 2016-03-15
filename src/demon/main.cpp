#include <precompiled.h>
#include "demon.h"

#include <io\path.h>
#include <apps\application.h>
#include <apps\invalidInitializationException.h>
#include <demon\screen.h>

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

void initCommandLineCommands()
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
    int curCommand = -1;

#if _DEBUG
    std::cout << "cmd line: ";
#endif

    for (int i = 1; i < argc; i++)
    {
        string arg(args[i]);

#if _DEBUG
        std::cout << args[i] << " ";
#endif
        vector<string>::iterator it;

        bool foundCommand = false;

        for (unsigned int cmdIndex = 0; cmdIndex < commandLineCommands.size(); cmdIndex++)
        {
            if (arg.compare(commandLineCommands[cmdIndex].name) == 0)
            {
                curCommand = cmdIndex;
                commandLineCommands[curCommand].shouldExecute = true;
                foundCommand = true;
                break;
            }
        }

        if (!foundCommand && curCommand > -1)
            commandLineCommands[curCommand].args.push_back(arg);
    }
#if _DEBUG
    std::cout << std::endl;
#endif
    return;
}

void executeCommands()
{
    auto count = commandLineCommands.size();

    for (unsigned int i = 0; i < count; i++)
        if (commandLineCommands[i].shouldExecute)
            commandLineCommands[i].func(commandLineCommands[i].args);
}

int debugQuit(string msg)
{
#ifdef _DEBUG
    application::logError(msg);
    system("pause");
#endif
    return -1;
}

int main(int argc, char* args[])
{
    string exeFileName = string(args[0]);

    initCommandLineCommands();
    processCommandLine(argc, args);
    executeCommands();

    applicationStartInfo appStartInfo;
    appStartInfo.exeFileName = exeFileName;
    appStartInfo.resourcesPath = _resourcesPath;
    appStartInfo.libraryPath = _libraryPath;

    application app(appStartInfo);

    if (!path::exists(application::resourcesPath))
        return debugQuit("Resources path not found. [" + application::resourcesPath + "]");

    if (!path::exists(application::libraryPath))
        return debugQuit("Library path not found. [" + application::libraryPath + "]");

    try
    {
        auto mainScreen = new demon::screen("?", 1024, 768);
        debug("runnning.");
        app.run(mainScreen);
        safeDelete(mainScreen);
    }
    catch (const phi::invalidInitializationException& exception)
    {
        application::logError(exception.what());
        system("pause");
    }

    return 0;
}