#include <precompiled.h>
#include "selectionInputController.h"

#include <core\input.h>
#include <core\mesh.h>
#include <core\multiCommand.h>

#include <rendering\pickingFramebuffer.h>

#include "pickingId.h"
#include "selectSceneObjectCommand.h"
#include "unselectSceneObjectCommand.h"

namespace phi
{
    selectionInputController::selectionInputController(meshLayerBehaviour* meshLayerBehaviour, commandsManager* commandsManager) :
        _meshLayerBehaviour(meshLayerBehaviour),
        _commandsManager(commandsManager)
    {
    }

    bool selectionInputController::onMouseDown(phi::mouseEventArgs* e)
    {
        if (!e->leftButtonPressed)
            return false;

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

            if (contains(selectedObjects, node))
                return false;

            if (selectedMesh)
            {
                _commandsManager->executeCommand(
                    new selectSceneObjectCommand(selectedMesh->getNode(), selectedObjects));
            }

            clickComponent->onClick();
        }

        return true;
    }

    bool selectionInputController::onMouseDoubleClick(phi::mouseEventArgs* e)
    {
        return false;
    }
}