#pragma once
#include <phi.h>
#include "scenesApi.h"

#include <core\mathUtils.h>
#include <core\node.h>
#include <rendering\mesh.h>
#include <rendering\camera.h>
#include <rendering\pipeline.h>
#include <rendering\renderer.h>

namespace phi
{
    class scene
    {
    private:
        gl* _gl;
        pipeline* _pipeline;
        camera* _camera;
        node* _sceneRoot;
        float _w;
        float _h;
    
    private:
        void trackNode(node* node);
        void nodeChildAdded(node* addedChild);
        void nodeChildRemoved(node* removedChild);
        void nodeTransformChanged(node* changedNode);
        void meshSelectionChanged(mesh* mesh);

    public:
        SCENES_API scene(gl* gl, float w, float h);
        SCENES_API ~scene();

        SCENES_API void update();
        SCENES_API void render();
        SCENES_API void resize(float w, float h);
        SCENES_API void add(node* node);
        SCENES_API void remove(node* node);
        SCENES_API mesh* pick(int mouseX, int mouseY);
        SCENES_API float getZBufferValue(int x, int y);

        vector<node*>* getObjects() { return _sceneRoot->getChildren(); } //gambis
        renderer* getRenderer() const { return _pipeline->_renderer; }
        camera* getCamera() const { return _camera; }
    };
}