#pragma once
#include <phi.h>
#include <core/mouseEventArgs.h>
#include <core/keyboardEventArgs.h>
#include <core/inputKey.h>

#include "command.h"
#include "shortcuts.h"

namespace demon
{
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
        //phi::vector<command*> _pendingCommands;
        //phi::vector<command*> _executingCommands;
        shortcuts _shortcuts;
        std::unordered_map<phi::string, std::function<command*(void)>> _commands;
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
}