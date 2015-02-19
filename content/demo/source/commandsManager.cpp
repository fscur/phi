#include "commandsManager.h"
#include "selectCommand.h"
#include "panCommand.h"
#include "rotateCommand.h"
#include "zoomCommand.h"

namespace phi
{
    commandsManager::commandsManager()
    {
        _isMouseDown = false;
        _isCtrlPressed = false;
        _isShiftPressed = false;
        _isAltPressed = false;

        _commandInfo = new commandInfo();
        _commands[inputKey(PHI_MOUSE_LEFT, NONE)] = [] () -> command* { return new selectCommand(); };
        _commands[inputKey(PHI_MOUSE_MIDDLE, NONE)] = [] () -> command* { return new panCommand(); };
        _commands[inputKey(PHI_MOUSE_RIGHT, NONE)] = [] () -> command* { return new rotateCommand(); };
        _commands[inputKey(PHI_MOUSE_WHEEL_UP, NONE)] = [] () -> command* { return new zoomCommand(true); };
        _commands[inputKey(PHI_MOUSE_WHEEL_DOWN, NONE)] = [] () -> command* { return new zoomCommand(false); };
        std::fill_n(_states, TOTAL_KEYS, NONE);
        _modifiers = NONE;
    }

    commandsManager::~commandsManager()
    {
        delete(_commandInfo);

        while(_pendingCommands.size() > 0)
        {
            command* c = _pendingCommands[0];
            _pendingCommands.erase(_pendingCommands.begin());
            DELETE(c);
        }
    }

    void commandsManager::onBeginInput(phi::size<unsigned int> viewportSize)
    {
        std::fill_n(_states, TOTAL_KEYS, NONE);
        _modifiers = NONE;

        if (_commandInfo == nullptr)
            _commandInfo = new commandInfo();

        if (_isMouseDown)
        {
            if (_mouseDownEventArgs.leftButtonPressed)
                _states[PHI_MOUSE_LEFT] |= PRESSED;

            if (_mouseDownEventArgs.middleButtonPressed)
                _states[PHI_MOUSE_MIDDLE] |= PRESSED;

            if (_mouseDownEventArgs.rightButtonPressed)
                _states[PHI_MOUSE_RIGHT] |= PRESSED;
        }

        if (_isCtrlPressed)
            _modifiers |= CTRL_PRESSED;

        if (_isShiftPressed)
            _modifiers |= SHIFT_PRESSED;

        if (_isAltPressed)
            _modifiers |= ALT_PRESSED;

        _commandInfo->viewportSize = viewportSize;
    }

    bool commandsManager::onMouseDown(mouseEventArgs e)
    {
        if (e.leftButtonPressed)
            _states[PHI_MOUSE_LEFT] |= DOWN;

        if (e.middleButtonPressed)
            _states[PHI_MOUSE_MIDDLE] |= DOWN;

        if (e.rightButtonPressed)
            _states[PHI_MOUSE_RIGHT] |= DOWN;

        _isMouseDown = true;
        _mouseDownEventArgs = e;
        _commandInfo->mouseDownPos = glm::vec2(e.x, e.y);
        _commandInfo->mousePos = glm::vec2(e.x, e.y);

        return true;
    }

    bool commandsManager::onMouseMove(mouseEventArgs e)
    {
        _commandInfo->lastMousePos = _commandInfo->mousePos;
        _commandInfo->mousePos = glm::vec2(e.x, e.y);

        return true;
    }

    bool commandsManager::onMouseUp(mouseEventArgs e)
    {
        if (!_isMouseDown)
            return false;

        int key;
        if (e.leftButtonPressed)
            _states[PHI_MOUSE_LEFT] |= UP;

        if (e.middleButtonPressed)
            _states[PHI_MOUSE_MIDDLE] |= UP;

        if (e.rightButtonPressed)
            _states[PHI_MOUSE_RIGHT] |= UP;

        _commandInfo->mousePos = glm::vec2(e.x, e.y);

        _isMouseDown = false;

        _mouseDownEventArgs.leftButtonPressed = false;
        _mouseDownEventArgs.middleButtonPressed = false;
        _mouseDownEventArgs.rightButtonPressed = false;

        return true;
    }

    bool commandsManager::onMouseWheel(mouseEventArgs e)
    {
        int key;
        if (e.wheelDelta > 0)
            key = PHI_MOUSE_WHEEL_UP;
        else
            key = PHI_MOUSE_WHEEL_DOWN;

        _commandInfo->wheelDelta = e.wheelDelta;
        _commandInfo->mousePos = glm::vec2(e.x, e.y);

        _states[key] |= DOWN;

        return true;
    }

    bool commandsManager::onKeyDown(keyboardEventArgs e)
    {
        _isCtrlPressed = e.isCtrlPressed;
        _isShiftPressed = e.isShiftPressed;
        _isAltPressed = e.isAltPressed;

        if (_isCtrlPressed)
            _modifiers |= CTRL_PRESSED;

        if (_isShiftPressed)
            _modifiers |= SHIFT_PRESSED;

        if(_isAltPressed)
            _modifiers |= ALT_PRESSED;

        return true;
    }

    bool commandsManager::onKeyUp(keyboardEventArgs e)
    {
        _isCtrlPressed = false;
        _isShiftPressed = false;
        _isAltPressed = false;

        return true;
    }

    void commandsManager::onEndInput()
    {
        for (auto i : _commands)
        {
            if ((_states[i.first.key] & DOWN) == DOWN && _modifiers == i.first.modifiers)
            {
                auto f = i.second;
                if (f != nullptr)
                {
                    command* command = f();
                    command->info = _commandInfo;
                    _pendingCommands[i.first.key] = command;
                    command->init();
                    break;
                }
            }

            if ((_states[i.first.key] & DOWN) == NONE && (_states[i.first.key] & PRESSED) == NONE)
                _executingCommands.erase(i.first.key);
        }
    }

    void commandsManager::update()
    {
        std::unordered_map<int, command*>::iterator it = _pendingCommands.begin();
        while(it != _pendingCommands.end())
        {
            command* c = it->second;
            if (c->canExecute())
            {
                _executingCommands[it->first] = c;
                it = _pendingCommands.erase(it);
            }
            else
                it++;
        }

        for (auto i : _executingCommands)
        {
            command* c = i.second;
            c->info = _commandInfo;
            c->update();
        }
    }
}