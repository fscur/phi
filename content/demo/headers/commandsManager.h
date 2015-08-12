#ifndef _PHI_COMMANDS_MANAGER_H_
#define _PHI_COMMANDS_MANAGER_H_

#include <glm\glm.hpp>
#include <unordered_map>
#include <queue>
#include <stack>
#include <functional>
#include <mutex>
#include "command.h"
#include "mouseEventArgs.h"
#include "keyboardEventArgs.h"
#include "inputKey.h"
#include "shortcuts.h"

struct pendingCommand
{
public:
    command* cmd;
    std::function<void(command*)> callback;
    bool isUndo;

    pendingCommand(command* cmd, std::function<void(command*)> callback, bool isUndo) :
        cmd(cmd), callback(callback), isUndo(isUndo)
    {
    }
};

class commandsManager
{
private:
    //commandInfo* _commandInfo;
    //std::vector<command*> _pendingCommands;
    //std::vector<command*> _executingCommands;
    shortcuts _shortcuts;
    std::unordered_map<std::string, std::function<command*(void)>> _commands;
    std::queue<pendingCommand*> _pendingCommands;
    std::stack<command*> _undo;
    std::stack<command*> _redo;
    bool _executingCommand;
    std::mutex _pendingCommandsMutex;

private:
    void enqueueCommand(command* cmd, std::function<void(command*)> callback, bool isUndo);
    void startNextCommand();

public:
    commandsManager();
    ~commandsManager();

    void undo();
    void redo();
    void executeCommand(command* cmd);
    void update();
    bool onInput(phi::inputKey key);
};

#endif