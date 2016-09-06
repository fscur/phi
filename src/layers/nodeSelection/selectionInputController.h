#pragma once
#include <phi.h>
#include "..\layersApi.h"

#include <core\node.h>
#include <application\commandsManager.h>

#include <context\layer.h>

#include <input\inputController.h>

namespace phi
{
    class selectionInputController :
        public inputController
    {
    private:
        commandsManager* _commandsManager;
        vector<node*> _selectedNodes;

        layer* _layer;

    private:
        bool isSelectedOrHasSelectedChildren(const node * const node);
        bool isParentSelected(const node* const node);
        bool isSonOfRoot(const node* const node);
        bool hasSelectedDescendant(const node* const node);
        bool hasModelComponentInItselfOrInDescendants(const node * const node);
        node* getSonOfFirstSelected(node* const node);
        node* findTargetNode(node* const node);

    public:
        LAYERS_API selectionInputController(commandsManager* commandsManager, layer* layer = nullptr);
        virtual ~selectionInputController() {}

        LAYERS_API bool select(node* node);
        LAYERS_API bool deselectAll();

        LAYERS_API virtual bool onMouseClick(mouseEventArgs* e) override;
        LAYERS_API virtual bool onMouseDoubleClick(mouseEventArgs* e) override;
        LAYERS_API virtual bool onMouseMove(mouseEventArgs* e) override;
        LAYERS_API virtual void cancel() override;

        const vector<node*>* getSelectedNodes() const { return &_selectedNodes; }
    };
}