#pragma once
#include <phi.h>

#include <core/node.h>

#include <input/inputController.h>

#include <application/commandsManager.h>

#include "../layersApi.h"
#include "selectionLayerBehaviour.h"

namespace phi
{
    class selectionInputController :
        public inputController
    {
    private:
        commandsManager* _commandsManager;
        selectionLayerBehaviour* _selectionBehaviour;
        bool _isAdditiveSelection;
        eventToken _selectionBehaviourSelectedNodesChangedEventToken;

    private:
        bool isSelectedOrHasSelectedChildren(const node * const node);
        bool isParentSelected(const node* const node);
        bool isSonOfRoot(const node* const node);
        bool hasSelectedDescendant(const node* const node);
        bool hasModelComponentInItselfOrInDescendants(const node * const node);
        node* getSonOfFirstSelected(node* const node);
        node* findTargetNode(node* const node);
        void onSelectionBehaviourSelectedNodesChanged(selectionLayerBehaviour * selectionBehaviour);

    public:
        LAYERS_API selectionInputController(commandsManager* commandsManager, selectionLayerBehaviour* selectionBehaviour);
        virtual ~selectionInputController() {}

        LAYERS_API virtual bool onKeyDown(keyboardEventArgs* e) override;
        LAYERS_API virtual bool onKeyUp(keyboardEventArgs* e) override;
        LAYERS_API virtual bool onMouseClick(mouseEventArgs* e) override;
        LAYERS_API virtual bool onMouseDoubleClick(mouseEventArgs* e) override;
        LAYERS_API virtual void cancel() override;

        LAYERS_API void select(node* node);
        LAYERS_API void deselectAll();
    };
}