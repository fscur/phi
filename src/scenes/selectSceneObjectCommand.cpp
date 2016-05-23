#include <precompiled.h>
#include "selectSceneObjectCommand.h"

namespace phi
{
    selectSceneObjectCommand::selectSceneObjectCommand(node* nodeToSelect, const vector<node*>& selectedObjects) :
        _unselectCommand(new unselectSceneObjectCommand(selectedObjects)),
        _selectedObjects(selectedObjects),
        _nodeToSelect(nodeToSelect)
    {
    }

    selectSceneObjectCommand::~selectSceneObjectCommand()
    {
    }

    bool selectSceneObjectCommand::isSelectedOrHasSelectedChildren(const node* const node)
    {
        if (node->getIsSelected())
            return true;

        for (auto child : *node->getChildren())
        {
            if (child->getIsSelected())
                return true;
        }

        return false;
    }

    bool selectSceneObjectCommand::hasSelectedParent(const node* const node)
    {
        return node->getParent()->getIsSelected();
    }

    bool selectSceneObjectCommand::isSonOfARoot(const node* const node)
    {
        return node->getParent()->getParent();
    }

    bool selectSceneObjectCommand::hasSelectedChildren(const node* const node)
    {
        for (auto& child : *node->getChildren())
        {
            if (hasSelectedChildren(child))
                return true;
        }

        return node->getIsSelected();
    }

    node* selectSceneObjectCommand::getSonOfFirstSelected(node* const node)
    {
        auto childOfSelected = node;
        auto grandParent = node->getParent()->getParent();

        while (grandParent && grandParent->getIsSelected())
        {
            childOfSelected = childOfSelected->getParent();
            grandParent = grandParent->getParent();
        }

        return childOfSelected;
    }

    void selectSceneObjectCommand::execute()
    {
        while (isSonOfARoot(_nodeToSelect))
        {
            if (hasSelectedParent(_nodeToSelect))
            {
                _nodeToSelect = getSonOfFirstSelected(_nodeToSelect);
                break;
            }

            if (hasSelectedChildren(_nodeToSelect->getParent()))
                break;

            _nodeToSelect = _nodeToSelect->getParent();
        }

        _unselectCommand->execute();

        _nodeToSelect->traverse([](node* node)
        {
            node->setIsSelected(true);
        });
    }

    void selectSceneObjectCommand::executeUndo()
    {
        _nodeToSelect->traverse([](node* node)
        {
            node->setIsSelected(false);
        });

        _unselectCommand->executeUndo();
    }
}
