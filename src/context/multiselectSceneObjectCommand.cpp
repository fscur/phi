#include <precompiled.h>
#include "multiselectSceneObjectCommand.h"

namespace phi
{
    multiselectSceneObjectCommand::multiselectSceneObjectCommand(node* nodeToSelect) :
        _nodeToSelect(nodeToSelect)
    {
    }

    multiselectSceneObjectCommand ::~multiselectSceneObjectCommand()
    {
    }

    node* multiselectSceneObjectCommand::getSonOfRoot(node* node)
    {
        auto sonOfRoot = node;

        auto parent = _nodeToSelect->getParent();
        while (parent && parent->getParent())
        {
            sonOfRoot = parent;
            parent = parent->getParent();
        }

        return sonOfRoot;
    }

    void multiselectSceneObjectCommand::execute()
    {
        _nodeToSelect = getSonOfRoot(_nodeToSelect);

        _nodeToSelect->traverse([](node* node)
        {
            node->setIsSelected(true);
        });
    }

    void multiselectSceneObjectCommand::executeUndo()
    {
        _nodeToSelect->traverse([](node* node)
        {
            node->setIsSelected(false);
        });
    }
}
