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

void rCommandFunction(std::vector<std::string> args)
{
    std::string r = args[0];

    if (r == "0")
        phi::scenesManager::get()->setSceneRenderer(phi::scenesManager::basicRenderer);
    else if (r == "1")
        phi::scenesManager::get()->setSceneRenderer(phi::scenesManager::fsRenderer);
    else if (r == "2")
        phi::scenesManager::get()->setSceneRenderer(phi::scenesManager::dsRenderer);
}

void initCommandLineCommands()
{
    commandLineCommands.push_back(commandLineCommand("/r", &rCommandFunction));
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

    screen *mainScreen = new screen();
    mainScreen->initialize(path);

    initCommandLineCommands();
    processCommandLine(argc, args);
    executeCommands();
    app->run(mainScreen);

    delete mainScreen;
    delete app;

    return 0;
}