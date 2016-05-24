#pragma once
#include <phi.h>
#include "appsApi.h"

#include <core\command.h>
#include <core\mouseEventArgs.h>
#include <core\keyboardEventArgs.h>
#include "shortcut.h"

namespace phi
{
    class commandsManager
    {
    private:
        vector<shortcut> _shortcuts;
        std::stack<command*> _undo;
        std::stack<command*> _redo;
        vector<keycode> _pressedKeys;
        vector<shortcut> _commandShortcuts;

    private:
        void onKeyDown(phi::keyboardEventArgs* e);
        void onKeyUp(phi::keyboardEventArgs* e);
        void clearRedo();
    public:
        APPS_API commandsManager();
        APPS_API ~commandsManager();

        APPS_API void addShortcut(shortcut shortcut);
        APPS_API void undo();
        APPS_API void redo();
        APPS_API void executeCommand(command* cmd);
    };
}