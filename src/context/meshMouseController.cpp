#include <precompiled.h>
#include "meshMouseController.h"

#include <core\input.h>
#include <core\mesh.h>
#include <core\multiCommand.h>

#include <rendering\pickingFramebuffer.h>

#include "pickingId.h"
#include "selectSceneObjectCommand.h"
#include "unselectSceneObjectCommand.h"

namespace phi
{
    meshMouseController::meshMouseController(meshLayerBehaviour* meshLayerBehaviour, commandsManager* commandsManager) :
        _meshLayerBehaviour(meshLayerBehaviour),
        _commandsManager(commandsManager),
        _isCtrlPressed(false)
    {
    }

    void meshMouseController::onKeyDown(keyboardEventArgs * e)
    {
        _isCtrlPressed = e->key == PHIK_CTRL;
    }

    void meshMouseController::onKeyUp(keyboardEventArgs * e)
    {
        if (e->key == PHIK_CTRL)
            _isCtrlPressed = false;
    }

    void meshMouseController::onMouseDown(phi::mouseEventArgs* e)
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
            }
        }
    }

    void meshMouseController::onMouseMove(phi::mouseEventArgs* e)
    {
    }

    void meshMouseController::onMouseUp(phi::mouseEventArgs* e)
    {
    }

    void meshMouseController::onMouseWheel(phi::mouseEventArgs* e)
    {
    }

    void meshMouseController::update()
    {
    }
}