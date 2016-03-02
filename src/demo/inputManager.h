#pragma once
#include <precompiled.h>
#include <core/mouseEventArgs.h>
#include <core/keyboardEventArgs.h>
#include <core/inputKey.h>

#include "cameraController.h"
#include "commandsManager.h"

class inputManager
{
private:
    cameraController* _currentController;
    commandsManager* _commandsManager;

private:
    phi::inputKey mouseToKey(phi::mouseEventArgs* e);
    phi::inputKey keyboardToKey(phi::keyboardEventArgs* e);

public:
    inputManager(commandsManager* commandsManager);
    ~inputManager();

    void setCurrentCameraController(cameraController* value) 
    {
        _currentController = value; 
    }

    bool onMouseDown(phi::mouseEventArgs* e);
    bool onMouseMove(phi::mouseEventArgs* e);
    bool onMouseUp(phi::mouseEventArgs* e);
    bool onMouseWheel(phi::mouseEventArgs* e);
    bool onKeyDown(phi::keyboardEventArgs* e);
    bool onKeyUp(phi::keyboardEventArgs* e);

    void update();
};