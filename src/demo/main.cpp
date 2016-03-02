#include <core/globals.h>

#include "screen.h"
#include "application.h"

#include <vector>

struct commandLineCommand
{
public:
    std::string name;
    std::vector<std::string> args;
    void (*func)(std::vector<std::string>);
    bool shouldExecute;

public:
    commandLineCommand(std::string name, void (*func)(std::vector<std::string>))
    {
        this->name = name;
        shouldExecute = false;
        this->func = func;
    }
};

std::vector<commandLineCommand> commandLineCommands;
std::string _resourcesPath;
std::string _libraryPath;

void rpCommandFunction(std::vector<std::string> args)
{
    _resourcesPath = args[0];
}

void lpCommandFunction(std::vector<std::string> args)
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
        std::string arg(args[i]);

#if _DEBUG
        std::cout << args[i] << " ";
#endif

        std::vector<std::string>::iterator it;

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

std::string getDirPath(std::string filePath)
{
    std::string exePath = filePath;

    auto index = filePath.find_last_of('\\');
    return exePath.substr(0, index);
}

int main(int argc, char* args[])
{
    application *app = new application();

    std::string exePath = std::string(args[0]);
    std::string path = getDirPath(exePath);
    application::path = path;
    application::exePath = exePath;

    phi::debug("application path: " + path + ".");

    initCommandLineCommands();
    processCommandLine(argc, args);
    executeCommands();

    auto mainScreen = new screen();

    if (!_resourcesPath.empty())
        mainScreen->setResourcesPath(_resourcesPath);
    else
        mainScreen->setResourcesPath(path + "\\resources\\");

    if (!_libraryPath.empty())
        mainScreen->setLibraryPath(_libraryPath);
    else
        mainScreen->setLibraryPath(path + "\\library\\");

    mainScreen->initialize(path);

    phi::debug("runnning.");
    app->run(mainScreen);

    phi::safeDelete(mainScreen);
    phi::safeDelete(app);

    return 0;
}