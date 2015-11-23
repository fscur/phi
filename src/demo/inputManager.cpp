#include "phi/demo/inputManager.h"

#include "phi/core/globals.h"

#include "phi/demo/shortcuts.h"

inputManager::inputManager(commandsManager* commandsManager)
{
    _commandsManager = commandsManager;
    _currentController = nullptr;
}

inputManager::~inputManager()
{
}

phi::inputKey inputManager::mouseToKey(phi::mouseEventArgs* e)
{
    auto modifiers = e->isAltPressed ? PHI_ALT_PRESSED : 0;
    modifiers |= e->isCtrlPressed ? PHI_CTRL_PRESSED : 0;
    modifiers |= e->isShiftPressed ? PHI_SHIFT_PRESSED : 0;

    if (e->leftButtonPressed)
        return phi::inputKey(PHI_MOUSE_LEFT, modifiers);
    else if (e->rightButtonPressed)
        return phi::inputKey(PHI_MOUSE_RIGHT, modifiers);
    else if (e->middleButtonPressed)
        return phi::inputKey(PHI_MOUSE_MIDDLE, modifiers);
}

phi::inputKey inputManager::keyboardToKey(phi::keyboardEventArgs* e)
{
    auto modifiers = e->isAltPressed ? PHI_ALT_PRESSED : 0;
    modifiers |= e->isCtrlPressed ? PHI_CTRL_PRESSED : 0;
    modifiers |= e->isShiftPressed ? PHI_SHIFT_PRESSED : 0;
    return phi::inputKey(e->key, modifiers);
}

bool inputManager::onMouseDown(phi::mouseEventArgs* e)
{
    auto key = mouseToKey(e);
    if (_currentController->onMouseDown(e, key))
        return true;

    return _commandsManager->onInput(key);
}

bool inputManager::onMouseMove(phi::mouseEventArgs* e)
{
    return _currentController->onMouseMove(e);
}

bool inputManager::onMouseUp(phi::mouseEventArgs* e)
{
    auto key = mouseToKey(e);
    return _currentController->onMouseUp(e, key);
}

bool inputManager::onMouseWheel(phi::mouseEventArgs* e)
{
    phi::inputKey* input;

    if (e->wheelDelta > 0)
        input = new phi::inputKey(PHI_MOUSE_WHEEL_UP, PHI_NONE);
    else
        input = new phi::inputKey(PHI_MOUSE_WHEEL_DOWN, PHI_NONE);

    auto res = _currentController->onMouseWheel(e, *input);
    DELETE(input);
    return res;
}

bool inputManager::onKeyDown(phi::keyboardEventArgs* e)
{
    auto key = keyboardToKey(e);
    if (_currentController->onKeyDown(e, key))
        return true;

    return _commandsManager->onInput(key);
}

bool inputManager::onKeyUp(phi::keyboardEventArgs* e)
{
    auto key = keyboardToKey(e);
    return _currentController->onKeyUp(e, key);
}

void inputManager::update()
{
    if (_currentController != nullptr)
        _currentController->update();
}