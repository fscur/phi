#include <precompiled.h>

#include <core\input.h>

#include "commandsManager.h"

namespace phi
{
    commandsManager::commandsManager()
    {
        phi::input::keyDown->assign(std::bind(&commandsManager::onKeyDown, this, std::placeholders::_1));
        phi::input::keyUp->assign(std::bind(&commandsManager::onKeyUp, this, std::placeholders::_1));
        _executingCommand = false;
    }

    commandsManager::~commandsManager()
    {
    }

    void commandsManager::addShortcut(shortcut shortcut)
    {
        _shortcuts.push_back(shortcut);
    }

    void commandsManager::enqueueCommand(command* cmd, std::function<void(command*)> callback, bool isUndo)
    {
        _pendingCommandsMutex.lock();
        _pendingCommands.push(new pendingCommand(cmd, callback, isUndo));
        _pendingCommandsMutex.unlock();
    }

    void commandsManager::startNextCommand()
    {
        _pendingCommandsMutex.lock();
        if (!_executingCommand && !_pendingCommands.empty())
        {
            _executingCommand = true;
            pendingCommand* pendingCmd = _pendingCommands.front();
            if (!pendingCmd->isUndo)
                pendingCmd->cmd->startAsync(pendingCmd->callback);
            else
                pendingCmd->cmd->startUndoAsync(pendingCmd->callback);
            _pendingCommands.pop();
        }
        _pendingCommandsMutex.unlock();
    }

    void commandsManager::undo()
    {
        if (_undo.empty())
            return;

        auto cmd = _undo.top();
        _undo.pop();

        auto callback = [&](command* c)
        {
            _redo.push(c);
            _executingCommand = false;
            startNextCommand();
        };
        enqueueCommand(cmd, callback, true);
    }

    void commandsManager::redo()
    {
        if (_redo.empty())
            return;

        auto cmd = _redo.top();
        _redo.pop();

        auto callback = [&](command* c)
        {
            _undo.push(c);
            _executingCommand = false;
            startNextCommand();
        };
        enqueueCommand(cmd, callback, false);
    }

    void commandsManager::executeCommand(command* cmd)
    {
        auto callback = [&](command* c)
        {
            if (c->getIsUndoable())
            {
                _undo.push(c);

                while (!_redo.empty())
                    _redo.pop();
            }

            _executingCommand = false;
            startNextCommand();
        };
        enqueueCommand(cmd, callback, false);
        startNextCommand();
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
        size_t shortcutsCopyCount = shortcutsCopy.size();
        size_t i = 0u;

        while (i < pressedKeysCount)
        {
            for (size_t j = 0u; j < shortcutsCopyCount; ++j)
            {
                if (i >= shortcutsCopy[j].keys.size() || 
                    shortcutsCopy[j].keys[i] != _pressedKeys[i])
                    shortcutsCopy.erase(shortcutsCopy.begin() + j--);
            }
            ++i;
        }

        shortcutsCopyCount = shortcutsCopy.size();

        for (size_t j = 0u; j < shortcutsCopyCount; ++j)
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