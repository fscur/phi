#pragma once
#include <phi.h>

#include <scenes\scene.h>

namespace demon
{
    class selectionMouseController
    {
    private:
        phi::scene* _scene;

    public:
        phi::vector<phi::node*> selectedObjects;

    private:
        bool isSelectedOrHasSelectedChildren(const phi::node* const node);
        void unselectAll();
        void select(phi::node* const node);

    public:
        selectionMouseController(phi::scene* scene);

        void onMouseDown(int mouseX, int mouseY);
    };
}