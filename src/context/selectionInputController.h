#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\node.h>
#include <application\commandsManager.h>

#include "inputController.h"

namespace phi
{
    class selectionInputController :
        public inputController
    {
    private:
        commandsManager* _commandsManager;
        vector<node*> _selectedNodes;

    private:
        bool isSelectedOrHasSelectedChildren(const node * const node);
        bool isParentSelected(const node* const node);
        bool isSonOfRoot(const node* const node);
        bool hasSelectedDescendant(const node* const node);
        bool hasModelComponentInItselfOrInDescendants(const node * const node);
        node* getSonOfFirstSelected(node* const node);
        node* findTargetNode(node* const node);

    public:
        CONTEXT_API selectionInputController(commandsManager* commandsManager);
        virtual ~selectionInputController() {}

        CONTEXT_API bool select(node* node);
        CONTEXT_API bool deselectAll();

        CONTEXT_API virtual bool onMouseUp(mouseEventArgs* e) override;
        const vector<node*>* getSelectedNodes() const { return &_selectedNodes; }
    };
}