#pragma once
#include <phi.h>
#include "appsApi.h"

#include <core/mouseEventArgs.h>
#include <core/keyboardEventArgs.h>

#include "command.h"
#include "shortcut.h"

namespace phi
{
    class commandsManager
    {
    private:
        std::vector<shortcut> _shortcuts;
        std::stack<command*> _undo;
        std::stack<command*> _redo;
        std::vector<keycode> _pressedKeys;
        std::vector<shortcut> _commandShortcuts;

    private:
        void onKeyDown(phi::keyboardEventArgs* e);
        void onKeyUp(phi::keyboardEventArgs* e);
    public:
        APPS_API commandsManager();
        APPS_API ~commandsManager();

        APPS_API void addShortcut(shortcut shortcut);
        APPS_API void undo();
        APPS_API void redo();
        APPS_API void executeCommand(command* cmd);
    };
}