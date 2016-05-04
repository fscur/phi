#include <precompiled.h>
#include "selectionMouseController.h"

namespace demon
{
    using namespace phi;

    selectionMouseController::selectionMouseController(scene* scene) :
        _scene(scene)
    {
    }

    bool selectionMouseController::isSelectedOrHasSelectedChildren(const node* const node)
    {
        for (auto selectedObject : selectedObjects)
        {
            if (selectedObject->getParent() == node || selectedObject == node)
            {
                return true;
            }
        }

        return false;
    }

    void selectionMouseController::unselectAll()
    {
        for (auto selected : selectedObjects)
        {
            selected->traverse<mesh>([](mesh* mesh)
            {
                mesh->setSelected(false);
            });
        }

        selectedObjects.clear();
    }

    void selectionMouseController::select(phi::node* const node)
    {
        node->traverse<mesh>([](mesh* component)
        {
            component->setSelected(true);
        });

        selectedObjects.push_back(node);
    }

    void selectionMouseController::onMouseDown(int mouseX, int mouseY)
    {
        auto selectedMesh = _scene->pick(mouseX, mouseY);

        if (selectedMesh)
        {
            auto selectedNode = selectedMesh->getNode();

            auto parent = selectedNode->getParent();
            while (parent && !isSelectedOrHasSelectedChildren(parent))
            {
                selectedNode = parent;
                parent = parent->getParent();
            }

            unselectAll();
            select(selectedNode);
        }
        else
        {
            unselectAll();
        }
    }
}
