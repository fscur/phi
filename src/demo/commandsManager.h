#pragma once

#include <core/mouseEventArgs.h>
#include <core/keyboardEventArgs.h>
#include <core/inputKey.h>

#include "command.h"
#include "shortcuts.h"

#include <glm\glm.hpp>

#include <unordered_map>
#include <queue>
#include <stack>
#include <functional>
#include <mutex>

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