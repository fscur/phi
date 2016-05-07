#include <precompiled.h>
#include "selectionMouseController.h"

namespace demon
{
    using namespace phi;

    selectionMouseController::selectionMouseController(scene* scene) :
        _scene(scene),
        _selectedObjects(new vector<node*>())
    {
    }

    selectionMouseController::~selectionMouseController()
    {
        safeDelete(_selectedObjects);
    }

    bool selectionMouseController::isSelectedOrHasSelectedChildren(const node* const node)
    {
        for (auto selectedObject : *_selectedObjects)
        {
            auto parent = selectedObject->getParent();
            if (parent == node || selectedObject == node)
            {
                return true;
            }
        }

        return false;
    }

    void selectionMouseController::unselectAll()
    {
        for (auto selected : *_selectedObjects)
        {
            selected->traverse<mesh>([](mesh* mesh)
            {
                mesh->setSelected(false);
            });
        }

        _selectedObjects->clear();
    }

    void selectionMouseController::select(phi::node* const node)
    {
        node->traverse<mesh>([](mesh* component)
        {
            component->setSelected(true);
        });

        _selectedObjects->push_back(node);
    }

    void selectionMouseController::onMouseDown(int mouseX, int mouseY)
    {
        auto selectedMesh = _scene->pick(mouseX, mouseY);

        if (selectedMesh)
        {
            auto selectedNode = selectedMesh->getNode();

            auto parent = selectedNode;
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
