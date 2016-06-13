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

#include "iMouseController.h"

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

        vector<std::function<void(void)>> _onUpdate;
        vector<renderPass*> _renderPasses;
        vector<std::function<void(node*)>> _onNodeAdded;
        vector<std::function<void(node*)>> _onNodeSelectionChanded;
        vector<iMouseController*> _controllers;
        map<node*, nodeEventTokens*> _nodeTokens;

        buffer* _frameUniformsBuffer;

    public:
        eventHandler<layer*> onInputChanged;

    private:
        void initialize();
        void createFrameUniforms();
        void updateFrameUniforms();
        void trackNode(node* node);
        void untrackNode(node * node);
        void nodeChildAdded(node* addedChild);
        void nodeChildRemoved(node* removedChild);
        void nodeTransformChanged(node* changedNode);
        void nodeSelectionChanged(node* node);

    public:
        layer(camera* camera);
        ~layer();

        CONTEXT_API void addOnUpdate(std::function<void(void)> updateFunction);
        CONTEXT_API void addRenderPass(renderPass* renderPass);
        CONTEXT_API void addOnNodeAdded(std::function<void(node*)> onNodeAdded);
        CONTEXT_API void addOnNodeSelectionChanged(std::function<void(node*)> onNodeSelectionChanged);
        CONTEXT_API void addMouseController(iMouseController* controller);

        CONTEXT_API void add(node* node);
        CONTEXT_API void update();
        CONTEXT_API void render();

        CONTEXT_API void onMouseDown(mouseEventArgs* e);
        CONTEXT_API void onMouseMove(mouseEventArgs* e);
        CONTEXT_API void onMouseUp(mouseEventArgs* e);
        CONTEXT_API void onMouseWheel(mouseEventArgs* e);
        CONTEXT_API void onKeyDown(keyboardEventArgs* e);
        CONTEXT_API void onKeyUp(keyboardEventArgs* e);

        unordered_map<string, renderTarget*> getOuts() { return _renderPasses.back()->getOuts(); }
        camera* getCamera() { return _camera; }

        void resize(const resolution& resolution);
    };
}