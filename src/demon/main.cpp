#include <precompiled.h>
#include <apps\application.h>
#include <demon\screen.h>

struct commandLineCommand
{
public:
    phi::string name;
    phi::vector<phi::string> args;
    void(*func)(phi::vector<phi::string>);
    bool shouldExecute;

public:
    commandLineCommand(phi::string name, void(*func)(phi::vector<phi::string>))
    {
        this->name = name;
        shouldExecute = false;
        this->func = func;
    }
};

phi::vector<commandLineCommand> commandLineCommands;
phi::string _resourcesPath;
phi::string _libraryPath;

void rpCommandFunction(phi::vector<phi::string> args)
{
    _resourcesPath = args[0];
}

void lpCommandFunction(phi::vector<phi::string> args)
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
        phi::string arg(args[i]);

#if _DEBUG
        std::cout << args[i] << " ";
#endif

        phi::vector<phi::string>::iterator it;

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

phi::string getDirPath(phi::string filePath)
{
    phi::string exePath = filePath;

    auto index = filePath.find_last_of('\\');
    return exePath.substr(0, index);
}

int main(int argc, char* args[])
{
    phi::string exeFileName = phi::string(args[0]);
    phi::application app(exeFileName);

    phi::string path = getDirPath(exeFileName);

    phi::debug("application path: " + path + ".");

    initCommandLineCommands();
    processCommandLine(argc, args);
    executeCommands();

    auto mainScreen = new demon::screen("?", 1024, 768);

    if (!_resourcesPath.empty())
        mainScreen->setResourcesPath(_resourcesPath);
    else
        mainScreen->setResourcesPath(path + "\\resources\\");

    if (!_libraryPath.empty())
        mainScreen->setLibraryPath(_libraryPath);
    else
        mainScreen->setLibraryPath(path + "\\library\\");
    
    phi::debug("runnning.");
    app.run(mainScreen);

    phi::safeDelete(mainScreen);
    
    return 0;
}