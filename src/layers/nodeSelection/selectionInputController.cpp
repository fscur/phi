#include <precompiled.h>

#include <core/mesh.h>
#include <core/modelNode.h>
#include <core/multiCommand.h>

#include <input/input.h>

#include <rendering/pickingFramebuffer.h>

#include "..\pickingId.h"

#include "selectionInputController.h"
#include "selectNodeCommand.h"
#include "deselectNodeCommand.h"

namespace phi
{
    selectionInputController::selectionInputController(commandsManager* commandsManager, selectionLayerBehaviour* selectionBehaviour) :
        inputController(),
        _commandsManager(commandsManager),
        _selectionBehaviour(selectionBehaviour),
        _isAdditiveSelection(false)
    {
        _selectionBehaviourSelectedNodesChangedEventToken =
            _selectionBehaviour->selectedNodesChanged.assign(
                std::bind(&selectionInputController::onSelectionBehaviourSelectedNodesChanged, this, std::placeholders::_1));
    }

    bool selectionInputController::isSelectedOrHasSelectedChildren(const node* const node)
    {
        if (node->isSelected())
            return true;

        for (auto child : *node->getChildren())
        {
            if (child->isSelected())
                return true;
        }

        return false;
    }

    bool selectionInputController::isParentSelected(const node* const node)
    {
        return node->getParent()->isSelected();
    }

    bool selectionInputController::isSonOfRoot(const node* const node)
    {
        return node->getParent()->getParent() == nullptr;
    }

    bool selectionInputController::hasSelectedDescendant(const node* const node)
    {
        for (auto& child : *node->getChildren())
        {
            if (hasSelectedDescendant(child))
                return true;
        }

        return node->isSelected();
    }

    bool selectionInputController::hasModelComponentInItselfOrInDescendants(const node* const node)
    {
        for (auto& child : *node->getChildren())
        {
            if (hasModelComponentInItselfOrInDescendants(child))
                return true;
        }

        return node->getComponent<modelNode>() != nullptr;
    }

    node* selectionInputController::getSonOfFirstSelected(node* const node)
    {
        auto childOfSelected = node;
        auto grandParent = node->getParent()->getParent();

        while (grandParent && grandParent->isSelected())
        {
            childOfSelected = childOfSelected->getParent();
            grandParent = grandParent->getParent();
        }

        return childOfSelected;
    }

    node* selectionInputController::findTargetNode(node* node)
    {
        while (node != nullptr && !hasModelComponentInItselfOrInDescendants(node))
            node = node->getParent();

        if (!node)
            return nullptr;

        while (!isSonOfRoot(node))
        {
            if (isParentSelected(node))
            {
                node = getSonOfFirstSelected(node);
                break;
            }

            if (hasSelectedDescendant(node->getParent()))
                break;

            node = node->getParent();
        }

        return node;
    }

    void selectionInputController::onSelectionBehaviourSelectedNodesChanged(selectionLayerBehaviour* selectionBehaviour)
    {
        _unused(selectionBehaviour);

        if (_selectionBehaviour->getSelectedNodes()->size() > 0)
            _requestControlEvent->raise(this);
        else
            _resignControlEvent->raise(this);
    }

    void selectionInputController::pushSelectCommand(node* node)
    {
        auto selectCommand = new selectNodeCommand(node);

        if (_isAdditiveSelection)
        {
            _commandsManager->executeCommand(selectCommand);
            return;
        }

        auto commands = new multiCommand(
        {
            new deselectNodeCommand(*_selectionBehaviour->getSelectedNodes()),
            selectCommand
        });

        _commandsManager->executeCommand(commands);
    }

    bool selectionInputController::onKeyDown(keyboardEventArgs* e)
    {
        if (e->key != PHIK_CTRL)
            return false;

        _isAdditiveSelection = true;
        return true;
    }

    bool selectionInputController::onKeyUp(keyboardEventArgs* e)
    {
        if (e->key != PHIK_CTRL)
            return false;

        _isAdditiveSelection = false;
        return false;
    }

    bool selectionInputController::onMouseClick(mouseEventArgs* e)
    {
        if (!e->leftButtonPressed)
            return false;

        auto idOnMousePosition = pickingFramebuffer::pick(e->x, e->y);
        auto clickComponent = pickingId::get(idOnMousePosition);
        if (!clickComponent)
        {
            if (_selectionBehaviour->getSelectedNodes()->size() > 0)
                deselectAll();

            return true;
        }

        auto clickedNode = clickComponent->getNode();
        select(clickedNode);

        return true;
    }

    void selectionInputController::cancel()
    {
        deselectAll();
    }

    void selectionInputController::select(node* node)
    {
        auto targetNode = findTargetNode(node);
        if (!targetNode)
            return;

        if (!targetNode->isSelected())
            pushSelectCommand(targetNode);
        else if (_isAdditiveSelection)
            deselect(targetNode);
    }

    void selectionInputController::deselect(node* node)
    {
        auto deselectCommand = new deselectNodeCommand({ node });
        _commandsManager->executeCommand(deselectCommand);
    }

    void selectionInputController::deselectAll()
    {
        _commandsManager->executeCommand(new deselectNodeCommand(*_selectionBehaviour->getSelectedNodes()));
    }
}