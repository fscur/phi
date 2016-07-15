#pragma once
#include <phi.h>
#include "applicationApi.h"

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
        APPLICATION_API commandsManager();
        APPLICATION_API ~commandsManager();

        APPLICATION_API void addShortcut(shortcut shortcut);
        APPLICATION_API void undo();
        APPLICATION_API void redo();
        APPLICATION_API void executeCommand(command* cmd);
    };
}