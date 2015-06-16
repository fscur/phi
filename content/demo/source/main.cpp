#include <vector>

#include "application.h"
#include "globals.h"
#include "screen.h"

#include "testSceneRenderer.h"

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

        for (int cmdIndex = 0; cmdIndex < commandLineCommands.size(); cmdIndex++)
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
    for (int i = 0; i < commandLineCommands.size(); i++)
    {
        if (commandLineCommands[i].shouldExecute)
        {
            LOG("command executed: " << commandLineCommands[i].name);

            commandLineCommands[i].func(commandLineCommands[i].args);

            for (int j = 0; j < commandLineCommands[i].args.size(); j++)
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

glm::vec3 refract(float n, glm::vec3 I, glm::vec3 N)
{
    auto cosI = glm::dot(N, I);
    auto sinT2 = n * n * (1.0 - cosI * cosI);

    if (sinT2 > 1.0)
        return glm::vec3(0.0); // or genDType(0.0)
    else
        return n * I - (n * cosI + (float)glm::sqrt(1.0 - sinT2)) * N;
}

int main(int argc, char* args[])
{
    auto i = glm::normalize(glm::vec3(1.0, 1.0, 0.0));
    auto n = glm::normalize(glm::vec3(-1.0, 0.0, 0.0));
    auto a = refract(1.5, i, n); 

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

    phi::testSceneRenderer* renderer = new phi::testSceneRenderer(mainScreen->getSize());
    phi::scenesManager::get()->setSceneRenderer(renderer);
    mainScreen->setSceneRenderer(renderer);

    app->run(mainScreen);

    delete mainScreen;
    delete app;

    return 0;
}