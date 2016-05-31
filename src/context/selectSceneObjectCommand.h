#pragma once
#include <phi.h>
#include "scenesApi.h"

#include <core\command.h>
#include <core\node.h>

#include "unselectSceneObjectCommand.h"

namespace phi
{
    class selectSceneObjectCommand :
        public command
    {
    private:
        unselectSceneObjectCommand* _unselectCommand;
        vector<node*> _selectedObjects;
        node* _nodeToSelect;
    private:
        bool isSelectedOrHasSelectedChildren(const node * const node);
        bool hasSelectedParent(const node* const node);
        bool isSonOfARoot(const node* const node);
        bool hasSelectedChildren(const node* const node);
        node * getSonOfFirstSelected(node* const node);
    public:
        SCENES_API selectSceneObjectCommand(node* nodeToSelect, const vector<node*>& selectedObjects);
        SCENES_API ~selectSceneObjectCommand();

        SCENES_API virtual void execute();
        SCENES_API virtual void executeUndo();
    };
}