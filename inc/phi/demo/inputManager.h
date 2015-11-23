#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#include "phi/core/mouseEventArgs.h"
#include "phi/core/keyboardEventArgs.h"
#include "phi/core/inputKey.h"
#include "phi/demo/cameraController.h"
#include "phi/demo/commandsManager.h"

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

    void setCurrentCameraController(cameraController* value) { _currentController = value; }

    bool onMouseDown(phi::mouseEventArgs* e);
    bool onMouseMove(phi::mouseEventArgs* e);
    bool onMouseUp(phi::mouseEventArgs* e);
    bool onMouseWheel(phi::mouseEventArgs* e);
    bool onKeyDown(phi::keyboardEventArgs* e);
    bool onKeyUp(phi::keyboardEventArgs* e);

    void update();
};

#endif