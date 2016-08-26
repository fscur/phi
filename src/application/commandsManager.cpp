#include <precompiled.h>

#include <input\input.h>

#include "commandsManager.h"

namespace phi
{
    commandsManager::commandsManager()
    {
        input::keyDown->assign(std::bind(&commandsManager::onKeyDown, this, std::placeholders::_1));
        input::keyUp->assign(std::bind(&commandsManager::onKeyUp, this, std::placeholders::_1));
    }

    commandsManager::~commandsManager()
    {
        while (_undo.size())
        {
            safeDelete(_undo.top());
            _undo.pop();
        }

        while (_redo.size())
        {
            safeDelete(_redo.top());
            _redo.pop();
        }
    }

    void commandsManager::clearRedo()
    {
        while (!_redo.empty())
        {
            safeDelete(_redo.top());
            _redo.pop();
        }
    }

    void commandsManager::addShortcut(shortcut shortcut)
    {
        _shortcuts.push_back(shortcut);
    }

    void commandsManager::undo()
    {
        if (_undo.empty())
            return;

        auto cmd = _undo.top();
        _undo.pop();

        _redo.push(cmd);
        cmd->executeUndo();
    }

    void commandsManager::redo()
    {
        if (_redo.empty())
            return;

        auto cmd = _redo.top();
        _redo.pop();

        _undo.push(cmd);
        cmd->execute();
    }

    void commandsManager::executeCommand(command* cmd)
    {
        if (cmd->getIsUndoable())
        {
            cmd->execute();
            _undo.push(cmd);
            clearRedo();
        }
        else
        {
            cmd->execute();
            safeDelete(cmd);
        }
    }

    void commandsManager::onKeyDown(phi::keyboardEventArgs* e)
    {
        auto pressedKeysCount = _pressedKeys.size();

        if (pressedKeysCount == 0u || _pressedKeys.back() != e->key)
        {
            _pressedKeys.push_back(e->key);
            pressedKeysCount++;
        }

        auto shortcutsCopy = _shortcuts;
        auto i = 0u;

        while (i < pressedKeysCount)
        {
            for (auto j = 0u; j < shortcutsCopy.size(); ++j)
            {
                if (i >= shortcutsCopy[j].keys.size() || shortcutsCopy[j].keys[i] != _pressedKeys[i])
                    shortcutsCopy.erase(shortcutsCopy.begin() + j--);
            }
            ++i;
        }

        for (auto j = 0u; j < shortcutsCopy.size(); ++j)
            if (shortcutsCopy[j].keys.size() == i)
                executeCommand(shortcutsCopy[j].commandFunc());
    }

    void commandsManager::onKeyUp(phi::keyboardEventArgs* e)
    {
        phi::removeIfContains(_pressedKeys, e->key);
    }
}