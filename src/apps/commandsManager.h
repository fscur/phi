#pragma once
#include <phi.h>
#include "appsApi.h"

#include <core/mouseEventArgs.h>
#include <core/keyboardEventArgs.h>

#include "command.h"
#include "shortcut.h"

namespace phi
{
    struct pendingCommand
    {
    public:
        command* cmd;
        std::function<void(command*)> callback;
        bool isUndo;

        pendingCommand(command* cmd, std::function<void(command*)> callback, bool isUndo) :
            cmd(cmd), callback(callback), isUndo(isUndo)
        {}
    };

    class commandsManager
    {
    private:
        std::vector<shortcut> _shortcuts;
        std::queue<pendingCommand*> _pendingCommands;
        std::stack<command*> _undo;
        std::stack<command*> _redo;
        bool _executingCommand;
        std::mutex _pendingCommandsMutex;

        std::vector<keycode> _pressedKeys;

        std::vector<shortcut> _commandShortcuts;

    private:
        void onKeyDown(phi::keyboardEventArgs* e);
        void onKeyUp(phi::keyboardEventArgs* e);
        void enqueueCommand(command* cmd, std::function<void(command*)> callback, bool isUndo);
        void startNextCommand();

    public:
        APPS_API commandsManager();
        APPS_API ~commandsManager();

        APPS_API void addShortcut(shortcut shortcut);
        APPS_API void undo();
        APPS_API void redo();
        APPS_API void executeCommand(command* cmd);
    };
}