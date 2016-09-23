#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\node.h>

#include <rendering\renderPass.h>
#include <rendering\camera.h>
#include <rendering\buffer.h>
#include <rendering\frameUniformBlock.h>

#include <input\mouseEventArgs.h>
#include <input\keyboardEventArgs.h>
#include <input\inputController.h>

namespace phi
{
    class layer
    {
    private:
        struct nodeEventTokens
        {
            nodeEventTokens(
                eventToken childAdded,
                eventToken childRemoved,
                eventToken transformChanged,
                eventToken selectionChanged,
                eventToken obbChanged) :
                childAdded(childAdded),
                childRemoved(childRemoved),
                transformChanged(transformChanged),
                selectionChanged(selectionChanged),
                obbChanged(obbChanged)
            {
            }

            eventToken childAdded;
            eventToken childRemoved;
            eventToken transformChanged;
            eventToken selectionChanged;
            eventToken obbChanged;
        };

    private:
        camera* _camera;
        node* _root;
        map<node*, nodeEventTokens*> _nodeTokens;

        vector<renderPass*> _renderPasses;
        vector<std::function<void(void)>> _onUpdate;
        vector<std::function<void(node*)>> _onNodeAdded;
        vector<std::function<void(node*)>> _onNodeRemoved;
        vector<std::function<void(node*)>> _onNodeTransformChanged;
        vector<std::function<void(node*)>> _onNodeSelectionChanged;
        vector<std::function<void(node*)>> _onNodeObbChanged;
        vector<std::function<void(void)>> _onDelete;
        vector<inputController*> _controllers;
        inputController* _currentController;
        std::stack<inputController*> _controllersStack;
        buffer* _frameUniformsBuffer;

    private:
        void createFrameUniforms();
        void updateFrameUniforms();
        void trackNode(node* node);
        void untrackNode(node * node);
        void nodeAdded(node* node);
        void nodeRemoved(node* node);
        void nodeTransformChanged(node* changedNode);
        void nodeSelectionChanged(node * changedNode);
        void nodeObbChanged(node * changedNode);

    public:
        CONTEXT_API layer(camera* camera);
        CONTEXT_API ~layer();

        CONTEXT_API void add(node* node);

        CONTEXT_API void initialize();
        CONTEXT_API void update();
        CONTEXT_API void render();

        CONTEXT_API bool onMouseDown(mouseEventArgs* e);
        CONTEXT_API bool onMouseUp(mouseEventArgs* e);
        CONTEXT_API bool onMouseClick(mouseEventArgs* e);
        CONTEXT_API bool onMouseDoubleClick(mouseEventArgs* e);
        CONTEXT_API bool onMouseMove(mouseEventArgs* e);
        CONTEXT_API bool onBeginMouseWheel(mouseEventArgs* e);
        CONTEXT_API bool onMouseWheel(mouseEventArgs* e);
        CONTEXT_API bool onEndMouseWheel(mouseEventArgs* e);
        CONTEXT_API bool onKeyDown(keyboardEventArgs* e);
        CONTEXT_API bool onKeyUp(keyboardEventArgs* e);

        void addOnUpdate(std::function<void(void)> onUpdate) { _onUpdate.push_back(onUpdate); }
        void addOnNodeAdded(std::function<void(node*)> onNodeAdded) { _onNodeAdded.push_back(onNodeAdded); }
        void addOnNodeRemoved(std::function<void(node*)> onNodeRemoved) { _onNodeRemoved.push_back(onNodeRemoved); }
        void addOnNodeTransformChanged(std::function<void(node*)> onNodeTransformChanged) { _onNodeTransformChanged.push_back(onNodeTransformChanged); }
        void addOnNodeSelectionChanged(std::function<void(node*)> onNodeSelectionChanged) { _onNodeSelectionChanged.push_back(onNodeSelectionChanged); }
        void addOnNodeObbChanged(std::function<void(node*)> onNodeObbChanged) { _onNodeObbChanged.push_back(onNodeObbChanged); }

        CONTEXT_API void addMouseController(inputController* controller);
        void addRenderPass(renderPass* renderPass) { _renderPasses.push_back(renderPass); }
        void addRenderPasses(vector<renderPass*> renderPasses) { _renderPasses.insert(_renderPasses.end(), renderPasses.begin(), renderPasses.end()); }
        void addOnDelete(std::function<void(void)> onDelete) { _onDelete.push_back(onDelete); }

        camera* getCamera() { return _camera; }

        void resize(const resolution& resolution);
        node* getRoot() { return _root; }
    };
}