#pragma once
#include <phi.h>
#include "scenes.h"

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
        phi::gl* _gl;

        vector<node*> _objects;

        phi::renderer* _renderer;
        phi::pipeline* _pipeline;

    public:
        size_t w;
        size_t h;
        phi::camera* camera;

    public:
        SCENES_API scene(phi::gl* gl, size_t w, size_t h);
        SCENES_API ~scene();
        
        SCENES_API void update();
        SCENES_API void render();
        SCENES_API void resize(size_t w, size_t h);
        SCENES_API void add(node* n);
        SCENES_API void remove(node* n);
        SCENES_API float getZBufferValue(int x, int y);
    };
}