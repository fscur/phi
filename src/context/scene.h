#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\mathUtils.h>
#include <core\node.h>
#include <core\mesh.h>
#include <rendering\camera.h>

#include <sceneRendering\pipeline.h>

namespace phi
{
    class scene
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
        gl* _gl;
        pipeline* _pipeline;
        camera* _camera;
        node* _sceneRoot;
        float _w;
        float _h;

        phi::map<node*, nodeEventTokens*> _nodeTokens;
        vector<node*> _selectedNodes;

    private:
        void trackNode(node* node);
        void untrackNode(node * node);
        void nodeChildAdded(node* addedChild);
        void nodeChildRemoved(node* removedChild);
        void nodeTransformChanged(node* changedNode);
        void nodeSelectionChanged(node * node);

    public:
        CONTEXT_API scene(gl* gl, float w, float h);
        CONTEXT_API ~scene();

        CONTEXT_API void update();
        CONTEXT_API void render();
        CONTEXT_API void resize(float w, float h);
        CONTEXT_API void add(node* node);
        CONTEXT_API void remove(node* node);
        CONTEXT_API mesh* pick(int mouseX, int mouseY);
        CONTEXT_API float getZBufferValue(int x, int y);

        vector<node*> getSelectedObjects() { return _selectedNodes; }
        vector<node*>* getObjects() { return _sceneRoot->getChildren(); } //gambis
        camera* getCamera() const { return _camera; }
    };
}