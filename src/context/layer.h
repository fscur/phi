#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\keyboardEventArgs.h>
#include <core\mouseEventArgs.h>
#include <core\node.h>

#include <rendering\renderPass.h>
#include <rendering\camera.h>
#include <rendering\buffer.h>
#include <rendering\frameUniformBlock.h>

#include "mouseController.h"

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
                eventToken selectionChanged) :
                childAdded(childAdded),
                childRemoved(childRemoved),
                transformChanged(transformChanged),
                selectionChanged(selectionChanged)
            {
            }

            eventToken childAdded;
            eventToken childRemoved;
            eventToken transformChanged;
            eventToken selectionChanged;
        };

    private:
        camera* _camera;
        node* _root;
        map<node*, nodeEventTokens*> _nodeTokens;

        vector<renderPass*> _renderPasses;
        vector<std::function<void(node*)>> _onNodeAdded;
        vector<std::function<void(node*)>> _onNodeRemoved;
        vector<std::function<void(node*)>> _onNodeTransformChanged;
        vector<std::function<void(node*)>> _onNodeSelectionChanged;
        vector<std::function<void(void)>> _onDelete;
        vector<mouseController*> _controllers;

        buffer* _frameUniformsBuffer;

    private:
        void createFrameUniforms();
        void updateFrameUniforms();
        void trackNode(node* node);
        void untrackNode(node * node);
        void nodeChildAdded(node* addedChild);
        void nodeChildRemoved(node* removedChild);
        void nodeTransformChanged(node* changedNode);
        void nodeSelectionChanged(node * changedNode);

    public:
        CONTEXT_API layer(camera* camera);
        CONTEXT_API ~layer();

        CONTEXT_API void add(node* node);

        CONTEXT_API void initialize();
        CONTEXT_API void update();
        CONTEXT_API void render();

        CONTEXT_API void onMouseDown(mouseEventArgs* e);
        CONTEXT_API void onMouseMove(mouseEventArgs* e);
        CONTEXT_API void onMouseUp(mouseEventArgs* e);
        CONTEXT_API void onMouseWheel(mouseEventArgs* e);
        CONTEXT_API void onKeyDown(keyboardEventArgs* e);
        CONTEXT_API void onKeyUp(keyboardEventArgs* e);

        void addOnNodeAdded(std::function<void(node*)> onNodeAdded) { _onNodeAdded.push_back(onNodeAdded); }
        void addOnNodeRemoved(std::function<void(node*)> onNodeRemoved) { _onNodeRemoved.push_back(onNodeRemoved); }
        void addOnNodeTransformChanged(std::function<void(node*)> onNodeTransformChanged) { _onNodeTransformChanged.push_back(onNodeTransformChanged); }
        void addOnNodeSelectionChanged(std::function<void(node*)> onNodeSelectionChanged) { _onNodeSelectionChanged.push_back(onNodeSelectionChanged); }

        void addMouseController(mouseController* controller) { _controllers.push_back(controller); }
        void addRenderPass(renderPass* renderPass) { _renderPasses.push_back(renderPass); }
        void addRenderPasses(vector<renderPass*> renderPasses) { _renderPasses.insert(_renderPasses.end(), renderPasses.begin(), renderPasses.end()); }
        void addOnDelete(std::function<void(void)> onDelete) { _onDelete.push_back(onDelete); }

        unordered_map<string, renderTarget*> getOuts() { return _renderPasses.back()->getOuts(); }
        camera* getCamera() { return _camera; }

        void resize(const resolution& resolution);
    };
}