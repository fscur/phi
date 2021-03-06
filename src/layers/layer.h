#pragma once
#include <phi.h>
#include "layersApi.h"

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
                eventToken obbChanged,
                eventToken componentAdded,
                eventToken componentRemoved) :
                childAdded(childAdded),
                childRemoved(childRemoved),
                transformChanged(transformChanged),
                selectionChanged(selectionChanged),
                obbChanged(obbChanged),
                componentAdded(componentAdded),
                componentRemoved(componentRemoved)
            {
            }

            eventToken childAdded;
            eventToken childRemoved;
            eventToken transformChanged;
            eventToken selectionChanged;
            eventToken obbChanged;
            eventToken componentAdded;
            eventToken componentRemoved;
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
        vector<std::function<void(node*, component*)>> _onNodeComponentAdded;
        vector<std::function<void(node*, component*)>> _onNodeComponentRemoved;
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
        void nodeComponentAdded(node* node, component* component);
        void nodeComponentRemoved(node* node, component* component);

    public:
        LAYERS_API layer(camera* camera);
        LAYERS_API ~layer();

        LAYERS_API virtual void add(node* node);

        LAYERS_API void initialize();
        LAYERS_API void update();
        LAYERS_API void render();

        LAYERS_API bool onMouseDown(mouseEventArgs* e);
        LAYERS_API bool onMouseUp(mouseEventArgs* e);
        LAYERS_API bool onMouseClick(mouseEventArgs* e);
        LAYERS_API bool onMouseDoubleClick(mouseEventArgs* e);
        LAYERS_API bool onMouseMove(mouseEventArgs* e);
        LAYERS_API bool onBeginMouseWheel(mouseEventArgs* e);
        LAYERS_API bool onMouseWheel(mouseEventArgs* e);
        LAYERS_API bool onEndMouseWheel(mouseEventArgs* e);
        LAYERS_API bool onKeyDown(keyboardEventArgs* e);
        LAYERS_API bool onKeyUp(keyboardEventArgs* e);

        void addOnUpdate(std::function<void(void)> onUpdate) { _onUpdate.push_back(onUpdate); }
        void addOnNodeAdded(std::function<void(node*)> onNodeAdded) { _onNodeAdded.push_back(onNodeAdded); }
        void addOnNodeRemoved(std::function<void(node*)> onNodeRemoved) { _onNodeRemoved.push_back(onNodeRemoved); }
        void addOnNodeTransformChanged(std::function<void(node*)> onNodeTransformChanged) { _onNodeTransformChanged.push_back(onNodeTransformChanged); }
        void addOnNodeSelectionChanged(std::function<void(node*)> onNodeSelectionChanged) { _onNodeSelectionChanged.push_back(onNodeSelectionChanged); }
        void addOnNodeObbChanged(std::function<void(node*)> onNodeObbChanged) { _onNodeObbChanged.push_back(onNodeObbChanged); }
        void addOnNodeComponentAdded(std::function<void(node*, component*)> onNodeComponentAdded) { _onNodeComponentAdded.push_back(onNodeComponentAdded); }
        void addOnNodeComponentRemoved(std::function<void(node*, component*)> onNodeComponentRemoved) { _onNodeComponentRemoved.push_back(onNodeComponentRemoved); }

        LAYERS_API void addInputController(inputController* controller);

        void addRenderPass(renderPass* renderPass) { _renderPasses.push_back(renderPass); }
        void addRenderPasses(vector<renderPass*> renderPasses) { _renderPasses.insert(_renderPasses.end(), renderPasses.begin(), renderPasses.end()); }
        void addOnDelete(std::function<void(void)> onDelete) { _onDelete.push_back(onDelete); }

        camera* getCamera() const { return _camera; }

        LAYERS_API void resize(const resolution& resolution);
        node* getRoot() { return _root; }
    };
}