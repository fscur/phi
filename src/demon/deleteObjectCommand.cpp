#include <precompiled.h>
#include "deleteObjectCommand.h"

namespace demon
{
    using namespace phi;

    deleteObjectCommand::deleteObjectCommand(scene* scene, selectionMouseController* selectionController) :
        _scene(scene),
        _selectionController(selectionController)
    {
    }

    deleteObjectCommand::~deleteObjectCommand()
    {
    }

    void deleteObjectCommand::execute()
    {
        if (!_selectionController->hasSelectedObjects())
            return;

        auto selectedObjects = _selectionController->getSelectedObjects();

        for (auto node : *selectedObjects)
        {
            auto parent = node->getParent();
            parent->removeChild(node);

            _nodesParents[node] = parent;
        }

        for (auto pair : _nodesParents)
        {
            auto it = std::find(selectedObjects->begin(), selectedObjects->end(), pair.first);

            if (it != selectedObjects->end())
                selectedObjects->erase(it);
        }
    }

    void deleteObjectCommand::executeUndo()
    {
        auto selectedObjects = _selectionController->getSelectedObjects();
        for (auto pair : _nodesParents)
        {
            pair.second->addChild(pair.first);
            selectedObjects->push_back(pair.first);
        }

        _nodesParents.clear();
    }
}