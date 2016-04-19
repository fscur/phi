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
        vector<node*> _objects;
        renderer* _renderer;
        pipeline* _pipeline;
        camera* _camera;
        float _w;
        float _h;

    public:
        SCENES_API scene(gl* gl, float w, float h);
        SCENES_API ~scene();
        
        SCENES_API void update();
        SCENES_API void render();
        SCENES_API void resize(float w, float h);
        SCENES_API void add(node* n);
        SCENES_API void remove(node* n);

        SCENES_API float getZBufferValue(int x, int y);
        SCENES_API camera* getCamera() const { return _camera; }
    };
}