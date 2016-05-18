#include <precompiled.h>

#include <core\input.h>

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
            auto undoCmd = _undo.top();
            _undo.pop();
            safeDelete(undoCmd);
        }

        while (_redo.size())
        {
            auto redoCmd = _redo.top();
            _redo.pop();
            safeDelete(redoCmd);
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
            _undo.push(cmd);

            while (!_redo.empty())
                _redo.pop();
        }

        cmd->execute();
    }

    void commandsManager::onKeyDown(phi::keyboardEventArgs* e)
    {
        size_t pressedKeysCount = _pressedKeys.size();

        if (pressedKeysCount == 0u || _pressedKeys.back() != e->key)
        {
            _pressedKeys.push_back(e->key);
            pressedKeysCount++;
        }

        auto shortcutsCopy = _shortcuts;
        size_t i = 0u;

        while (i < pressedKeysCount)
        {
            for (size_t j = 0u; j < shortcutsCopy.size(); ++j)
            {
                if (i >= shortcutsCopy[j].keys.size() || shortcutsCopy[j].keys[i] != _pressedKeys[i])
                    shortcutsCopy.erase(shortcutsCopy.begin() + j--);
            }
            ++i;
        }

        for (size_t j = 0u; j < shortcutsCopy.size(); ++j)
            if (shortcutsCopy[j].keys.size() == i)
                executeCommand(shortcutsCopy[j].commandFunc());
    }

    void commandsManager::onKeyUp(phi::keyboardEventArgs* e)
    {
        auto index = std::find(_pressedKeys.begin(), _pressedKeys.end(), e->key);
        while (index != _pressedKeys.end())
            index = _pressedKeys.erase(index);
    }
}