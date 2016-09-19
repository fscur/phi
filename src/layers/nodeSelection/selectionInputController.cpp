#include <precompiled.h>
#include "selectionInputController.h"

#include <input\input.h>
#include <core\mesh.h>
#include <core\model.h>
#include <core\multiCommand.h>

#include <rendering\pickingFramebuffer.h>

#include <context\pickingId.h>
#include "selectNodeCommand.h"
#include "unselectNodeCommand.h"

namespace phi
{
    selectionInputController::selectionInputController(commandsManager* commandsManager, layer* layer) :
        inputController(),
        _commandsManager(commandsManager),
        _layer(layer)
    {
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

        return node->getComponent<model>() != nullptr;
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

        assert(("Cannot select node tree without model component.", node));

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

    bool selectionInputController::select(node* node)
    {
        auto targetNode = findTargetNode(node);

        // TODO: testar undo
        deselectAll();
        _commandsManager->executeCommand(
            new selectNodeCommand(targetNode));
        _selectedNodes.push_back(targetNode);

        return true;
    }

    bool selectionInputController::deselectAll()
    {
        auto hasSelectedObjects = _selectedNodes.size() > 0u;
        if (hasSelectedObjects)
        {
            _commandsManager->executeCommand(
                new unselectNodeCommand(_selectedNodes));
            _selectedNodes.clear();
            return true;
        }

        return false;
    }

    bool selectionInputController::onMouseClick(mouseEventArgs * e)
    {
        if (!e->leftButtonPressed)
            return false;

        auto idOnMousePosition = pickingFramebuffer::pick(e->x, e->y);

        auto clickComponent = pickingId::get(idOnMousePosition);
        if (clickComponent)
        {
            auto clickedNode = clickComponent->getNode();
            clickComponent->onMouseUp();

            auto selectedMesh = clickedNode->getComponent<mesh>();
            if (selectedMesh)
            {
                select(clickedNode);
                _requestControlEvent->raise(this);
            }
        }
        else if (_selectedNodes.size() > 0)
            cancel();

        return false;
    }

    bool selectionInputController::onMouseDoubleClick(phi::mouseEventArgs* e)
    {
        return false;
    }

    bool selectionInputController::onMouseMove(mouseEventArgs * e)
    {
        auto root = _layer->getRoot();

        auto idOnMousePosition = pickingFramebuffer::pick(e->x, e->y);

        auto click = pickingId::get(idOnMousePosition);
        if (!click)
        {
            for (auto& child : *root->getChildren())
            {
                child->traverse<mouseInteractionComponent>([](mouseInteractionComponent* comp)
                {
                    if (comp->isMouseOver())
                        comp->onMouseLeave();
                });
            }
            return false;
        }

        auto selectedNode = click->getNode();
        while (!isSonOfRoot(selectedNode))
            selectedNode = selectedNode->getParent();

        for (auto& child : *root->getChildren())
        {
            if (child != selectedNode)
            {
                child->traverse<mouseInteractionComponent>([](mouseInteractionComponent* comp)
                {
                    if (comp->isMouseOver())
                        comp->onMouseLeave();
                });
            }
            else
            {
                child->traverse<mouseInteractionComponent>([](mouseInteractionComponent* comp)
                {
                    if (!comp->isMouseOver())
                        comp->onMouseEnter();
                });
            }
        }

        return true;
    }

    void selectionInputController::cancel()
    {
        deselectAll();
        _resignControlEvent->raise(this);
    }
}