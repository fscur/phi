#include <precompiled.h>
#include "selectionMouseController.h"

#include <core\input.h>
#include <core\mesh.h>
#include <core\multiCommand.h>

#include <rendering\pickingFramebuffer.h>

#include "pickingId.h"
#include "selectSceneObjectCommand.h"
#include "unselectSceneObjectCommand.h"

namespace phi
{
    selectionMouseController::selectionMouseController(meshLayerBehaviour* meshLayerBehaviour, commandsManager* commandsManager) :
        _meshLayerBehaviour(meshLayerBehaviour),
        _commandsManager(commandsManager),
        _isCtrlPressed(false)
    {
    }

    void selectionMouseController::onKeyDown(keyboardEventArgs * e)
    {
        _isCtrlPressed = e->key == PHIK_CTRL;
    }

    void selectionMouseController::onKeyUp(keyboardEventArgs * e)
    {
        if (e->key == PHIK_CTRL)
            _isCtrlPressed = false;
    }

    void selectionMouseController::onMouseDown(phi::mouseEventArgs* e)
    {
        if (e->leftButtonPressed)
        {
            auto idOnMousePosition = pickingFramebuffer::pick(e->x, e->y);

            auto clickComponent = pickingId::get(idOnMousePosition);
            auto selectedObjects = _meshLayerBehaviour->getSelectedObjects();

            if (!clickComponent)
            {
                auto hasSelectedObjects = selectedObjects.size() > 0u;
                if (hasSelectedObjects)
                    _commandsManager->executeCommand(
                        new unselectSceneObjectCommand(selectedObjects));
            }
            else
            {
                auto node = clickComponent->getNode();
                auto selectedMesh = node->getComponent<mesh>();

                if (selectedMesh)
                {
                    _commandsManager->executeCommand(
                        new selectSceneObjectCommand(selectedMesh->getNode(), selectedObjects));
                }

                clickComponent->onClick();
            }
        }
    }
}