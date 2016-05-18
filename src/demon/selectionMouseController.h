#pragma once
#include <phi.h>

#include <scenes\scene.h>

namespace demon
{
    class selectionMouseController
    {
    private:
        phi::scene* _scene;
        phi::vector<phi::node*>* _selectedObjects;

    private:
        bool isSelectedOrHasSelectedChildren(const phi::node* const node);
        void unselectAll();
        void select(phi::node* const node);

    public:
        selectionMouseController(phi::scene* scene);
        ~selectionMouseController();

        void onMouseDown(int mouseX, int mouseY);
        
        bool hasSelectedObjects() { return _selectedObjects->size() > 0; }
        phi::vector<phi::node*>* getSelectedObjects() { return _selectedObjects; }
    };
}