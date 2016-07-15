#include <precompiled.h>

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

class allocation
{
private:
   int _order;

public:
   allocation(int order) :
       _order(order)
   {
   }

   int getOrder() { return _order; }
};

class deallocation
{
public:
   deallocation()
   {
   }
};

class memoryOperation
{
private:
   allocation* _allocation;
   vector<deallocation*> _deallocations;

public:
   memoryOperation(allocation* allocation) :
       _allocation(allocation)
   {
   }

   void add(deallocation* deallocation)
   {
       _deallocations.push_back(deallocation);
   }

   bool hasDuplicatedDeletes()
   {
       return _deallocations.size() > 1;
   }

   int getAllocationOrder() { return _allocation->getOrder(); }
};

class heap
{
private:
   map<int, vector<memoryOperation*>> _incompletedOperations;
   map<int, vector<memoryOperation*>> _completedOperations;
   map<int, memoryOperation*> _initiatedOperations;

public:
   void addAllocation(int address, int order)
   {
       if(phi::contains(_initiatedOperations, address))
           _incompletedOperations[address].push_back(_initiatedOperations[address]);

       _initiatedOperations[address] = new memoryOperation(new allocation(order));
   }

   void addDeallocation(int address)
   {
       if (phi::contains(_initiatedOperations, address))
       {
           auto operation = _initiatedOperations[address];
           operation->add(new deallocation());

           _initiatedOperations.erase(address);
           _completedOperations[address].push_back(operation);
       }
       else
       {
           auto addressOperations = _completedOperations[address];
           auto lastAddressOperation = *(addressOperations.begin() + addressOperations.size() - 1);
           lastAddressOperation->add(new deallocation());
       }
   }

   void dump()
   {
       for (auto& pair : _initiatedOperations)
       {
           auto address = pair.first;
           auto operation = pair.second;

           debug("memory leak:\n address: " + std::to_string(address) + " order: " + std::to_string(operation->getAllocationOrder()));
       }

       for (auto& pair : _incompletedOperations)
       {
           auto address = pair.first;
           auto operations = pair.second;

           for (auto& operation : operations)
           {
               debug("memory leak:\n address: " + std::to_string(address) + " order: " + std::to_string(operation->getAllocationOrder()));
           }
       }

       for (auto& pair : _completedOperations)
       {
           auto address = pair.first;
           auto operations = pair.second;

           for (auto& operation : operations)
           {
               if (operation->hasDuplicatedDeletes())
               {
                   debug("duplicated delete\n address: " + std::to_string(address) + " order: " + std::to_string(operation->getAllocationOrder()));
               }
           }
       }
   }

} heap;

void allocate(int address)
{
   static int order = 300;
   heap.addAllocation(address, order++);
}

void deallocate(int address)
{
   heap.addDeallocation(address);
}

int main(int argc, char* args[])
{
    allocate(1);
    deallocate(1);

    allocate(2);
    allocate(2);
    deallocate(2);
    allocate(2);
    allocate(2);
    deallocate(2);

    allocate(2);
    allocate(2);

    allocate(3);
    deallocate(3);
    deallocate(3);

    allocate(1);
    deallocate(1);

    heap.dump();
    system("pause");

    // char executableFileName[FILENAME_MAX];
    // strcpy_s(executableFileName, args[0]);

    // initializeDiagnostics(executableFileName);
    // initializeGetText();
    // initializeCommandLineCommands();
    // processCommandLine(argc, args);
    // executeCommands();

    // try
    // {
    //     auto appStartInfo = applicationStartInfo(executableFileName, _resourcesPath, "");
    //     auto application = phi::application(appStartInfo);
    //     auto screen = demon::screen(L"‎(ﾉಥ益ಥ）ﾉ﻿ ┻━┻", resolution(1920, 1080));

    //     application.run(&screen);
    // }
    // catch (const phi::invalidInitializationException& exception)
    // {
    //     application::logError(exception.what());
    //     system("pause");
    // }

    // phi::input::release();

    return 0;
}