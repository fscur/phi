#include <vector>

#include "phi/demo/screen.h"

#include "phi/demo/application.h"
#include "phi/core/globals.h"

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
phi::scenesManager* _scenesManager;
phi::sceneRenderer* _renderer;
std::string _resourcesPath;

void rCommandFunction(std::vector<std::string> args)
{
    std::string r = args[0];

    if (r == "0")
       _renderer = phi::scenesManager::basicRenderer;
    else if (r == "1")
        _renderer = phi::scenesManager::fsRenderer;
    else if (r == "2")
        _renderer = phi::scenesManager::dsRenderer;
}

void rpCommandFunction(std::vector<std::string> args)
{
    _resourcesPath = args[0];
}

void initCommandLineCommands()
{
    /***********

    /r <0..2>           Set renderer
    /rp <path>          Set resources path

    ***********/
    commandLineCommands.push_back(commandLineCommand("/r", &rCommandFunction));
    commandLineCommands.push_back(commandLineCommand("/rp", &rpCommandFunction));

}

void processCommandLine(int argc, char* args[])
{
    int curCommand = -1;

    for (int i = 1; i < argc; i++)
    {
        std::string arg(args[i]);

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

    return;
}

void executeCommands()
{
    for (unsigned int i = 0; i < commandLineCommands.size(); i++)
    {
        if (commandLineCommands[i].shouldExecute)
        {
            LOG("command executed: " << commandLineCommands[i].name);

            commandLineCommands[i].func(commandLineCommands[i].args);

            for (unsigned int j = 0; j < commandLineCommands[i].args.size(); j++)
            {
                LOG("arg[" << j << "]: " <<commandLineCommands[i].args[j]);
            }
        }
    }
}

std::string getDirPath(std::string filePath)
{
    std::string exePath = filePath;

    int index = filePath.find_last_of('\\');
    return exePath.substr(0, index);
}

int main(int argc, char* args[])
{
    application *app = new application();

    std::string exePath = std::string(args[0]);
    std::string path = getDirPath(exePath);
    application::path = path;
    application::exePath = exePath;

    _scenesManager = phi::scenesManager::get();

    initCommandLineCommands();
    processCommandLine(argc, args);
    executeCommands();

    auto mainScreen = new screen();

    if (!_resourcesPath.empty())
        mainScreen->setResourcesPath(_resourcesPath);

    mainScreen->initialize(path);
    _scenesManager->setSceneRenderer(_renderer);

    app->run(mainScreen);

    delete mainScreen;
    delete app;

    return 0;
}