#include <precompiled.h>
#include "deleteObjectCommand.h"

namespace demon
{
    using namespace phi;

    deleteObjectCommand::deleteObjectCommand(scene* scene, selectionMouseController* selectionController) :
        _scene(scene),
        _selectionController(selectionController),
        _deletedObjects(vector<node*>())
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
            _deletedObjects.push_back(node);

            auto parent = node->getParent();
            if (parent == nullptr)
            {
                _scene->remove(node);
            }
            else
            {
                parent->removeChild(node);
                for (auto child : node->getChildren())
                    parent->addChild(child);

                _scene->remove(node); //TODO: remove when this shit gets real
            }
        }

        for (auto node : _deletedObjects)
        {
            auto it = std::find(selectedObjects->begin(), selectedObjects->end(), node);
            selectedObjects->erase(it);
        }
    }

    void deleteObjectCommand::executeUndo()
    {
        std::cout << "Ooops! Ctrl+Z not implemented for deleteObjectCommand." << std::endl;
    }
}